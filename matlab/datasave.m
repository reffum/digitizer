% Open file with Zybo data
fileID = fopen('2020_08_GRE_1T_Valera.dat');
A = fread(fileID, 'uint16');

% Sample period
T = 1/(160e6);

% Get part of data
B = A(1600000:2800000);

% Save data in Matlab data file
M = timeseries(B, 0:T:T*(size(B)-1));
save('M.mat', 'M', '-v7.3');

fclose(fileID);

