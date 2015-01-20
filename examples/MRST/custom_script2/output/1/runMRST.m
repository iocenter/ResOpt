function [] = runMRST

% required paths/filenames-------------------------------------------------
%mrstPath = '/Users/steink/Documents/mrst_bitbucket/mrst_core/';
mrstPath = '/home/aleksaju/Work/postdoc/Stein/ResOpt/mrstFeb2014/mrst_core';

% ResOpt and MRST will exchange data through text-files in current path
% name for main input-file
caseNm = 'SIMPLE10x5x10.txt';
% also provide path to MRST/ResOpt specific code (including runCaseMRST.m) 
codePath = '/home/aleksaju/Work/postdoc/Stein/ResOpt/code/';

% -----Setup MRST and run simulation --------------------------------------
run(fullfile(mrstPath,'startup.m'))
mrstModule add ad-fi deckformat
addpath(codePath);

runCaseMRST(caseNm);
