#include<stdio.h>                   //标准输入输出头文件
#include<stdint.h>
#include<pcap/pcap.h>               //网络数据包捕获头文件
#include<netinet/ip.h>              //处理IP相关操作的头文件
#include<netinet/tcp.h>             //传输与控制协议
#include<net/ethernet.h>            //处理以太网相关操作的头文件
#include<jni.h>                     //Java本地方法头文件
#include<mysql/mysql.h>             //MySql API
#include<string.h>                  //字符串处理头文件
#include<pthread.h>
#include<mysql/mysql.h>
#include"HALF_half.h"               //包含了C端所有JNI方法的函数声明

int database(long time, long time_point);

struct 
{
    char* Source_Mac;
    char* Destination_Mac;
    unsigned short Ether_Type;
}packetdata;

struct 
{
    const char* name;
}mid;

struct timeval first_ts;

JavaVM* jvm;
jobject globalCallback = NULL;
pcap_t* pcapHandle = NULL;
pthread_t captureThread;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jvm = vm;
    return JNI_VERSION_1_6;
}

struct timeval base_time;
int first_packet = 1;

void packet_handler(__u_char *user, const struct pcap_pkthdr *header, const __u_char *packet)
{
    struct timeval curr_time = header->ts;
    long diff_sec, diff_usec;
    if (first_packet)
    {
        base_time = curr_time;
        first_packet = 0;
    }
    if (curr_time.tv_usec >= base_time.tv_usec)
    {
        diff_sec = curr_time.tv_sec - base_time.tv_sec;
        diff_usec = curr_time.tv_usec - base_time.tv_usec;
    }
    else
    {
        diff_sec = curr_time.tv_sec - base_time.tv_sec - 1;
        diff_usec = curr_time.tv_usec + 1000000 - base_time.tv_usec;
    }
    database(diff_sec, diff_usec);
    printf("%ld.%06ld\n", diff_sec, diff_usec);

    struct ether_header* eth_header = (struct ether_header*)packet;
    struct ip* ip_header = (struct ip*)(packet + 14);

    __u_char* src_mac = eth_header->ether_shost;
    __u_char* des_mac = eth_header->ether_dhost;

    char* mac_src_addr = (char*)malloc(18 * sizeof(char));
    char* mac_des_addr = (char*)malloc(18 * sizeof(char));

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

    snprintf(mac_src_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
    snprintf(mac_des_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", des_mac[0], des_mac[1], des_mac[2], des_mac[3], des_mac[4], des_mac[5]);

    if (header->len < 14)
    {
        printf("Packet is too short to be Ethernet Frame\n");
    }
    unsigned short ether_type = *(unsigned short*)(packet + 12);
    ether_type = ntohs(ether_type);
    unsigned char priority_byte = packet[12];
    int priority = (priority_byte >> 5) & 0x07;

    int ip_header_len = ip_header -> ip_hl << 2;
    int version = (ip_header->ip_hl & 0xF0) >> 4;
    int total_length = ntohs(ip_header->ip_len);
    int ip_identifier = ntohs(ip_header->ip_id);

    const __u_char *ip_header_flag = packet + 14;
    unsigned char flags_byte = ip_header_flag[5];
    int reserved_bit = (flags_byte >> 7) & 0x01;
    int df_bit = (flags_byte >> 6) & 0x01;
    int mf_bit = (flags_byte >> 5) & 0x01;
    uint16_t flags_and_offset = (ip_header_flag[6] << 8) | ip_header_flag[7];
    int fragment_offset =(flags_and_offset & 0x1FFF) * 8;
    int time_to_live = ip_header->ip_ttl;

    JNIEnv* env;
    (*jvm)->AttachCurrentThread(jvm, (void**)&env, NULL);
    jobject callback = (jobject)user;
    jclass callbackClass = (*env)->GetObjectClass(env, callback);
    jmethodID onPacketMethod = (*env)->GetMethodID(env, callbackClass, "Packet", "([BLjava/lang/String;Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;I[BIIIIIIIIIJJ)V");
    jbyte bytes[4];
    bytes[0] = (jbyte)(total_length >> 24);
    bytes[1] = (jbyte)(total_length >> 16);
    bytes[2] = (jbyte)(total_length >> 8);
    bytes[3] = (jbyte)total_length;
    jbyteArray jPacket = (*env)->NewByteArray(env, header->len);
    jbyteArray jIptollen = (*env)->NewByteArray(env, 4);
    jstring jSrcMac = (*env)->NewStringUTF(env, mac_src_addr);
    jstring jDesMac = (*env)->NewStringUTF(env, mac_des_addr);
    jstring jSrcip = (*env)->NewStringUTF(env, src_ip);
    jstring jDstip = (*env)->NewStringUTF(env, dst_ip);

    (*env)->SetByteArrayRegion(env, jPacket, 0, header->len, (jbyte*)packet);
    (*env)->SetByteArrayRegion(env, jIptollen, 0, 4, bytes);
    (*env)->CallVoidMethod(env, callback, onPacketMethod, jPacket, jSrcMac, jDesMac, ether_type, priority, jSrcip, jDstip, ip_header_len, jIptollen, ip_identifier, reserved_bit, df_bit, mf_bit, fragment_offset, time_to_live, ip_header->ip_tos, ip_header->ip_p, ip_header->ip_sum, diff_sec, diff_usec);
    (*env)->DeleteLocalRef(env, jPacket);
    (*env)->DeleteLocalRef(env, jIptollen);
    (*env)->DeleteLocalRef(env, callbackClass);
    (*jvm)->DetachCurrentThread(jvm);
    printf("C : src_mac: %s\t des_mac: %s\t Ethernet Type: 0x%04x\t Frame Length: %d\t Payload Length: %d\t Priority: %d\n\n", mac_src_addr, mac_des_addr, ether_type, header->len, header->len - 14, priority);
    printf("src_ip: %s\t dst_ip: %s\t ip_version: %d\t Header Length: %d\t Total Length: %d\t ip_id: %d\t ip_flags: R:%dD:%dM:%d\t ip_fragment_offset: %d\n time_to_live: %d\t type_of_service: 0x%02x\t, protocol: %d\t checksum: 0x%04x\n\n", src_ip, dst_ip, version, ip_header_len, total_length, ip_identifier, reserved_bit, df_bit, mf_bit, fragment_offset, time_to_live, ip_header->ip_tos, ip_header->ip_p, ip_header->ip_sum);

    int offset = 0;
    for (int i = 0; i < header->len; i++)
    {
        if (i % 16 == 0)
        {
            printf("%04x\t", offset);
        }
        printf("%02x ", packet[i]);
        if (((i + 1) % 8 == 0 && i != header->len - 1))
        {
            printf(" ");
        }
        offset++;
        if ((i + 1) % 16 == 0)
        {
            printf(" ");
            for (int j = i - 15; j <= i; j++)
            {
                if (isprint(packet[j]))
                {
                    printf("%c", packet[j]);
                }
                else
                {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    if (header->len % 16 != 0)
    {
        for (int i = header->len; i < (header->len / 16 + 1) * 16; i++)
        {
            printf("   ");
            if ((i + 1) % 8 == 0 && i != header->len -1)
            {
                printf(" ");
            }
        }
        printf(" ");
        for (int j = (header->len / 16) * 16; j < header->len; j++)
        {
            if (isprint(packet[j]))
            {
                printf("%c", packet[j]);
            }
            else
            {
                printf(".");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void* capture_thread(void* arg)
{
    JNIEnv *env;
    (*jvm)->AttachCurrentThread(jvm, (void**)&env, NULL);
    const char *device = (const char*)arg;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcapHandle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (pcapHandle)
    {
        pcap_loop(pcapHandle, -1, packet_handler, (__u_char*)globalCallback);
    }
    (*jvm)->DetachCurrentThread(jvm);
    return NULL;
}

JNIEXPORT void JNICALL Java_HALF_half_startCapture(JNIEnv* env, jobject instance, jstring jdevice, jobject callback) 
{
    const char* device = (*env)->GetStringUTFChars(env, jdevice, 0);
    globalCallback = (*env)->NewGlobalRef(env, callback);
    pthread_create(&captureThread, NULL, capture_thread, (void*)device);
    //(*env)->ReleaseStringUTFChars(env, jdevice, device);
}

JNIEXPORT void JNICALL Java_HALF_half_stopCapture(JNIEnv* env, jobject instance)
{
    if (pcapHandle)
    {
        pcap_breakloop(pcapHandle);
        pcap_close(pcapHandle);
        pcapHandle = NULL;
    }
    if (globalCallback)
    {
        (*env)->DeleteGlobalRef(env, globalCallback);
        globalCallback = NULL;
    }
}


//jni方法函数，获取所有扫描到的网络设备名称并传输给Java端
JNIEXPORT jobjectArray JNICALL Java_HALF_half_DevN(JNIEnv* env, jobject obj)
{
    pcap_if_t* AllDev;              //网络接口信息结构体
    pcap_if_t* Dev;                 //用于操作的网络接口结构体变量
    char errbuf[PCAP_ERRBUF_SIZE];  //用于存储运行出现错误信息的缓冲区数组
    
    //用于扫描寻找所有网络设备的函数
    if (pcap_findalldevs(&AllDev, errbuf) == -1)
    {
        //标准错误输出流，输出扫描网络设备时出现的错误
        fprintf(stderr, "Error in findalldevs %s\n", errbuf);
        pcap_freealldevs(AllDev);   //释放所有网络设备所占用的内存
        return 0;                   //程序终止，返回
    }
    else
    {
        int i = 0;                  //统计网络设备数量
        //jobjectArray表示Java中的对象数组
        jobjectArray Allname = (*env)->NewObjectArray(env, sizeof(AllDev) + 1, (*env)->FindClass(env, "java/lang/String"), NULL);
        //遍历已经存储了查找到的网络设备的结构体变量
        for (Dev = AllDev; Dev != NULL; Dev = Dev->next)
        {
            //jstring类用于表示java中的String类
            jstring name = (*env)->NewStringUTF(env, Dev->name);
            //设置jobjectArray对象数组的值
            (*env)->SetObjectArrayElement(env, Allname, i++, name);
        }
        pcap_freealldevs(AllDev);   //释放所有网络设备所占用的内存
        return Allname;             //程序终止，返回
    }                       
}

//jni方法函数，获取所有扫描到的网络设备描述并传输给Java端
JNIEXPORT jobjectArray JNICALL Java_HALF_half_DevD(JNIEnv* env, jobject obj)
{
    pcap_if_t* AllDev;              //网络接口信息结构体
    pcap_if_t* Dev;                 //用于操作的网络接口结构体变量
    char errbuf[PCAP_ERRBUF_SIZE];  //用于存储运行出现错误信息的缓冲区数组
    
    //用于扫描寻找所有网络设备的函数
    if (pcap_findalldevs(&AllDev, errbuf) == -1)
    {
        //标准错误输出流，输出扫描网络设备时出现的错误
        fprintf(stderr, "Error in findalldevs %s\n", errbuf);
        pcap_freealldevs(AllDev);   //释放所有网络设备所占用的内存
        return 0;                   //程序终止，返回
    }
    else
    {
        int i = 0;                  //统计网络设备数量
        //jobjectArray表示Java中的对象数组
        jobjectArray Alldescription = (*env)->NewObjectArray(env, sizeof(AllDev) + 1, (*env)->FindClass(env, "java/lang/String"), NULL);
        //遍历已经存储了查找到的网络设备的结构体变量
        for (Dev = AllDev; Dev != NULL; Dev = Dev->next)
        {
            //jstring类用于表示java中的String类
            jstring description = (*env)->NewStringUTF(env, Dev->description);
            //判断设备描述是否为空，为空则将jstring变量设置为一个内容为“NULL”的字符串，否则将java将检测到空指针
            if (Dev->description == NULL)
            {
                //将jstring变量设置为一个内容为“NULL”的字符串
                description = (*env)->NewStringUTF(env, "NULL");
            }
            //设置jobjectArray对象数组的值
            (*env)->SetObjectArrayElement(env, Alldescription, i++, description);
        }
        pcap_freealldevs(AllDev);   //释放所有网络设备所占用的内存
        return Alldescription;      //程序终止，返回
    }                       
}

int database(long time, long time_point)
{
    MYSQL* conn;
    //MYSQL_RES* res;
    //MYSQL_ROW* row;
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS PacketData ("
                                   "id INT AUTO_INCREMENT PRIMARY KEY,"
                                   "time BIGINT NOT NULL DEFAULT 0.0,"
                                   "time_point BIGINT NOT NULL DEFAULT 0.0"
                                   ")";

    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, "localhost", "root", NULL, NULL, 0, NULL, 0))
    {
        printf("MySql Connect!\n");
        if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS Packet")) 
        {
            fprintf(stderr, "创建数据库失败：%s\n", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        if (mysql_query(conn, "USE Packet")) 
        {
            fprintf(stderr, "选择数据库失败：%s\n", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        if (mysql_query(conn, "USE Packet")) 
        {
            fprintf(stderr, "选择数据库失败：%s\n", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        else
        {
            if (mysql_query(conn, createTableQuery))
            {
                fprintf(stderr, "数据表创建失败: %s\n", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }    
        }
    }
    else 
    {
        fprintf(stderr, "Connect Failed:%s\n", mysql_error(conn));
    }

    char insertQuery[100];
    sprintf(insertQuery, "INSERT INTO PacketData (time, time_point) VALUES ('%ld', '%ld')", time, time_point);
    if (mysql_query(conn, insertQuery) != 0) 
    {
        fprintf(stderr, "插入数据时出错: %s\n", mysql_error(conn));
        return 1;
    }
    printf("成功插入数据\n");
    mysql_close(conn);
    return 0;
}

//主函数
int main(int argc, char const *argv[])
{

    return 0;
}
