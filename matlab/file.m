fileID = fopen('2020_08_GRE_1T_Valera.dat');
A = fread(fileID, 'uint16');

B = A(1600000:2800000);
% 10ms data 

T = 1/(160e6);

M = timeseries(B, 0:T:T*(size(B)-1));
save('M.mat', 'M', '-v7.3');

%set(gcf,'renderer','zbuffer')
% pl = plot(B);
% grid on;
fclose(fileID);

% Save B data to file
% fileID = fopen('AdcData.data', 'w');
% fprintf(fileID, '%X\n', B);
% fclose(fileID);
