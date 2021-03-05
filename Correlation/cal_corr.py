import os
import pandas as pd
import matplotlib.pyplot as plt
data = pd.read_csv(os.path.join(os.path.dirname(os.getcwd()),"pre_process/2021-02-07-bd.txt"),sep=' ',header=None)
rows = data.shape[0]
cols = data.shape[1]
ddata = data.iloc[:,2:cols-2]
ddata = ddata.T.reset_index(drop=True).T
new_cols = ['co2','methanal','TVOC','PM2.5','PM10','T','H']
ddata.columns = new_cols
corr_array = ddata.corr(method='spearman')
print(corr_array)

x1 = ddata[:]['co2']
x2 = ddata[:]['TVOC']
plt.scatter(x1,x2)
plt.show()