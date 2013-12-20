mrstPath = '';
run(fullfile(mrstPath,'startup.m'))

% change controls
inj1 = [5000, 5000, 5000]*meter^3/day;
prod1 = [2300, 2000, 2000]*barsa;
prod2 = [2300, 2000, 2000]*barsa;
u  = [inj1; prod1; prod2];

try
 %write control file:
 fid = fopen('res_water_CONTROLS.TXT', 'w');
 fprintf(fid, '%+12.6e %+12.6e %+12.6e\n',  [inj1; prod1; prod2]);
 fclose(fid);

 runSim2;
catch err
 warning('something went wrong with MRST');
 exit(3)
end
