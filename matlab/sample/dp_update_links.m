function [ ] = dp_update_links(link, ee_type)

% ee_type (end efector type) : 'hand' or 'foot' or 'head'

idx = 1;
link(idx).rot = dp_get_rpy_rot(link(idx).dir * link(idx).angle);
link(idx).pos = link(idx).offset;

while link(idx).child > 0
    parent = idx;
    idx = link(idx).child;

    link(idx).rot = dp_get_rpy_rot(link(idx).dir * link(idx).angle);
    link(idx).rot = link(parent).rot * link(idx).rot;

    
    link(idx).pos = link(parent).pos + link(idx).rot * link(idx).offset;
   
    from = link(parent).pos;
    to   = link(idx).pos;
    plot3([from(1) to(1)], [from(2), to(2)], [from(3), to(3)], 'LineWidth', 5);
    draw_cylinder(link(parent).pos, link(parent).rot * link(idx).dir, 5, 25, 40);
    
    if link(idx).child == 0
        if (strcmp(ee_type ,'foot'))
            draw_rectangular(link(idx).pos, link(idx).rot, [122.0 37.1*2 2]);
        elseif (strcmp(ee_type ,'head'))
            draw_rectangular(link(idx).pos, link(idx).rot, [ 60.0 40.0 40.0]);
        end
    end
end

end