import numpy as np
from math import radians,cos,sin,asin,sqrt
import pandas as pd
from  matplotlib import pyplot as plt
import matplotlib.cm as cm

def haversine(lon1,lat1,lon2,lat2):
    R=6372.8
    dlon=radians(lon2-lon1)
    dlat=radians(lat2-lat1)
    lat1=radians(lat1)
    lat2=radians(lat2)
    a = sin(dlat/2)**2 + cos(lat1)*cos(lat2)*sin(dlon/2)**2
    c = 2*asin(sqrt(a))
    d = R * c
    return d

def IDW(x,y,z,xi,yi):#xi,yi为插值网格点
    lstxyzi=[]
    for p in range(len(xi)):
        lstdist=[]
        for s in range(len(x)):
            d=(haversine(x[s],y[s],xi[p],yi[p]))#求取距离当前插值点的距离
            lstdist.append(d)#将求取的距离添加到一个list中
        sumsup=list((1/np.power(lstdist,2)))#对每一个距离求取1/x^2,并把数组转换成列表
        suminf=np.sum(sumsup)
        sumsup=np.sum(np.array(sumsup)*np.array(z))#数组的加减乘除，对应元素进行运算，各个原始点的值乘以权重后相加
        u=sumsup/suminf#归一化
        xyzi=[xi[p],yi[p],u]
        lstxyzi.append(xyzi)
    return(lstxyzi)

data=pd.read_json(r'E:/大学/srtp/SRTP_code/pre_process/after_filtering.json')
lngs=data['lng'].values
lats=data['lat'].values
counts=data['count'].values#co2
Num=50

grid_lat = np.linspace(np.max(lats),np.min(lats),Num)
grid_lng = np.linspace(np.max(lngs),np.min(lngs),Num)
xgrid, ygrid = np.meshgrid(grid_lng, grid_lat)

#将插值网格数据整理
df_grid=pd.DataFrame(dict(long=xgrid.flatten(),lat=ygrid.flatten()))
#这里将数组转成列表
grid_lon_list=df_grid["long"].tolist()
grid_lat_list=df_grid["lat"].tolist()

pm_idw=IDW(lngs,lats,counts,grid_lon_list,grid_lat_list)
IDW_grid_df=pd.DataFrame(pm_idw,columns=["lon","lat","idw_value"])
IDW_grid_df.head()
pollutant=[x[2] for x in pm_idw]
r=int(sqrt(np.size(pollutant)))
z1=np.zeros([r,r],dtype=int)
for i in range(r):
    for j in range(r):
        z1[i][j]=pollutant[i*r+j]


fig=plt.figure()
ax1=plt.axes(projection='3d')
ax1.scatter3D(lngs,lats,counts,color='gray')
surf1=ax1.plot_surface(xgrid,ygrid,z1,cmap=cm.coolwarm)
plt.title('IDW Interpolation Result')
plt.ylabel('Latitude')
plt.xlabel('Longitude')
ax1.set_zlabel('CO2(ppm)')
plt.colorbar(surf1,shrink=0.5, aspect=10)#标注


plt.figure(2)
im1=plt.imshow(z1, extent=[np.min(lngs),np.max(lngs),np.max(lats),np.min(lats)], cmap=cm.coolwarm, interpolation='nearest', origin="lower")#pl.cm.jet
#extent=[]为x,y范围  favals为
plt.colorbar(im1)
plt.show()

file = open(r'int_idw.json', 'w')  # 建立json数据文件
file.write('[')

for line in range(len(pm_idw)-1):
    lat=str(pm_idw[line][1])
    lng=str(pm_idw[line][0])
    count=str(pm_idw[line][2])
    str_temp = '{"lng":' + lng + ',"lat":' + lat + ',"count":' + count + '},'
    file.write(str_temp)
    file.write('\n')
lat=str(pm_idw[len(pm_idw)-1][1])
lng=str(pm_idw[len(pm_idw)-1][0])
count=str(pm_idw[len(pm_idw)-1][2])
str_temp = '{"lng":' + lng + ',"lat":' + lat + ',"count":' + count + '}'
file.write(str_temp)
file.write(']')
file.close()
