package HALF;

import javax.swing.BoxLayout;   //导入javax.swing中的BoxLayout类，BoxLayout用于按照水平或垂直方向排列组件
import javax.swing.JFrame;      //导入javax.swing中的JFrame类，JFrame是用于创建窗口的顶级容器
import javax.swing.JLabel;      //导入javax.swing中的JLabel类，JLabel用于显示文本或图像等不可编辑的显示信息
import javax.swing.JMenu;       //导入javax.swing中的JMenu类，JMenu用于创建菜单
import javax.swing.JMenuBar;    //导入javax.swing中的JMenuBar类，JMenuBar用于创建菜单栏
import javax.swing.JMenuItem;   //导入javax.swing中的JMenuItem类，JMenuItem用于创建菜单项
import javax.swing.JPanel;      //导入javax.swing中的JPanel类，JPanel类是一种轻量级容器，可用于组织和容纳其他组件
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.SwingWorker;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
//导入javax.swing.border中的TitledBorder类，TitledBorder用于创建带有标题的边框
import javax.swing.table.DefaultTableModel;
import java.awt.*;              //导入java.awt包下的所有类，java.awt是抽象窗口工具包，提供了图形用户界面（GUI）编程的基本类
import java.awt.event.ActionEvent;
                                //导入java.awt.event中的ActionEent类，ActionEvent用于封装动作事件
import java.awt.event.ActionListener;
                                //导入java.awt.event中的ActionListener接口，ActionListener用于监听动作事件
import java.awt.event.MouseAdapter;
                                //导入java.awt.event中的MouseAdapter类，MouseAdapter是一个抽象类，用于接收鼠标事件
import java.awt.event.MouseEvent;
                                //导入java.awt.event中的MouseEvent类，MouseEvent用于封装鼠标操作所从产生的事件
import java.util.ArrayList;
import java.util.List;

