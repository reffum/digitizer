fileID = fopen('2020_08_GRE_1T_Valera.dat');
A = fread(fileID, 'uint16');

% 10ms data 
B = A(1600000:2800000);

%set(gcf,'renderer','zbuffer')

pl = plot(B);
grid on;
fclose(fileID);

% Save B data to file
% fileID = fopen('AdcData.data', 'w');
% fprintf(fileID, '%X\n', B);
% fclose(fileID);
