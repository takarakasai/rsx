function [ rot ] = untitled(rpy_deg)

rad = rpy_deg / 180.0 * pi;

%% roll
xrot = [   1            0            0        ;
           0          cos(rad(1)) -sin(rad(1));
           0          sin(rad(1))  cos(rad(1));];
%% pitch
yrot = [ cos(rad(2))    0          sin(rad(2));
           0            1            0        ;
        -sin(rad(2))    0          cos(rad(2));];
%% yaw
zrot = [ cos(rad(3))  -sin(rad(3))    0        ;
         sin(rad(3))   cos(rad(3))    0        ;
           0             0            1        ;];

rot = zrot * yrot * xrot;
       
end