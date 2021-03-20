clc
clear all
load data
step=0.00002;
border=0.0005;
[pos,dst]=dsmerge(data.pos,data.PM2_5);
[X,Y]=meshgrid(min(pos(:,1))-border:step:max(pos(:,1))+border,min(pos(:,2))-border:step:max(pos(:,2)+border));
Z = griddata(pos(:,1),pos(:,2),dst,X,Y,'v4');
t=1e-7;
Z = InterpAdjust(X,Y,Z,pos,t);
mesh(X, Y, Z);         %绘制预测表面
hold on,
plot3(pos(:,1),pos(:,2),dst,'*r', 'MarkerSize',1)    %绘制原始散点数据
xlabel('Latitude');
ylabel('Longitude');
zlabel('PM2.5');
hold off
Output=[];
for i=1:size(X,1)
    for j=1:size(X,2)
        if(~isnan(Z(i,j)))
            Output=[Output;X(i,j) Y(i,j) Z(i,j)];
        end
    end
end
save([localpath(),'样条插值结果.txt'],'Output','-ascii');