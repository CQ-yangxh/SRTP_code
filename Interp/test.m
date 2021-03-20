    PM2_5=smooth(y1);
    PM2_5=KalmanFiltering(sum(PM2_5(1:5))/5,PM2_5,1,1,1,4,(sum(PM2_5(1:5))/5).^2);
    plot(x,y1);
    hold on
    plot(x,PM2_5,'r','linewidth',1.5);
    title('PM2.5')
    legend('原始数据','滤波轨迹');
    