public class half {             //类名
    static{
        //加载JNI方法C语言动态运行库
        System.load("/home/senk-j/桌面/456/HALF/NEW-HALF/src/half.so");
    }
    //本地方法声明
    //DevN为设备名字
    private native String[] DevN();
    //DevD为设备描述
    private native String[] DevD();
    public interface PacketCallback {
        void Packet(byte[] length, String src_mac, String des_mac, int ether_type, int priority, String ip_src, String ip_dst, int ip_head_length, byte[] ip_len, int ip_id, int reserved_bit, int df_bit, int mf_bit, int fragment_offset, int time_to_live, int type_of_service, int protocol, int checksum, long diff_sec, long diff_usec);
    }
    public native void startCapture(String device, PacketCallback callback);
    public native void stopCapture();
    //主函数
    public static void main(String[] args) {
        half half = new half(); //实例化类half
        GUI gui = new GUI();    //实例化类GUI
        DATA data = new DATA();
        //实例化有关GUI的类
        GUI_MENUBAR gui_menubar = new GUI_MENUBAR();
        GUI_MENUBAR_MENU gui_menubar_menu = new GUI_MENUBAR_MENU();
        GUI_PANEL gui_panel_Name = new GUI_PANEL();
        GUI_PANEL gui_panel_Description = new GUI_PANEL();
        GUI_BORDER gui_border_Name = new GUI_BORDER("Name");
        GUI_BORDER gui_border_Description = new GUI_BORDER("Description");
        GUI_PANEL_LABEL_FONT gui_panel_label_font = new GUI_PANEL_LABEL_FONT("Arial", Font.BOLD, 16);
        GUI_MENUBAR_MENU_MENUITEM gui_menubar_menu_menuitem = new GUI_MENUBAR_MENU_MENUITEM(gui, gui_panel_Name, gui_panel_Description);      
        
        GUI_PANEL_PAYLOAD gui_panel_payload = new GUI_PANEL_PAYLOAD();

        GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER gui_scrollpane_table_model = new GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER();
        GUI_SCROLLPANE_TABLE_DATA_LINK_LAYER gui_scorllpane_table = new GUI_SCROLLPANE_TABLE_DATA_LINK_LAYER(gui_scrollpane_table_model, gui_panel_payload, gui);
        GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER gui_scrollpane = new GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER(gui_scorllpane_table);
        GUI_JTABBEDPANE_DATA_LINK_LAYER gui_jtabbedpane = new GUI_JTABBEDPANE_DATA_LINK_LAYER(gui_scrollpane);        

        GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER gui_scrollpane_table_model_network_layer = new GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER();
        GUI_SCROLLPANE_TABLE_NETWORK_LAYER gui_scrollpane_table_network_layer = new GUI_SCROLLPANE_TABLE_NETWORK_LAYER(gui_scrollpane_table_model_network_layer);
        GUI_JTABBEDPANE_SCROLLPANE_NETWORK_LAYER gui_jtabbedpane_scrollpane_network_layer = new GUI_JTABBEDPANE_SCROLLPANE_NETWORK_LAYER(gui_scrollpane_table_network_layer);
        
  

        //对String类数组赋值
        String[] devname = half.DevN();
        String[] devdescription = half.DevD();
        //调用类GUI类相关的方法
        gui_menubar_menu.add(gui_menubar_menu_menuitem);
        gui_menubar.add(gui_menubar_menu);
        gui.setJMenuBar(gui_menubar);
        gui_menubar_menu.init("menu");
        gui_panel_Name.Name_init();
        gui_panel_Description.Description_init();
        gui_border_Name.setTitleFont(gui_panel_label_font);
        gui_border_Description.setTitleFont(gui_panel_label_font);
        gui_panel_Name.setBorder(gui_border_Name);
        gui_panel_Description.setBorder(gui_border_Description);
        //调用类GUI中的init()方法
        gui.init();
        //调用类JFrame中的add方法将gui_panel实例化类添加到gui实例化类中
        gui.add(gui_panel_Name);
        gui.add(gui_panel_Description);
        //取得String类数组所有值
        for(String index : devname) {
            //实例化类GUI_PANEL_LABEL
            GUI_PANEL_LABEL gui_panel_label_Name = new GUI_PANEL_LABEL(gui, gui_jtabbedpane, gui_scrollpane, gui_scrollpane_table_model, gui_jtabbedpane_scrollpane_network_layer, gui_scrollpane_table_model_network_layer, gui_panel_payload);
            //调用GUI_PANEL_LABEL类中的setFont方法
            gui_panel_label_Name.setFont(gui_panel_label_font);
            //调用GUI_PANEL_LABEL类中的NameLoad方法加载网络设备名
            gui_panel_label_Name.NameLoad(index);
            //调用GUI_PANEL中的add方法
            gui_panel_Name.add(gui_panel_label_Name);
        }
        for(String index : devdescription) {
            JLabel Description = new JLabel();
            //调用GUI_PANEL_LABEL类中的setFont方法
            Description.setFont(gui_panel_label_font);
            //调用GUI_PANEL_LABEL类中的DescriptionLoad方法加载网络设备描述
            Description.setText(index);
            //调用GUI_PANEL中的add方法
            gui_panel_Description.add(Description);
        }
        data.init();
    }
}

//自定义类GUI继承类JFrame
class GUI extends JFrame {
    //公开的实例化方法init()
    public void init() {
    //this指代本类
        //设置JFrame窗口的初始位置、大小
        this.setBounds(100, 50, 1600, 600);
        //设置JFrame窗口的可见性
        this.setVisible(true);
        //设置JFrame窗口布局为空
        this.setLayout(null);
        //设置JFrame窗口的默认关闭操作，窗口关闭时，程序终止运行
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //设置标题
        this.setTitle("GUI");
    }
}

