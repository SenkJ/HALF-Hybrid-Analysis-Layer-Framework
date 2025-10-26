import pymysql
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator
from collections import Counter

mydb = pymysql.connect(host='127.0.0.1',user='Senk.J',passwd='',database='Packet')

mycursor = mydb.cursor()

query = "SELECT id, time, time_point FROM PacketData"

mycursor.execute(query)

result = mycursor.fetchall()

number = []

time_list = []

time_list_point = []

TimeCount = []


def time_number(x = [], y = []) :
    x_major_locator = MultipleLocator(1)
    ax = plt.gca()
    ax.xaxis.set_major_locator(x_major_locator)
    y_major_locator = MultipleLocator(0.1)
    ay = plt.gca()
    ay.yaxis.set_major_locator(y_major_locator)
    plt.ylim(-0.1, 2.5)
    plt.xlabel('number')
    plt.ylabel('time(s)')
    plt.plot(x, y, linestyle = '-', marker = '.', markersize = 8, linewidth = 1, color = 'tab:blue')
    plt.plot(x, y, marker = '.', linestyle = '-', color = 'b', label = 'time-number')

    for i in range(len(x)):
        plt.text(x[i], y[i], f'({y[i]:.2f})', fontsize=8, ha='right', va='bottom')

i = 0
for var1, var2, var3 in result:
    time = var2 + var3/1000000
    number.append(var1 - 1)
    time_list.append(time)
    TimeCount.append(var2)
    print(var1 - 1, ':', time)
    time_list_point.append(time_list[i] - time_list[i - 1])
    print(time_list_point[i])
    i = i + 1

count = Counter(TimeCount)
print(count)
labels = list(count.keys())
counts = list(count.values())

plt.figure(1)
time_number(number, time_list_point)
manger = plt.get_current_fig_manager()

plt.figure(2)
x_major_locator = MultipleLocator(1)
ax = plt.gca()
ax.xaxis.set_major_locator(x_major_locator)
y_major_locator = MultipleLocator(1)
ay = plt.gca()
ay.yaxis.set_major_locator(y_major_locator)
bars = plt.bar(labels, counts)
plt.xlabel('time')
plt.ylabel('count')
plt.title('time-count')

# 在每个柱顶端添加具体数值
for bar in bars:
    height = bar.get_height()
    plt.text(
        bar.get_x() + bar.get_width() / 2,  # 柱的中心位置
        height,  # 柱的高度
        f'{int(height)}',  # 显示整数值
        ha='center', va='bottom', fontsize=8  # 对齐方式和字体大小
    )

plt.ioff()
plt.show()

mydb.close()

#统计每一秒内捕获到的数据包，绘制时间-数据包数统计图
#根据小数点后数字统计即可，即整数位数值一致，小数位数值不同的数据包时间戳
