txtname='data.txt';
F=fopen(txtname);%文件名
od=textscan(F,'%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s','delimiter',',');
num=0;
for i=1:size(od{1},1)
    if(strcmp(od{1}(i),'$GNGGA')==1)
        num=num+1;
        for j=1:15
            pd(num,j)=od{j}(i);
        end
    end
end
    
sd=char(pd(:,15));
for i=1:size(sd,1)
    if (sd(i,end)==' ')
        for j=size(sd,2):-1:2
            sd(i,j)=sd(i,j-1);
        end
        sd(i,1)='0';
    end
end
i=0;
for num=1:size(sd,1)
    head=sd(num,5:8);
    if(strcmp(head,'3C02')==0)
        continue;
    end
    i=i+1;
    time=char(pd(num,2));
    data.time(i,1)=str2double(time(1:2));%hour
    data.time(i,2)=str2double(time(3:4));%minute
    data.time(i,3)=str2double(time(5:10));%second
    n_lat=char(pd(num,3));
    e_long=char(pd(num,5));
    data.pos(i,1)=str2double(n_lat(1:2))+str2double(n_lat(3:9))/60;%North latitude
    data.pos(i,2)=str2double(e_long(1:3))+str2double(e_long(4:10))/60;%East longitude
    for j=1:17
        B(i,j)=string(sd(num,2*j+3:2*j+4));
    end
     check=dec2hex(sum(hex2dec(B(i,3:16))));
     B(i,18)=check(end-1:end);
end
    data.eCO2=hex2dec(strcat(B(:,3),B(:,4)));
    data.eCH2O=hex2dec(strcat(B(:,5),B(:,6)));
    data.TVOC=hex2dec(strcat(B(:,7),B(:,8)));
    data.PM2_5=hex2dec(strcat(B(:,9),B(:,10)));
    data.PM10=hex2dec(strcat(B(:,11),B(:,12)));
    data.temp=hex2dec(strcat(B(:,13),B(:,14)))/256;
    data.humi=hex2dec(strcat(B(:,15),B(:,16)))/256;
    DATA=[data.time data.pos data.eCO2 data.eCH2O data.TVOC data.PM2_5 data.PM10 data.temp data.humi];
    
    data.PM2_5=KalmanFiltering(sum(data.PM2_5(1:5))/5,data.PM2_5,1,1,1,36,(sum(data.PM2_5(1:5))/5).^2);
    save([localpath(),'data.mat'],'data');
