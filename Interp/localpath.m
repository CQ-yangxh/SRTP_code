function z=localpath()
    z=mfilename('fullpath'); 
    add0=mfilename;%当前M文件名
    add1=mfilename('fullpath');%当前m文件路径
    i=length(add0);
    j=length(add1);

    local_address=add1(1:j-i-1);
    z=[local_address, '/'];
end