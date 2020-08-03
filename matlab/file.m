fileID = fopen('1.dat');
A = fread(fileID, 'uint16');

% 10ms data 
B = A(1:2048000);

%set(gcf,'renderer','zbuffer')

pl = plot(B);
grid on;
fclose(fileID);

% Save B data to file
fileID = fopen('AdcData.data', 'w');
fprintf(fileID, '%X\n', B);
fclose(fileID);
