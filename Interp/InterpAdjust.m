function z = InterpAdjust(X,Y,Z,S,t)
    for i=1:size(X,1)
        for j=1:size(X,2)
            minDST=min((S(:,1)-X(i,j)).^2+(S(:,2)-Y(i,j)).^2);
            if (minDST>t)
                z(i,j)=NaN;
            else
                z(i,j)=Z(i,j);
            end
        end
    end
end