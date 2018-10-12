function draw_cmp(x, y, z, max_v, tit)
    figure;
    ax = axes;
    
    z(z > max_v) = max_v;
    z(z < 1/max_v) = 1/max_v;
    %surf(x, y, z', 'EdgeColor', 'interp', 'FaceColor', 'interp');
    surf(x, y, z', 'EdgeColor', 'none');
    xlabel('file (bytes)');
    ylabel('buff (bytes)');
    set(ax,'XScale','log');
    set(ax,'YScale','log');
    %set(ax,'ZScale','log');
    colorbar;
    title(['z in [1/' num2str(max_v) ';' num2str(max_v) ']; times ' tit]);
    view(0, 90);
end

