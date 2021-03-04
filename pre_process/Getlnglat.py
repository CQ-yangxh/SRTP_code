#!/usr/bin/env python3
import requests
import json
import pandas as pd

from urllib.request import urlopen
from urllib.parse import quote
def getlnglat(address):
    url = 'http://api.map.baidu.com/geocoding/v3/'
    ak = 'j8oShVHL9770jdzhixdzHWOsZ28rDDON'
    add = quote(address) #由于本文城市变量为中文，为防止乱码，先用quote进行编码
    uri = url + '?' + 'address=' + add + '&output=json' + '&ak=' + ak
    req = urlopen(uri)
    res = req.read().decode() #将其他编码的字符串解码成unicode
    temp = json.loads(res)
    print(temp)
    return temp

file = open(r'address.json','w') #建立json数据文件
wb = pd.read_excel('自定义查询.xlsx')
rows = wb.shape[0]
cols = wb.shape[1]
for i in range(0,rows):
    b = wb.loc[i][6]
    c = wb.loc[i][5]
    result = getlnglat(b)
    judge = result['status']
    if int(judge) == 0:
        lng = result['result']['location']['lng'] #采用构造的函数来获取经度
        lat = result['result']['location']['lat'] #获取纬度
        str_temp = '{"lng":' + str(lng) + ',"lat":' + str(lat) + ',"count":' + str(c) +'},'
        file.write(str_temp) #写入文档
        file.write('\n')

file.close()
