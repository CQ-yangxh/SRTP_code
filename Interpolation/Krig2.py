#有用的克里格插值
import pandas as pd
from pykrige.ok import OrdinaryKriging
import numpy as np
from  matplotlib import pyplot as plt
import matplotlib.cm as cm

data=pd.read_json(r'E:/大学/srtp/SRTP_code/pre_process/after_filtering.json')
lngs=data['lng'].values
lats=data['lat'].values
pm=data['count'].values
Num=20
grid_lat = np.linspace(np.max(lats),np.min(lats),Num)
grid_lng = np.linspace(np.max(lngs),np.min(lngs),Num)


OK=OrdinaryKriging(lngs, lats,pm,variogram_model='gaussian',nlags=5)
z1, ss1 = OK.execute('grid', grid_lng, grid_lat)

#转换成网格
xgrid, ygrid = np.meshgrid(grid_lng, grid_lat)
#将插值网格数据整理
df_grid =pd.DataFrame(dict(lng=xgrid.flatten(),lat=ygrid.flatten()))
#添加插值结果
df_grid["Krig_gaussian"] = z1.flatten()


file1=open('./int_krig.json','w')
file1.write('[')
for i in range(len(df_grid)-1):
    lng=df_grid['lng'].values[i]
    lat=df_grid['lat'].values[i]
    c=df_grid['Krig_gaussian'].values[i]
    str_temp='{"lng":'+str(lng)+',"lat":'+str(lat)+',"count":'+str(c)+'},'
    file1.write(str_temp)
    file1.write('\n')
i+=1
lng=df_grid['lng'].values[i]
lat=df_grid['lat'].values[i]
c=df_grid['Krig_gaussian'].values[i]
str_temp='{"lng":'+str(lng)+',"lat":'+str(lat)+',"count":'+str(c)+'}'
file1.write(str_temp)
file1.write(']')
file1.close()
            
fig=plt.figure(1)
ax1=plt.axes(projection='3d')
ax1.scatter3D(lngs,lats,pm,color='gray')
surf1=ax1.plot_surface(xgrid,ygrid,z1,cmap=cm.coolwarm)
plt.colorbar(surf1,shrink=0.5, aspect=10)#标注
plt.title('Kriging Interpolation Result')
plt.ylabel('Latitude')
plt.xlabel('Longitude')
ax1.set_zlabel('CO2(ppm)')

plt.figure(2)
im1=plt.imshow(z1, extent=[np.min(lngs),np.max(lngs),np.min(lats),np.max(lats)], cmap=cm.coolwarm, interpolation='nearest', origin="lower")#pl.cm.jet
#extent=[]为x,y范围  favals为
plt.colorbar(im1)
plt.show()