//自定义类GUI_PANEL继承类JPanel
class GUI_PANEL extends JPanel {
    //公开的实例化方法init()
    public void Name_init() {
    //this指代本类
        //设置布局为空
        this.setLayout(null);
        //设置JPanel容器在JFrame窗口中的初始位置、大小
        this.setBounds(0, 0, 200, 250);
        //设置布局方式为BoxLayout，BoxLayout用于按照水平或垂直方向排列组件
        this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    }
    //公开的实例化方法init()
    public void Description_init() {
    //this指代本类
        //设置布局为空
        this.setLayout(null);
        //设置JPanel容器在JFrame窗口中的初始位置、大小
        this.setBounds(200, 0, 400, 250);
        //设置布局方式为BoxLayout，BoxLayout用于按照水平或垂直方向排列组件
        this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    }
}

//自定义类GUI_PANEL_LABEL继承类JLabel
class GUI_PANEL_LABEL extends JLabel {
    public half half = new half();
    public DATA data = new DATA();
    public int start = 0;
    public String src_mac;
    private boolean isSelected = false;
    //公开的实例化方法NameLoad()，接收网络设备名
    public void NameLoad(String index) {
    //this指代本类
        //设置JLabel类的文本信息
        this.setText(index);
    }
    //公开的实例化方法DescriptionLoad()，接收网络设备描述
    public void DescriptionLoad(String index) {
    //this指代本类
        //设置JLabel类的文本信息
        this.setText(index);
    }
    //为当前组件添加鼠标监听器
    public GUI_PANEL_LABEL(GUI gui, GUI_JTABBEDPANE_DATA_LINK_LAYER gui_jtabbedpane ,GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER gui_jtabbedpane_scrollpane, GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER gui_scrollpane_table_model, GUI_JTABBEDPANE_SCROLLPANE_NETWORK_LAYER gui_jtabbedpane_scrollpane_network_layer, GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER gui_scrollpane_table_model_network_layer, GUI_PANEL_PAYLOAD gui_panel_payload) {
        //重写鼠标点击事件处理方法
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                isSelected = true;
                String name = getText();
                SwingWorker<Void, DATA> worker = new SwingWorker<Void, DATA> () {
                    @Override
                    protected Void doInBackground() throws Exception {
                        half.startCapture(name, new half.PacketCallback() {
                            @Override
                            public void Packet(byte[] payload, String src_mac, String des_mac, int ether_type, int priority, String ip_src, String ip_dst, int ip_head_length, byte[] ip_len, int ip_id, int reserved_bit, int df_bit, int mf_bit, int fragment_offset, int time_to_live, int type_of_service, int protocol, int checksum, long diff_sec, long diff_usec) {
                                String hex = Integer.toHexString(ether_type);
                                String type_of_service_hex = Integer.toHexString(type_of_service);
                                String type_of_service_index = "0x" + type_of_service_hex;
                                String protocol_hex = Integer.toHexString(protocol);
                                String protocol_index = "0x" + protocol_hex;
                                String checksum_hex = Integer.toHexString(checksum);
                                String checksum_index = "0x" + checksum_hex;
                                while (hex.length() < 4) {
                                    hex = "0" + hex;
                                }
                                String index = "0x" + hex;
                                int version = 0;
                                switch (index) {
                                    case "0x0806" :
                                        index = "ARP";
                                        break;
                                    case "0x0800" :
                                        index = "IPV4";
                                        version = 4;
                                        break;
                                    case "0x86dd" :
                                        index = "IPV6";
                                        version = 6;
                                        break;
                                    default:
                                        index = "Unknown";
                                        break;
                                }
                                switch (protocol) {
                                    case 6:
                                        protocol_index = "TCP";
                                        break;
                                    case 17:
                                        protocol_index = "UDP";
                                    case 69:
                                        protocol_index = "TFTP";
                                        break;
                                    default:
                                        protocol_index =  Integer.toString(Integer.parseInt(protocol_index.substring(2), 16));
                                        break;
                                }
                                int totallength = ((ip_len[0] & 0xFF) << 24) | ((ip_len[1] & 0xFF) << 16) | ((ip_len[2] & 0xFF) << 8) | ((ip_len[3] & 0xFF));
                                gui_scrollpane_table_model.setdata(src_mac, des_mac, hex, index, payload.length, payload.length - 14, priority, diff_sec, diff_usec);
                                gui_scrollpane_table_model_network_layer.setdata(ip_src, ip_dst, version, ip_head_length,totallength, ip_id, reserved_bit, df_bit, mf_bit, fragment_offset, time_to_live, type_of_service_index, protocol_index, checksum_index);
                                gui_panel_payload.bytestream.add(payload);
                            }
                        });
                        try {
                            Thread.sleep(60000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        return null;
                    }
                };
                worker.execute();
                //清理JFrame中所有组件
                gui.getContentPane().removeAll();
                //重载JFrame组件
                gui.getContentPane().revalidate();
                //重绘制
                gui.getContentPane().repaint();
                //设置JFrame标题
                gui.setTitle("Current Port: " + getText());
                gui_jtabbedpane.setBounds(0, 0, 1600, 300);
                gui_jtabbedpane.addTab("Data Link Layer Attributes" ,gui_jtabbedpane_scrollpane);
                gui_jtabbedpane.addTab("NetWork Layer Attributes", gui_jtabbedpane_scrollpane_network_layer);
                gui.add(gui_jtabbedpane);
                gui.add(gui_panel_payload);
                //切换选中状态
                isSelected = !isSelected;
                //根据选中状态设置背景颜色
                if (isSelected) {
                    //设置Background为GRAY
                    setBackground(null);
                } else {
                    //设置Background为null
                    setBackground(null);
                }
            }
            //重写鼠标进入事件处理方法
            @Override
            public void mouseEntered(MouseEvent e) {
                gui.setTitle(getText());
                //设置Background为LIGHT_GRAY
                setBackground(Color.LIGHT_GRAY);
                //设置鼠标光标为手型
                setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            }
            //重写鼠标离开事件处理方法
            @Override
            public void mouseExited(MouseEvent e) {
                if (!isSelected) {
                    //设置Background为null
                    setBackground(null);
                    //设置鼠标光标为默认样式
                    setCursor(Cursor.getDefaultCursor());
                    gui.setTitle("GUI");
                }
            }
        });
        //设置组件为不透明
        setOpaque(true); 
    }
}

