import matplotlib.pyplot as plt
plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签
plt.rcParams['axes.unicode_minus']=False   #这两行需要手动设置

filename = '2021-02-07-bd.txt'
X1 ,X2, X3, X4, X5, X6, X7 =  [], [], [], [], [], [], []
with open(filename, 'r') as f:  # 1
    lines = f.readlines()  # 2
    for line in lines:  # 3
        temp=line.split()  # 4
        X1.append(int(temp[2]))  #co2(ppm)
        X2.append(int(temp[3]))  #甲醛(ug/m^3)
        X3.append(int(temp[4]))  #TVOC(ug/m^3)
        X4.append(int(temp[5]))  #PM2.5(ug/m^3)
        X5.append(float(temp[6]))  #PM10ug/m^3)
        X6.append(float(temp[7]))  #TEM(C)
        X7.append(float(temp[8]))  #HUM(%)

plt.figure(1)
plt.ylabel('CO2(ppm)')
plt.plot(X1)

plt.figure(2)
plt.ylabel('甲醛(ug/m^3)')
plt.plot(X2)

plt.figure(3)
plt.ylabel('TVOC(ug/m^3)')
plt.plot(X3)

plt.figure(4)
plt.ylabel('PM2.5(ug/m^3)')
plt.plot(X4)

plt.figure(5)
plt.ylabel('PM10(ug/m^3)')
plt.plot(X5)

plt.figure(6)
plt.ylabel('温度(C)')
plt.plot(X6)

plt.figure(7)
plt.ylabel('湿度(%)')
plt.plot(X7)
plt.show()