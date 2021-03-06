import pandas as pd
from pykrige.ok import OrdinaryKriging
import numpy as np
import geopandas as gpd
import plotnine as pn


file = r"中国省级地图GS（2019）1719号.geojson"
js = gpd.read_file(file)
js_box = js.geometry.total_bounds
grid_lng = np.linspace(js_box[0],js_box[2],400)
grid_lat = np.linspace(js_box[1],js_box[3],400)

data=pd.read_json('./address.json')
lngs=data['lng'].values
lats=data['lat'].values
pm=data['count'].values


OK=OrdinaryKriging(lngs, lats,pm,variogram_model='gaussian',nlags=6)
z1, ss1 = OK.execute('grid', grid_lng, grid_lat)

#转换成网格
xgrid, ygrid = np.meshgrid(grid_lng, grid_lat)
#将插值网格数据整理
df_grid =pd.DataFrame(dict(lng=xgrid.flatten(),lat=ygrid.flatten()))
#添加插值结果
df_grid["Krig_gaussian"] = z1.flatten()
df_grid_geo = gpd.GeoDataFrame(df_grid, geometry=gpd.points_from_xy(df_grid["lng"], df_grid["lat"]),crs="EPSG:4326")
df_grid_clip = gpd.clip(df_grid_geo,js)

pn.options.figure_size = (10, 7.5)
Hotmap = (pn.ggplot() + 
           pn.geom_tile(df_grid_clip,pn.aes(x='lng',y='lat',fill='Krig_gaussian'),size=0.1) +
           pn.geom_map(js,fill='none',color='gray',size=0.3) + 
           pn.scale_fill_cmap(cmap_name='Spectral_r',name='PM2.5',
                           breaks=[20,40,60,80]
                           )+
           pn.scale_x_continuous(breaks=[80,90,100,110,120,130])+
           pn.labs(title="Hotmap",
                )+
           #添加文本信息

           pn.theme(
               text=pn.element_text(family="Roboto Condensed"),
               #修改背景
               panel_background=pn.element_blank(),
               axis_ticks_major_x=pn.element_blank(),
               axis_ticks_major_y=pn.element_blank(),
               axis_text=pn.element_text(size=12),
               axis_title = pn.element_text(size=14),
               panel_grid_major_x=pn.element_line(color="gray",size=.5),
               panel_grid_major_y=pn.element_line(color="gray",size=.5),
            ))
