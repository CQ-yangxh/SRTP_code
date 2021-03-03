
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sg
import datetime as dt

D=pd.read_csv('D:\School\srtp\SRTP_code-main\pre_process\s1.txt',sep=' ',header=None)
Start=dt.datetime(2020,11,8,0,10,50);
time=Start
T=[0 for x in range(0,1436)]
P=D[7]
PM=np.zeros(1436)
for i in range(1436):
    PM[i]=int(P[i],16)
    time=time+dt.timedelta(minutes=1)
    T[i]=time
smooth=sg.savgol_filter(PM,51,3)
plt.figure()
plt.xlabel('Datetime')
plt.ylabel('PM2.5($\mu$g/$\mathregular{m^2}$)')
plt.plot(T,PM,label='original data',linewidth=0.5)
plt.plot(T,smooth,label='smoothed',color='red')
plt.grid()
plt.legend()
plt.show()