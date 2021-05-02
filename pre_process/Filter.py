#实用与新的检测装置的滤波代码
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sg
import datetime as dt
plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签
plt.rcParams['axes.unicode_minus']=False   #这两行需要手动设置

D=pd.read_csv('E:/大学/srtp/SRTP_code/pre_process/2021-04-11-bd.txt',sep=' ',header=None)
ymd=D[0][0]
hms=D[1][0]
year=int(ymd.split('-')[0])
mon=int(ymd.split('-')[1])
day=int(ymd.split('-')[2])
hour=int(hms.split(':')[0])+7
minute=int(hms.split(':')[1])-5
second=int(hms.split(':')[2])
Start=dt.datetime(year,mon,11,hour,minute,second)
time=Start
T=[0 for x in range(0,len(D))]
R=np.empty((4,len(D)))
PMM=np.empty((len(D),4))
for j in range(4):
    P=D[2+j]#2为co2
    PM=np.zeros(len(D))
    for i in range(len(D)):
        PM[i]=int(P[i])
        time=time+dt.timedelta(seconds=2)
        T[i]=time
    smooth=sg.savgol_filter(PM,51,3)
    R[j]=smooth
    PMM.T[j]=PM.T
#将滤波后的数据存入txt文件
file = open(r'after_filtering.json','w')
file.write('[')
for j in range(len(D)-1):
    lat=str(D[9][j])
    lng=str(D[10][j])
    c=str(R[0][j])#co2
    str_temp = '{"lng":' + lng + ',"lat":' + lat + ',"count":' + c + '},'
    file.write(str_temp)
    file.write('\n')
lat=str(D[9][len(D)-1])
lng=str(D[10][len(D)-1])
c=str(R[0][len(D)-1])#co2
str_temp = '{"lng":' + lng + ',"lat":' + lat + ',"count":' + c + '}]'
file.write(str_temp)
file.close()

plt.figure()
plt.subplot(221)
plt.plot(T,PMM.T[0],label='original data',linewidth=0.5)
plt.plot(T,R[0],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('CO2(ppm)')
plt.title('CO2滤波数据')
plt.grid()

plt.subplot(222)
plt.plot(T,PMM.T[1],label='original data',linewidth=0.5)
plt.plot(T,R[1],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('CH2O(ug/m^3)')
plt.title('CH2O滤波数据')
plt.grid()

plt.subplot(223)
plt.plot(T,PMM.T[2],label='original data',linewidth=0.5)
plt.plot(T,R[2],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('TVOC(ug/m^3)')
plt.title('TVOC滤波数据')
plt.grid()

plt.subplot(224)
plt.plot(T,PMM.T[3],label='original data',linewidth=0.5)
plt.plot(T,R[3],label='smoothed',color='red')
plt.xlabel('Datetime')
plt.ylabel('PM2.5(ug/m^3)')
plt.title('PM2.5滤波数据')

plt.grid()
plt.legend()
plt.show()