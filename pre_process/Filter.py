#实用与新的检测装置的滤波代码
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sg
import datetime as dt

D=pd.read_csv('2021-02-07-bd.txt',sep=' ',header=None)
Start=dt.datetime(2021,2,7,20,51,20);
time=Start
T=[0 for x in range(0,1231)]
P=D[7]
PM=np.zeros(1231)
for i in range(1231):
    PM[i]=int(P[i])
    time=time+dt.timedelta(seconds=2)
    T[i]=time
smooth=sg.savgol_filter(PM,51,3)
plt.figure()
plt.xlabel('Datetime')
plt.ylabel('Temperature($\mathregular{ ^o}$C)')
plt.plot(T,PM,label='original data',linewidth=0.5)
plt.plot(T,smooth,label='smoothed',color='red')
plt.grid()
plt.legend()
plt.show()