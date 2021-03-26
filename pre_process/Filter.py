#实用与新的检测装置的滤波代码
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sg
import datetime as dt
plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签
plt.rcParams['axes.unicode_minus']=False   #这两行需要手动设置

D=pd.read_csv('xuanwuhu.txt',sep=' ',header=None)
Start=dt.datetime(2021,2,7,10,52,18);
time=Start
T=[0 for x in range(0,728)]#728为读取txt中的数据行数
R=np.empty((4,728))
PMM=np.empty((728,4))
for j in range(4):
    P=D[2+j]#2为co2
    PM=np.zeros(728)
    for i in range(728):
        PM[i]=int(P[i])
        time=time+dt.timedelta(seconds=2)
        T[i]=time
    smooth=sg.savgol_filter(PM,51,3)
    R[j]=smooth
    PMM.T[j]=PM.T
print(np.array(PMM).shape)
plt.figure()
plt.subplot(221)
plt.plot(T,PMM.T[0],label='original data',linewidth=0.5)
plt.plot(T,R[0],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('CO2(ppm)')
plt.title('CO2滤波数据')

plt.subplot(222)
plt.plot(T,PMM.T[1],label='original data',linewidth=0.5)
plt.plot(T,R[1],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('CH2O(ug/m^3)')
plt.title('CH2O滤波数据')

plt.subplot(223)
plt.plot(T,PMM.T[2],label='original data',linewidth=0.5)
plt.plot(T,R[2],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('TVOC(ug/m^3)')
plt.title('TVOC滤波数据')

plt.subplot(224)
plt.plot(T,PMM.T[3],label='original data',linewidth=0.5)
plt.plot(T,R[3],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('PM2.5(ug/m^3)')
plt.title('PM2.5滤波数据')

plt.grid()
plt.legend()
plt.show()