//自定义类GUI_BORDER继承自Font类
class GUI_BORDER extends TitledBorder {
    //公开的实例化方法GUI_BORDER()
    public GUI_BORDER(String title) {
        //调用父类构造方法
        super(title);
    }
}

//自定义类GUI_PANEL_LABEL_FONT继承自Font类
class GUI_PANEL_LABEL_FONT extends Font {
    //公开的实例化方法GUI_PANEL_LABEL_FONT()，接收字体名称、样式和大小
    public GUI_PANEL_LABEL_FONT(String name, int style, int size) {
        //调用父类构造方法
        super(name, style, size);
    }
}

//自定义类GUI_MENUBAR继承自JMenuBar类
class GUI_MENUBAR extends JMenuBar {

}

//自定义类GUI_MENUBAR_MENU继承自JMenu类
class GUI_MENUBAR_MENU extends JMenu {
    //公开的实例化方法GUI_MENUBAR_MENU()，接收标题
    public void init(String index) {
        //设置菜单栏标题
        this.setText(index);
    }
}

//自定义类GUI_MENUBAR_MENU_MENUITEM继承自JMenuItem
class GUI_MENUBAR_MENU_MENUITEM extends JMenuItem {
    //公开的构造方法，接收一个GUI，两个GUI_PANEL
    public GUI_MENUBAR_MENU_MENUITEM(GUI gui, GUI_PANEL gui_panel_name, GUI_PANEL gui_panel_Description) {
        //设置菜单项名称
        this.setText("exit");
        //添加动作监听器
        this.addActionListener(new ActionListener() {
            //重写动作实现
            @Override
            public void actionPerformed(ActionEvent arg0) {
                half half = new half();
                gui.getContentPane().removeAll();
                gui.getContentPane().revalidate();
                gui.repaint();
                //gui添加gui_panel
                gui.add(gui_panel_name);
                //gui添加gui_panel
                gui.add(gui_panel_Description);
                //重绘页面
                gui.repaint();
                half.stopCapture();
            }
        });
    }
}

