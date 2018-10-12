clear;
close all;
file_sizes = [50 100 200 300 500 1000 2000 3000 3500 3750 4000 4250 4500 5000 10000 20000 30000 50000 100000 200000 300000 500000 1000000 2000000 3000000 5000000 10000000 20000000 30000000 50000000 100000000 200000000 300000000 500000000 1000000000 2000000000 3000000000 4000000000];
buf = [50 100 200 300 500 1000 2000 3000 3500 3750 4000 4250 4500 5000 6000 7000 7500 7750 7900 8000 8100 8150 8192];
names.res_fld = 'res';
names.data_ext = 'data';

names.filename = 'msg_msg';
res_msg = build_res(names, file_sizes, buf);
names.filename = 'msg_mmap';
res_mmap = build_res(names, file_sizes, buf);
names.filename = 'msg_shm';
res_shm = build_res(names, file_sizes, buf);
cmp_data{1} = res_shm ./ res_mmap;
cmp_data{2} = res_msg ./ res_mmap;
cmp_data{3} = res_shm ./ res_msg;

tits = {'SHM-VS-MMAP', 'MSG-VS-MMAP', 'SHM-VS-MSG'};
%max_values = [1.15, 2.5, 10];
max_values = [1.15, 2.5];

for i_max = 1:length(max_values)
    for i_tit = 1:length(tits)
        draw_cmp(file_sizes, buf, cmp_data{i_tit}, max_values(i_max), tits{i_tit});
    end
end