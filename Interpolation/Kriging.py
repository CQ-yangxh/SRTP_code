import pandas as pd
from pykrige.ok import OrdinaryKriging
import numpy as np


data=pd.read_json('./address.json')
lngs=data['lng'].values
lats=data['lat'].values
pm=data['count'].values

grid_lng=np.linspace(np.min(lngs),np.max(lngs),400)
grid_lat=np.linspace(np.min(lats),np.max(lats),400)

OK=OrdinaryKriging(lngs, lats,pm,variogram_model='gaussian',nlags=6)
z1, ss1 = OK.execute('grid', grid_lng, grid_lat)

file=open('./PM2_5.json','w')

for i in range(len(grid_lng)):
    lng=grid_lng[i]
    for j in range(len(grid_lat)):
        lat=grid_lat[j]
        c=z1[i][j]
        str_temp='{"lng":'+str(lng)+',"lat":'+str(lat)+',"count":'+str(c)+'},'
        file.write(str_temp)
        file.write('\n')
file.close()
