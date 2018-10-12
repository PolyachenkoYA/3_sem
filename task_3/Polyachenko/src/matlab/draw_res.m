function draw_res(names, h, file_sizes, buf_sizes, tit)
% ------------------------------ time ----------------------------------
    figure;
    ax = axes;
    surf(file_sizes, buf_sizes, log(h'), 'EdgeColor', 'interp', 'FaceColor', 'interp');
    xlabel('file (bytes)');
    ylabel('buff (bytes)');
    set(ax,'XScale','log');
    set(ax,'YScale','log');
    %set(ax,'ZScale','log');

    colorbar;
    title(['time; ' tit]);
    view(0, 90);
    filename = [names.res_fld '/' names.filename '_time.fig'];
    savefig(filename);
    
% ------------------------------ eff ---------------------------------    
    l_file = length(file_sizes);
    l_buf = length(buf_sizes);
    eff = zeros(l_file, l_buf);
    for fi = 1:l_file
        for bi = 1:l_buf
            eff(fi, bi) = file_sizes(fi) / h(fi, bi);
        end
    end
    figure;
    ax = axes;    
    surf(file_sizes, buf_sizes, log(eff'), 'EdgeColor', 'interp', 'FaceColor', 'interp');
    xlabel('file (bytes)');
    ylabel('buff (bytes)');
    set(ax,'XScale','log');
    set(ax,'YScale','log');
    %set(ax,'ZScale','log');

    colorbar;
    title(['efficiency; ' tit]);
    view(0, 90);    
    filename = [names.res_fld '/' names.filename '_eff.fig'];
    savefig(filename);    
end

