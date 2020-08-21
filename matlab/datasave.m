% Open file with Zybo data
fileID = fopen('2020_08_GRE_1T_Valera.dat');
A = fread(fileID, 'uint16');

% Sample period
T = 1/(160e6);
Fs = 160e6;

% Get part of data
B = A(1600000:2800000) - mean(A);

% Save data in Matlab data file
%M = timeseries(B, 0:T:T*(size(B)-1));
%save('M.mat', 'M', '-v7.3');

% FFT
L = size(B,1)-1;

Y = fft(B);
f = Fs*(0:(L/2))/L;

P2 = abs(Y);
P1 = P2(1:L/2+1);

plot(P1); 
title('Single-Sided Amplitude Spectrum of X(t)');
xlabel('f (Hz)');
ylabel('|F(f)|');
grid on;

fclose(fileID);
