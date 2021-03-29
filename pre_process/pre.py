#该代码将txt文件中的坐标进行转化并重新写入
import math

# 系数常量
a = 6378245.0
ee = 0.00669342162296594323
x_pi = 3.14159265358979324 * 3000.0 / 180.0

# 转换经度
def transformLat(lat, lon):
    ret = -100.0 + 2.0 * lat + 3.0 * lon + 0.2 * lon * lon + 0.1 * lat * lon + 0.2 * math.sqrt(abs(lat))
    ret += (20.0 * math.sin(6.0 * lat * math.pi) + 20.0 * math.sin(2.0 * lat * math.pi)) * 2.0 / 3.0
    ret += (20.0 * math.sin(lon * math.pi) + 40.0 * math.sin(lon / 3.0 * math.pi)) * 2.0 / 3.0
    ret += (160.0 * math.sin(lon / 12.0 * math.pi) + 320 * math.sin(lon * math.pi / 30.0)) * 2.0 / 3.0
    return ret

# 转换纬度
def transformLon(lat, lon):
    ret = 300.0 + lat + 2.0 * lon + 0.1 * lat * lat + 0.1 * lat * lon + 0.1 * math.sqrt(abs(lat))
    ret += (20.0 * math.sin(6.0 * lat * math.pi) + 20.0 * math.sin(2.0 * lat * math.pi)) * 2.0 / 3.0
    ret += (20.0 * math.sin(lat * math.pi) + 40.0 * math.sin(lat / 3.0 * math.pi)) * 2.0 / 3.0
    ret += (150.0 * math.sin(lat / 12.0 * math.pi) + 300.0 * math.sin(lat / 30.0 * math.pi)) * 2.0 / 3.0
    return ret

# Wgs transform to gcj
def wgs2gcj(lat, lon):
    dLat = transformLat(lon - 105.0, lat - 35.0)
    dLon = transformLon(lon - 105.0, lat - 35.0)
    radLat = lat / 180.0 * math.pi
    magic = math.sin(radLat)
    magic = 1 - ee * magic * magic
    sqrtMagic = math.sqrt(magic)
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * math.pi)
    dLon = (dLon * 180.0) / (a / sqrtMagic * math.cos(radLat) * math.pi)
    mgLat = lat + dLat
    mgLon = lon + dLon
    loc = [mgLat, mgLon]
    return loc

# gcj transform to bd2
def gcj2bd(lat, lon):
    x = lon
    y = lat
    z = math.sqrt(x * x + y * y) + 0.00002 * math.sin(y * x_pi)
    theta = math.atan2(y, x) + 0.000003 * math.cos(x * x_pi)
    bd_lon = z * math.cos(theta) + 0.0065
    bd_lat = z * math.sin(theta) + 0.006
    bdpoint = [bd_lat, bd_lon]
    return bdpoint

# wgs transform to bd
def wgs2bd(lat, lon):
    wgs_to_gcj = wgs2gcj(lat, lon)
    gcj_to_bd = gcj2bd(wgs_to_gcj[0], wgs_to_gcj[1])
    return gcj_to_bd

def gainlat(pre_lat):
    lat=float(pre_lat)/100
    lat=5/3*lat-2/3*math.floor(lat)
    return lat

def gainlon(pre_lon):
    lon = float(pre_lon) / 100
    lon = 5 / 3 * lon - 2 / 3 * math.floor(lon)
    return lon

def gaintime(time):
    new_time=time[0]+time[1]+':'+time[2]+time[3]+':'+time[4]+time[5]
    return new_time

def gaindata(data):
    co2=int(data[7]+data[8],16)*256+int(data[9]+data[10],16)
    ch2o=int(data[11]+data[12],16)*256+int(data[13]+data[14],16)
    tvoc=int(data[15]+data[16],16)*256+int(data[17]+data[18],16)
    pm25=int(data[19]+data[20],16)*256+int(data[21]+data[22],16)
    pm10=int(data[23]+data[24],16)*256+int(data[25]+data[26],16)
    t=int(data[27]+data[28],16)+0.1*int(data[29]+data[30],16)
    h=int(data[31]+data[32],16)+0.1*int(data[33]+data[34],16)
    new_data=str(co2)+' '+str(ch2o)+' '+str(tvoc)+' '+str(pm25)+' '+str(pm10)+' '+str(t)+' '+str(h)
    return new_data
##################################
filename = 'data_2_10dian_xuanwuhu.txt'

with open(filename) as f:#默认以只读方式打开文件
    lines = f.readlines()#读取所有行，结果为列表，每行为列表一元素

data=[]
for line in lines:
    line_temp=line.split(',')
    if(line_temp[0]=='$GNGGA'):#清除没有定位的数据
        if(line_temp[14][3]=='3'):
            if(line_temp[6]!='0'):
                data.append(line_temp)
                print(line_temp)
f.close()

txt=''
with open('xuanwuhu.txt','w') as f:
    for line in data:
        bd_point=wgs2bd(gainlat(line[2]),gainlon(line[4]))
        str_lat=str(bd_point[0])
        str_lon=str(bd_point[1])
        str_data=gaindata(line[14])
        str_time=gaintime(line[1])
        txt='2020-00-00'+' '+str_time+' '+str_data+' '+str_lat+' '+str_lon
        f.write(txt.strip(' '))
        f.write('\n')
        txt=''
f.close()
