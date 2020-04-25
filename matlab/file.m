fileID = fopen('sin.dat');
A = fread(fileID, 'uint16');
plot(A);
grid on;
fclose(fileID);
