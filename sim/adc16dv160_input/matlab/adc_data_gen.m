%
% Genetate data for level sync simulation
%

% Sample rate
Fs = 160e6;

% Sample period
dt = 1 / Fs;

% Simulation time
Tsim = 10e-3;

% Samples number
Ns = Tsim / dts;

% Data freq
f = 10e3;

% Data amplitude
A = 1024;

% ADC data
AdcData = zeros(1, Ns);

%
% ADC data offset 
%

% Offset period
Toff = 2e-3;

% Offset freq
Foff = 1/Toff;

% Offset duty cycle
DCoff = 60;

% Offset value
Aoff = 10000;

% Generate data
t = 0:dt:Tsim;
AdcData = A * (1 + sin(2*pi*f*t)) + Aoff*(1 + square(2*pi*Foff*t, DCoff));
AdcDataInt = cast(AdcData, 'uint16');

plot(t, AdcDataInt);
grid on;

% Write data to file
fileID = fopen('AdcData.data', 'w');
fprintf(fileID, '%X\n', AdcDataInt);
fclose(fileID);


