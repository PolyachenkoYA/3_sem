function create_res(names, file_sizes, buf_sizes)
    l_file = length(file_sizes);
    l_buf = length(buf_sizes);

    whole_job = sum(sum(file_sizes ./ buf_sizes'));
    done_job = 0;

    filename = [names.res_fld '/' names.filename '.' names.data_ext];
    system(['rm ' filename]);
    for fi = 1:l_file
        for bi = 1:l_buf
            system(['./mmap.exe ' num2str(file_sizes(fi)) ' ' num2str(buf_sizes(bi)) ' tst >> ' filename]);
            done_job = done_job + file_sizes(fi) / buf_sizes(bi);
            disp(num2str(done_job / whole_job));
        end
    end    
end

