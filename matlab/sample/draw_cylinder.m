function [ output_args ] = untitled(pos, dir, r, len, num_of_step)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

% format: draw_cylinder([x,y,z], [roll, pitch, yaw], radius, length)
% ex:     draw_cylinder([0,1,0], [0,0,-90], 2, 2)

dir_origin = [0 0 1];
offset_dir = cross(dir_origin, dir);
offset_rad = acos(dir_origin * dir / (norm(dir_origin) * norm(dir)));

% TODO: deg? rad?
rot = dp_get_rpy_rot(offset_dir * offset_rad / pi * 180);

% num_of_step = 80;
step        = 2 * pi / num_of_step;
rad = 0:step:2*pi;
topz =   len / 2 * ones(size(rad));
btmz = - len / 2 * ones(size(rad));
x = r * cos(rad);
y = r * sin(rad);

xyz  = rot * [x;y;topz];
xyz2 = rot * [x;y;btmz];
topx = xyz(1,:);
topy = xyz(2,:);
topz = xyz(3,:);
btmx = xyz2(1,:);
btmy = xyz2(2,:);
btmz = xyz2(3,:);

% make circle surface
patch(topx' + pos(1), topy' + pos(2), topz' + pos(3), 'red');
patch(btmx' + pos(1), btmy' + pos(2), btmz' + pos(3), 'blue');

% make rectanble surfaces
x2 = [topx(1:end-1);topx(2:end  );btmx(2:end);btmx(1:end-1)];
y2 = [topy(1:end-1);topy(2:end  );btmy(2:end);btmy(1:end-1)];
z2 = [topz(1:end-1);topz(2:end  );btmz(2:end);btmz(1:end-1  )];
siz = size(x2(1,:));
patch(x2(:,1    ) + pos(1), y2(:,1    ) + pos(2), z2(:,1    ) + pos(3), 'red');
patch(x2(:,2:end) + pos(1), y2(:,2:end) + pos(2), z2(:,2:end) + pos(3), 'blue');

end

