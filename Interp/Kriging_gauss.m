clc
clear all
load data;
mS=data.pos;
mY=data.PM2_5;
[S,Y]=dsmerge(mS,mY);       %合并重复的采样点

%设定协方差矩阵上下界
theta = [20 20]; lob = [1e-6 1e-6]; upb = [15e-6 15e-6];
%设定变异函数模型为高斯模型
[dmodel, perf] = dacefit(S, Y, @regpoly2, @corrgauss, theta, lob, upb);
Size=200;       %设定插值网格的大小为200*200
border=0.0005;  %设定边界范围
X = gridsamp([min(data.pos(:,1))-border min(data.pos(:,2))-border;
              max(data.pos(:,1))+border max(data.pos(:,2))+border], Size);     

%格网点的预测值返回在矩阵YX中，预测点的均方根误差返回在矩阵MSE中
[YX,MSE] = predictor(X, dmodel);    
X1 = reshape(X(:,1),Size,Size); X2 = reshape(X(:,2),Size,Size);
YX = reshape(YX, size(X1));         %size(X1)=40*40
t=1e-7;
YX=InterpAdjust(X1,X2,YX,S,t);      %筛除离采样点过远的插值点
figure(1);
mesh(X1, X2, YX)         %绘制预测表面
hold on,
plot3(S(:,1),S(:,2),Y,'*r', 'MarkerSize',1)    %绘制原始散点数据
xlabel('Latitude');
ylabel('Longitude');
zlabel('PM2.5');
title('Kriging Interpolation Result');
hold off
figure(2);
Err=reshape(MSE,size(X1));
Err=InterpAdjust(X1,X2,Err,S,t);
mesh(X1, X2,Err);  %绘制每个点的插值误差大小
xlabel('Latitude');
ylabel('Longitude');
zlabel('Error');
title('Kriging Interpolation Error');
Output=[];
for i=1:size(X1,1)
    for j=1:size(X1,2)
        if(~isnan(YX(i,j)))
            Output=[Output;X1(i,j) X2(i,j) YX(i,j)];
        end
    end
end

save([localpath(),'Kriging_gauss插值结果.txt'],'Output','-ascii');