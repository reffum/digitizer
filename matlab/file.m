fileID = fopen('1.dat');
A = fread(fileID, 'uint16');

B = A(1600000:2800000);
% 10ms data 
B = A(1350000:1750000);

T = 1/(160e6);

M = timeseries(B, 0:T:T*(size(B)-1));
save('M.mat', 'M', '-v7.3');

%set(gcf,'renderer','zbuffer')
% pl = plot(B);
% grid on;
ylim([0 65530]);
fclose(fileID);

% Save B data to file
 fileID = fopen('AdcData.data', 'w');
 fprintf(fileID, '%X\n', B);
 fclose(fileID);
