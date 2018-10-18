clear;
N_threads = 1:22;
N0 = 2*3*2*5*7*2*3*11*13*2*17*19;
N_points = N0;
data_filename = 'res2.data';

system(['rm ' data_filename]);
for i_thr = 1:length(N_threads)
    system(['./tst1.exe ' num2str(N_threads(i_thr)) ' ' num2str(N_points) ' >> ' data_filename]);        
    disp(i_thr / length(N_threads));
end

data = dlmread(data_filename);

plot(N_threads, data);
grid on;
title(['time for comp']);
