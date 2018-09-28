function h = build_res(names, file_sizes, buf_sizes)
    filename = [names.res_fld '/' names.filename '.' names.data_ext];
    l_file = length(file_sizes);
    l_buf = length(buf_sizes);
    times = reshape(dlmread(filename), [2, l_file * l_buf]);
    h = zeros(l_file, l_buf);
    for fi = 1:l_file
        for bi = 1:l_buf
            i = bi + (fi-1) * l_buf;
            h(fi, bi) = times(1, i) + times(2, i);
        end
    end
end

