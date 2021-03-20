function  Xkf= KalmanFiltering(InitX,Z,F,Q,H,R,P0)

X=InitX;

%数据点数目
[l,s]=size(X);%获取数据维数
L=length(Z);
%初始化观测值矩阵 
Xkf=zeros(L,s);
Xkf(:,1)=X(:,1);
P=P0;
%滤波 
for i=2:L
    Xn=F*(Xkf(i-1,:)');%一步预测
   P=F*P*F'+Q;%一步预测误差方差阵
    K=P*H'*inv(H*P*H'+R);%滤波增益矩阵（权重）
    Xkf(i,:)=Xn+K*(Z(i,:)-H*Xn);%状态误差方差阵估计
    P=(eye(s)-K*H)*P;
end 
% fig=figure(1);
% set(fig,'position',[200 200 1200 500]);
% hold on;
% plot(Z(:,1),'--b.','MarkerSize',1);
% hold on;
% plot(Xkf(:,1),'--r','MarkerSize',1);
% legend('观测轨迹','滤波轨迹');
% xlabel('时间');
% ylabel('测量值');
end