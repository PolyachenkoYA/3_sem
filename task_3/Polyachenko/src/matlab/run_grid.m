clear;
close all;
names.res_fld = 'res';
names.data_ext = 'data';

file_sizes = [50 100 200 300 500 1000 2000 3000 3500 3750 4000 4250 4500 5000 10000 20000 30000 50000 100000 200000 300000 500000 1000000 2000000 3000000 5000000 10000000 20000000 30000000 50000000 100000000 200000000 300000000 500000000 1000000000 2000000000 3000000000 4000000000];

names_a = {'msg_mmap', 'big_mmap'; ...
           'msg_shm', 'big_shm'};
buffs_a = {[50 100 200 300 500 1000 2000 3000 3500 3750 4000 4250 4500 5000 6000 7000 7500 7750 7900 8000 8100 8150 8192], ...
           [50 100 200 300 500 1000 2000 3000 3500 3750 4096 4250 4500 5000 7000 7500 7750 8000 8100 8192 8500 9000 10000 20000 30000 50000 100000 200000 300000 500000 1000000 2000000 3000000 5000000 10000000 20000000 30000000 50000000 100000000 200000000 300000000 500000000 1000000000 2000000000]};
       
%file_sizes = [1000 2000 3000 10000];
%names_a = {'tst11', 'tst12', 'tst13'; ...
%           'tst21', 'tst22', 'tst23'};
%buffs_a = {[10 20 30], [100 200 300 500 1000]};

sz = size(names_a);
if(sz(2) ~= length(buffs_a))
    disp('wrong sizes');
    return;
end

for i_buff = 1:sz(2)
    for i_name = 1:sz(1)
        names.filename = names_a{i_name, i_buff};
        disp([names.filename ' ' num2str(i_name)]);
        create_res(names, file_sizes, buffs_a{i_buff}, i_name);
        res = build_res(names, file_sizes, buffs_a{i_buff});
        draw_res(names, res, file_sizes, buffs_a{i_buff});
        close all;
    end
end