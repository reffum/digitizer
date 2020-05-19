fileID = fopen('1.dat');
A = fread(fileID, 'uint16');
B = A(1:50000);

%set(gcf,'renderer','zbuffer')

pl = plot(A);
%ytickformat('usd')
% yticks = get(gca, 'YTick');
% set(gca, 'YTickLabel', dec2hex(yticks));

grid on;
fclose(fileID);

% 4e20 - 7530
% 5.02 - 4.98