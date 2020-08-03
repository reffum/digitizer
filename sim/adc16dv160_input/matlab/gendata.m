fileID = fopen('data.dat', 'w');
d = 1:100;
fprintf(fileID, '%d\n', d);
fclose(fileID);
