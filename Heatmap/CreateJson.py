import csv
import json
file = open(r'E:\大学\srtp\SRTP_code\Heatmap\pm2.5-bd.json', 'w')  # 建立json数据文件

with open(r'E:\大学\srtp\SRTP_code\pre_process\2021-02-07-bd.txt', 'r') as f:
    lines = f.readlines()
    for line in lines:  # 读取csv里的数据
        # line.split()是个list，取得所有需要的值
        lng = line.split()[10] #经度
        lat = line.split()[9] #纬度
        c = line.split()[6]
        str_temp = '{"lng":' + lng + ',"lat":' + lat + ',"count":' + c + '},'
        file.write(str_temp)  # 写入文档
        file.write('\n')
file.close()  # 保存
