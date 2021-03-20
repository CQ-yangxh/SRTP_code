clc
clear all
load data
step=0.00002;
border=0.0005;
[pos,dst]=dsmerge(data.pos,data.PM2_5);
lat=pos(:,1);
lon=pos(:,2);
x=min(lat)-border:step:max(lat)+border;
y=min(lon)-border:step:max(lon)+border;
weight=-2;
radius_length=length(lat);


for i=1:length(x)
    for j=1:length(y)
        D=[]; V=[]; wV =[]; vcc=[];
        D= sqrt((x(i)-lat).^2 +(y(j)-lon).^2);
        D=D(D<radius_length); vcc=dst(D<radius_length);
        V = vcc.*(D.^weight);
        wV = D.^weight;
        if isempty(D)
            V=NaN;
        else
            V=sum(V)/sum(wV);
        end
        Z(j,i)=V;
    end
end
t=1e-7;
[X,Y]=meshgrid(x,y);
Z = InterpAdjust(X,Y,Z,pos,t);
mesh(X, Y, Z);         %绘制预测表面
hold on,
plot3(lat,lon,dst,'*r', 'MarkerSize',1)    %绘制原始散点数据
xlabel('Latitude');
ylabel('Longitude');
zlabel('PM2.5');
title('IDW Interpolation Result');
hold off
Output=[];
for i=1:size(X,1)
    for j=1:size(X,2)
        if(~isnan(Z(i,j)))
            Output=[Output;X(i,j) Y(i,j) Z(i,j)];
        end
    end
end
save([localpath(),'IDW插值结果.txt'],'Output','-ascii');