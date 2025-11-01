#include <stdio.h>
#include <stdint.h>
#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>

// 全局变量
pcap_t* pcapHandle = NULL;
int running = 1;

// 信号处理函数，用于优雅退出
void signal_handler(int sig) {
    printf("\n收到信号 %d，正在停止捕获...\n", sig);
    running = 0;
    if (pcapHandle) {
        pcap_breakloop(pcapHandle);
    }
}

// 数据库函数（简化版，仅打印到终端）
void database(long time, long time_point) {
    printf("[数据库] 时间戳: %ld.%06ld\n", time, time_point);
}

// 数据包处理函数
void packet_handler(__u_char *user, const struct pcap_pkthdr *header, const __u_char *packet) {
    static struct timeval base_time;
    static int first_packet = 1;
    
    struct timeval curr_time = header->ts;
    long diff_sec, diff_usec;
    
    if (first_packet) {
        base_time = curr_time;
        first_packet = 0;
        printf("=== 开始捕获数据包 ===\n");
    }
    
    if (curr_time.tv_usec >= base_time.tv_usec) {
        diff_sec = curr_time.tv_sec - base_time.tv_sec;
        diff_usec = curr_time.tv_usec - base_time.tv_usec;
    } else {
        diff_sec = curr_time.tv_sec - base_time.tv_sec - 1;
        diff_usec = curr_time.tv_usec + 1000000 - base_time.tv_usec;
    }
    
    database(diff_sec, diff_usec);
    
    // 解析以太网帧
    struct ether_header* eth_header = (struct ether_header*)packet;
    
    if (header->len < 14) {
        printf("数据包太短，不是有效的以太网帧\n");
        return;
    }
    
    // 解析MAC地址
    __u_char* src_mac = eth_header->ether_shost;
    __u_char* des_mac = eth_header->ether_dhost;
    
    char mac_src_addr[18];
    char mac_des_addr[18];
    
    snprintf(mac_src_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", 
             src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
    snprintf(mac_des_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", 
             des_mac[0], des_mac[1], des_mac[2], des_mac[3], des_mac[4], des_mac[5]);
    
    // 解析以太网类型
    unsigned short ether_type = *(unsigned short*)(packet + 12);
    ether_type = ntohs(ether_type);
    
    // 解析优先级
    unsigned char priority_byte = packet[12];
    int priority = (priority_byte >> 5) & 0x07;
    
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 数据包 #%d - 长度: %d 字节                                │\n", 
           (int)(long)user + 1, header->len);
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ 源MAC: %-17s  目的MAC: %-17s │\n", mac_src_addr, mac_des_addr);
    printf("│ 以太网类型: 0x%04x        优先级: %d                    │\n", ether_type, priority);
    printf("│ 帧长度: %-4d 字节  载荷长度: %-4d 字节              │\n", 
           header->len, header->len - 14);
    
    // 如果是IP数据包，解析IP头部
    if (ether_type == 0x0800) {
        struct ip* ip_header = (struct ip*)(packet + 14);
        
        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);
        
        int ip_header_len = ip_header->ip_hl << 2;
        int version = (ip_header->ip_hl & 0xF0) >> 4;
        int total_length = ntohs(ip_header->ip_len);
        int ip_identifier = ntohs(ip_header->ip_id);
        
        // 解析IP标志位
        const __u_char *ip_header_flag = packet + 14;
        unsigned char flags_byte = ip_header_flag[5];
        int reserved_bit = (flags_byte >> 7) & 0x01;
        int df_bit = (flags_byte >> 6) & 0x01;
        int mf_bit = (flags_byte >> 5) & 0x01;
        uint16_t flags_and_offset = (ip_header_flag[6] << 8) | ip_header_flag[7];
        int fragment_offset = (flags_and_offset & 0x1FFF) * 8;
        int time_to_live = ip_header->ip_ttl;
        
        printf("│ 源IP: %-15s  目的IP: %-15s       │\n", src_ip, dst_ip);
        printf("│ IP版本: %d  头部长度: %d 字节  总长度: %d 字节 │\n", 
               version, ip_header_len, total_length);
        printf("│ ID: %-5d  标志位: R:%d D:%d M:%d  分片偏移: %d    │\n", 
               ip_identifier, reserved_bit, df_bit, mf_bit, fragment_offset);
        printf("│ TTL: %-3d  服务类型: 0x%02x  协议: %d  校验和: 0x%04x │\n", 
               time_to_live, ip_header->ip_tos, ip_header->ip_p, ip_header->ip_sum);
    }
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    // 显示数据包内容（十六进制和ASCII）
    printf("\n数据包内容:\n");
    printf("偏移量  十六进制值                              ASCII表示\n");
    printf("──────  ──────────────────────────────────────  ──────────────\n");
    
    int offset = 0;
    for (int i = 0; i < header->len; i++) {
        if (i % 16 == 0) {
            printf("%04x   ", offset);
        }
        printf("%02x ", packet[i]);
        
        if (((i + 1) % 8 == 0 && i != header->len - 1)) {
            printf(" ");
        }
        
        offset++;
        
        if ((i + 1) % 16 == 0) {
            printf("  ");
            for (int j = i - 15; j <= i; j++) {
                if (isprint(packet[j])) {
                    printf("%c", packet[j]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    
    if (header->len % 16 != 0) {
        for (int i = header->len; i < (header->len / 16 + 1) * 16; i++) {
            printf("   ");
            if ((i + 1) % 8 == 0 && i != header->len - 1) {
                printf(" ");
            }
        }
        printf("  ");
        for (int j = (header->len / 16) * 16; j < header->len; j++) {
            if (isprint(packet[j])) {
                printf("%c", packet[j]);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
    printf("\n");
    
    // 更新用户数据（包计数器）
    *((long*)user) = *((long*)user) + 1;
}

// 显示可用的网络设备
void list_devices() {
    pcap_if_t* allDevs;
    pcap_if_t* dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    int i = 0;
    
    printf("扫描可用的网络设备...\n");
    
    if (pcap_findalldevs(&allDevs, errbuf) == -1) {
        fprintf(stderr, "扫描网络设备时出错: %s\n", errbuf);
        return;
    }
    
    printf("\n可用的网络设备:\n");
    printf("┌─────┬─────────────────┬─────────────────────────────────────┐\n");
    printf("│ 编号 │ 设备名称        │ 设备描述                            │\n");
    printf("├─────┼─────────────────┼─────────────────────────────────────┤\n");
    
    for (dev = allDevs, i = 0; dev != NULL; dev = dev->next, i++) {
        printf("│ %-3d │ %-15s │ %-35s │\n", 
               i, dev->name, dev->description ? dev->description : "无描述");
    }
    
    printf("└─────┴─────────────────┴─────────────────────────────────────┘\n");
    pcap_freealldevs(allDevs);
}

// 开始捕获数据包
void start_capture(const char* device) {
    char errbuf[PCAP_ERRBUF_SIZE];
    long packet_count = 0;
    
    printf("正在打开设备: %s\n", device);
    
    pcapHandle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (!pcapHandle) {
        fprintf(stderr, "打开设备失败: %s\n", errbuf);
        return;
    }
    
    printf("开始捕获数据包... (按Ctrl+C停止)\n");
    printf("===============================================================\n");
    
    // 设置信号处理
    signal(SIGINT, signal_handler);
    
    // 开始捕获循环
    pcap_loop(pcapHandle, -1, packet_handler, (__u_char*)&packet_count);
    
    printf("\n捕获已停止。总共捕获了 %ld 个数据包。\n", packet_count);
    
    if (pcapHandle) {
        pcap_close(pcapHandle);
        pcapHandle = NULL;
    }
}

// 显示使用说明
void show_usage(const char* program_name) {
    printf("HALF 终端版本 - 混合分析层框架\n");
    printf("用法: %s [选项]\n", program_name);
    printf("\n选项:\n");
    printf("  -l, --list        列出所有可用的网络设备\n");
    printf("  -d, --device <设备名>  指定要捕获的设备\n");
    printf("  -h, --help        显示此帮助信息\n");
    printf("\n示例:\n");
    printf("  %s -l                    # 列出所有设备\n", program_name);
    printf("  %s -d eth0               # 在eth0设备上开始捕获\n", program_name);
    printf("  %s -d wlan0              # 在wlan0设备上开始捕获\n", program_name);
}

int main(int argc, char* argv[]) {
    printf("HALF 终端版本 v1.0\n");
    printf("==================\n");
    
    if (argc == 1) {
        show_usage(argv[0]);
        return 0;
    }
    
    const char* target_device = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            list_devices();
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0) {
            if (i + 1 < argc) {
                target_device = argv[i + 1];
                i++; // 跳过设备名参数
            } else {
                fprintf(stderr, "错误: 需要指定设备名称\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "错误: 未知选项 '%s'\n", argv[i]);
            show_usage(argv[0]);
            return 1;
        }
    }
    
    if (target_device) {
        start_capture(target_device);
    } else {
        // 如果没有指定设备，先列出设备让用户选择
        list_devices();
        printf("\n请使用 -d 选项指定要捕获的设备名称。\n");
    }
    
    return 0;
}