class DATA {
    public String Source_Mac;
    public String Destination_Mac;
    public int Ethernet_Type;

    public void init() {

    }
}

class GUI_JTABBEDPANE_DATA_LINK_LAYER extends JTabbedPane {
    public GUI_JTABBEDPANE_DATA_LINK_LAYER(GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER gui_jtabbedpane_scrollpane) {

    }
}

class GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER extends JScrollPane {
    public GUI_JTABBEDPANE_SCROLLPANE_DATA_LINK_LAYER(GUI_SCROLLPANE_TABLE_DATA_LINK_LAYER table) {
        super(table);
    }
}

class GUI_JTABBEDPANE_SCROLLPANE_NETWORK_LAYER extends JScrollPane {
    public GUI_JTABBEDPANE_SCROLLPANE_NETWORK_LAYER(GUI_SCROLLPANE_TABLE_NETWORK_LAYER table) {
        super(table);
    }
}

class GUI_SCROLLPANE_TABLE_DATA_LINK_LAYER extends JTable {
    public GUI_SCROLLPANE_TABLE_DATA_LINK_LAYER(GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER model, GUI_PANEL_PAYLOAD gui_panel_payload, GUI gui) {
        this.revalidate();
        this.setModel(model);
        this.getColumnModel().getColumn(0).setPreferredWidth(0);
        this.getColumnModel().getColumn(1).setPreferredWidth(0);
        this.getColumnModel().getColumn(2).setPreferredWidth(0);
        this.getColumnModel().getColumn(3).setPreferredWidth(0);
        this.getColumnModel().getColumn(4).setPreferredWidth(0); 
        this.getColumnModel().getColumn(5).setPreferredWidth(0);
        this.getColumnModel().getColumn(6).setPreferredWidth(0);
        this.getColumnModel().getColumn(7).setPreferredWidth(0);
        this.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
        this.getSelectionModel().addListSelectionListener(new ListSelectionListener() {
            @Override
            public void valueChanged(ListSelectionEvent e) {
                int RowNumber = getSelectedRow();
                gui_panel_payload.SHOW_DATA(RowNumber);
            }
        });
    }
}

class GUI_SCROLLPANE_TABLE_NETWORK_LAYER extends JTable {
    public GUI_SCROLLPANE_TABLE_NETWORK_LAYER(GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER model) {
        this.revalidate();
        this.setModel(model);
        this.getColumnModel().getColumn(0).setPreferredWidth(0);
        this.getColumnModel().getColumn(1).setPreferredWidth(0);
        this.getColumnModel().getColumn(2).setPreferredWidth(0);
        this.getColumnModel().getColumn(3).setPreferredWidth(0);
        this.getColumnModel().getColumn(4).setPreferredWidth(0); 
        this.getColumnModel().getColumn(5).setPreferredWidth(0);
        this.getColumnModel().getColumn(6).setPreferredWidth(0);
        this.getColumnModel().getColumn(7).setPreferredWidth(0);
        this.getColumnModel().getColumn(8).setPreferredWidth(0);
        this.getColumnModel().getColumn(9).setPreferredWidth(0);
        this.getColumnModel().getColumn(10).setPreferredWidth(0);
        this.getColumnModel().getColumn(11).setPreferredWidth(0);
        this.getColumnModel().getColumn(12).setPreferredWidth(0);
        this.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
    }
}

class GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER extends DefaultTableModel {
    public GUI_SCROLLPANE_TABLE_MODEL_DATA_LINK_LAYER () {
        this.addColumn("No");
        this.addColumn("time");
        this.addColumn("Source MAC");
        this.addColumn("Destination MAC");
        this.addColumn("Ethernet Type");
        this.addColumn("Frame Type");
        this.addColumn("Frame Length");
        this.addColumn("Payload Length");
        this.addColumn("Priority");
    }

    public void setdata(String src_mac, String des_mac, String ethernet_type, String frame_type, int frame_length, int payload_length, int priority, long diff_sec, long diff_usec) {
        String time = String.format("%d.%06d", diff_sec, diff_usec);
        this.addRow(new Object[]{getRowCount() + 1 , time, src_mac, des_mac, "0x" + ethernet_type, frame_type, frame_length + "b", payload_length + "b", priority});
    }
}

class GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER extends DefaultTableModel {
    public GUI_SCROLLPANE_TABLE_MODEL_NETWORK_LAYER() {
        this.addColumn("No");
        this.addColumn("Source IP");
        this.addColumn("Destination IP");
        this.addColumn("IP Version");
        this.addColumn("Head length");
        this.addColumn("Total Length");
        this.addColumn("IP Identifier");
        this.addColumn("IP Flags");
        this.addColumn("IP Fragment Offset");
        this.addColumn("Time To Live");
        this.addColumn("Type Of Service");
        this.addColumn("Protocol");
        this.addColumn("Checksum");
    }

    public void setdata(String src_ip, String dst_ip, int version, int ip_head_length, int total_length, int ip_identifier, int reserved_bit, int df_bit, int mf_bit, int fragment_offset, int time_to_live, String type_of_service, String protocol, String checksum) {
        this.addRow(new Object[]{getRowCount() + 1, src_ip, dst_ip, version, ip_head_length + "b", total_length + "b", ip_identifier, "R:" + reserved_bit + " DF:" + df_bit + " MF:" + mf_bit, fragment_offset, time_to_live, type_of_service, protocol, checksum});
    }
}

class GUI_PANEL_PAYLOAD extends JTextArea {
    public List<byte[]> bytestream = new ArrayList<>();
    public GUI_PANEL_PAYLOAD() {
        this.setBounds(0, 300, 1600, 300);
    }
    public void SHOW_DATA(int nmuber) {
        int offset = 0;
        byte[] index = bytestream.get(nmuber);
        StringBuilder hex = new StringBuilder();
        for (int i = 0; i < index.length; i++) {
            if (i % 16 == 0) {
                hex.append(String.format("%04x\t", offset));        
            }
            hex.append(String.format("%02x ", index[i]));
            if ((((i + 1) % 8 == 0 && i != index.length - 1))) {
                hex.append(String.format("  "));
            }
            offset++;
            if (((i + 1) % 16 == 0)) {
                hex.append(String.format(" "));
                for (int j = i - 15; j <= i; j++) {
                    if (index[j] >= 32 && index[j] <= 126) {
                        hex.append((char)index[j]);
                    }
                    else {
                        hex.append(String.format(" . "));
                    }
                }
                hex.append(String.format("\n"));
            }
        }
        if (index.length % 16 != 0) {
            for (int i = index.length; i < (index.length / 16 + 1) * 16; i++) {
                String.format("   ");
                if ((i + 1) % 8 == 0 && i != index.length - 1) {
                    hex.append(String.format(" "));
                }
            }
            String.format(" ");
            for (int j = (index.length / 16) * 16; j < index.length; j++) {
                if (Character.isISOControl(index[j])) {
                    hex.append(String.format("%c", index[j]));
                }
                else {
                    hex.append(String.format("."));
                }
            }
            hex.append(String.format("\n"));
            }
        hex.append(String.format("\n"));
        this.setText(hex.toString());
    }
}
