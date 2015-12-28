
figure;
hold;

% 本当は[TODO:]でも言及しているとおり、同一のリンクを親リンクとして持つような関節を
% 扱うような仕組みがあった方が汎用性が高まるが、今回はわかりやすさを重視して
% あえて同一の親リンクを持つリンクを辿るルーチンは入れていない.

% TODO: -38.5?-28.5?-18.5?
% TODO:        -7.5?-27.5?
% link(1) としないのは繰り返し実行する際にエラーとなるのを防ぐため

% TODO: +30?  & 60.0
hlink     = struct('name', 'hbase',            'offset', [12.18    0.0 110.10]', 'child', 2, 'dir', [0 0 0]', 'angle', 0);
hlink(2)  = struct('name', 'hhead_yaw',        'offset', [ 0.00    0.0  20.00]', 'child', 0, 'dir', [0 0 1]', 'angle', 0);

% TODO: -30.0 & -/+44.4
ralink    = struct('name', 'rabase',           'offset', [12.18  -30.0 110.10]', 'child', 2, 'dir', [0 0 0]', 'angle', 0);
ralink(2) = struct('name', 'ra_shoulder_pitch','offset', [ 0.00  -44.4   0.00]', 'child', 3, 'dir', [0 1 0]', 'angle', 0);
ralink(3) = struct('name', 'ra_elbow_yaw',     'offset', [ 0.00  -87.5   4.45]', 'child', 4, 'dir', [0 0 1]', 'angle', 0);
ralink(4) = struct('name', 'ra_list_roll',     'offset', [ 0.00 -101.0   0.00]', 'child', 0, 'dir', [1 0 0]', 'angle', 0);

lalink    = struct('name', 'labase',           'offset', [12.18   30.0 110.10]', 'child', 2, 'dir', [0 0 0]', 'angle', 0);
lalink(2) = struct('name', 'la_shoulder_pitch','offset', [ 0.00   44.4   0.00]', 'child', 3, 'dir', [0 1 0]', 'angle', 0);
lalink(3) = struct('name', 'la_elbow_yaw',     'offset', [ 0.00   87.5   4.45]', 'child', 4, 'dir', [0 0 1]', 'angle', 0);
lalink(4) = struct('name', 'la_list_roll',     'offset', [ 0.00  101.0   0.00]', 'child', 0, 'dir', [1 0 0]', 'angle', 0);

rlink    = struct('name', 'rbase',        'offset', [0 -17.0   0.0]', 'child', 2, 'dir', [0 0 0]', 'angle', 0);
rlink(2) = struct('name', 'rhip_roll',    'offset', [0   0.0 -18.5]', 'child', 3, 'dir', [1 0 0]', 'angle', 0);
rlink(3) = struct('name', 'rhip_pitch',   'offset', [0   0.0 -65.0]', 'child', 4, 'dir', [0 1 0]', 'angle', 0);
rlink(4) = struct('name', 'rknee_pitch',  'offset', [0   0.0 -65.0]', 'child', 5, 'dir', [0 1 0]', 'angle', 0);
rlink(5) = struct('name', 'rankle_pitch', 'offset', [0   0.0 -18.5]', 'child', 6, 'dir', [0 1 0]', 'angle', 0);
rlink(6) = struct('name', 'rankle_roll',  'offset', [0 -37.1 -27.5]', 'child', 0, 'dir', [1 0 0]', 'angle', 0);

llink    = struct('name', 'lbase',        'offset', [0 +17.0   0.0]', 'child', 2, 'dir', [0 0 0]', 'angle', 0);
llink(2) = struct('name', 'lhip_roll',    'offset', [0   0.0 -18.5]', 'child', 3, 'dir', [1 0 0]', 'angle', 0);
llink(3) = struct('name', 'lhip_pitch',   'offset', [0   0.0 -65.0]', 'child', 4, 'dir', [0 1 0]', 'angle', 0);
llink(4) = struct('name', 'lknee_pitch',  'offset', [0   0.0 -65.0]', 'child', 5, 'dir', [0 1 0]', 'angle', 0);
llink(5) = struct('name', 'lankle_pitch', 'offset', [0   0.0 -18.5]', 'child', 6, 'dir', [0 1 0]', 'angle', 0);
llink(6) = struct('name', 'lankle_roll',  'offset', [0 +37.1 -27.5]', 'child', 0, 'dir', [1 0 0]', 'angle', 0); 
   
while 1 
    %
    for idx = 1:1:size(hlink,2)
        hlink(idx).angle = rand * 180 - 90;        
    end
    for idx = 1:1:size(lalink,2)
        lalink(idx).angle = rand * 40 - 20;
        ralink(idx).angle = rand * 40 - 20;       
    end
    for idx = 1:1:size(llink,2)

        llink(idx).angle = rand * 40 - 20;
        rlink(idx).angle = rand * 40 - 20;
    end
    %

    clf;
    view([1 0.5 0.2]); 
    axis equal;
    grid on;
    xlim([-200 200]);
    ylim([-200 200]);
    zlim([-200 200]);

    hold on;
    
    draw_rectangular([0 0 0], dp_get_rpy_rot([0 0 0]), [30 60 120]);
    dp_update_links(hlink,  'head');    
    dp_update_links(lalink, 'hand');
    dp_update_links(ralink, 'hand');
    dp_update_links(rlink,  'foot');
    dp_update_links(llink,  'foot');

    pause
end


