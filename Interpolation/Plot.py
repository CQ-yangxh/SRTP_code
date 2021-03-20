import pandas as pd
from pykrige.ok import OrdinaryKriging
import numpy as np

data=pd.read_json('./PM2_5.json')
lngs=data['lng'].values
lats=data['lat'].values
pm=data['count'].values