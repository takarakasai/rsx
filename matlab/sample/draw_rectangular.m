function [ output_args ] = untitled(pos, rot, size)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

% format: draw_rectanguler([x,y,z], [roll, pitch, yaw], [width, hight, depth])
% ex:     draw_cylinder([0,1,0], [0,0,-90], [100 40 2])

%{
dir_origin = [0 0 1];
offset_dir = cross(dir_origin, dir);
offset_rad = acos(dir_origin * dir / (norm(dir_origin) * norm(dir)));

% TODO: deg? rad?
rot = dp_get_rpy_rot(offset_dir * offset_rad / pi * 180);
%}

width = size(1) / 2.0;
hight = size(2) / 2.0;
depth = size(3);

xyz    = {[-width -width  width  width; -hight  hight  hight -hight;     0      0      0      0;]};
xyz{2} =  [-width -width  width  width; -hight  hight  hight -hight; depth  depth  depth  depth;];

xyz{3} =  [-width -width  width  width; -hight -hight -hight -hight;     0  depth  depth      0;];
xyz{4} =  [-width -width  width  width;  hight  hight  hight  hight;     0  depth  depth      0;];

xyz{5} =  [-width -width -width -width; -hight -hight  hight  hight;     0  depth  depth      0;];
xyz{6} =  [ width  width  width  width; -hight -hight  hight  hight;     0  depth  depth      0;];

for tmp = xyz
    rect = (rot * tmp{1})';
    patch(rect(:,1) + pos(1), rect(:,2) + pos(2), rect(:,3) + pos(3), 'green');
end

end

