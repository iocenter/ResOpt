
/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "mrstbatchsimulator.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <iostream>


#include "model.h"
#include "adjointscoupledmodel.h"
#include "reservoir.h"
#include "well.h"
#include "injectionwell.h"
#include "productionwell.h"
#include "wellconnection.h"
#include "wellcontrol.h"
#include "realvariable.h"
#include "intvariable.h"
#include "stream.h"
#include "adjoint.h"
#include "adjointcollection.h"

namespace ResOpt
{


using std::cout;
using std::endl;

MrstBatchSimulator::MrstBatchSimulator() :
    m_first_launch(true),
    run_number(1)
{
}

MrstBatchSimulator::MrstBatchSimulator(const MrstBatchSimulator &m)
    : ReservoirSimulator(m),
      m_first_launch(true),
      run_number(1)
{
}

MrstBatchSimulator::~MrstBatchSimulator()
{}



//-----------------------------------------------------------------------------------------------
// generates the control input file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateControlInputFile(Model *m)
{
    bool ok = true;



    QFile ctrl_file(folder() + "/test2.m");

    if(!ctrl_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to control file: %s", ctrl_file.fileName().toLatin1().constData());
        exit(1);
    }

    QTextStream *out_ctrl = new QTextStream(&ctrl_file);


    // starting to generate the file

    //*out_ctrl << "mrstPath = '/usr/local/MRST/mrst-2013a';" << "\n"; // beehive
    *out_ctrl << "mrstPath = '/home/aleksaju/Work/postdoc/MRST/mrst-2013a';" << "\n"; // linux virtual
    //*out_ctrl << "mrstPath = '/Users/aleksaju/Skole/Postdoc/MRST/versions/2012b';" << "\n"; // mac
    //*out_ctrl << "mrstPath = '/Volumes/Macintosh HD/MATS/Dropbox/Skole/_Masteroppgave/Matlab/mrst-2012b';" << "\n"; // mats mac
    *out_ctrl << "mrstPath = '/Users/eirikhaug/Desktop/mrst-2013a';" << "\n"; // eirik mac

    *out_ctrl << "run(fullfile(mrstPath,'startup.m'))" << "\n\n";

    *out_ctrl << "% change controls" << "\n";

    // for collection of well names
    QStringList well_names;

    // getting the well controls


    // first printing the injection wells

    for(int i = 0; i < m->numberOfWells(); ++i)
    {

        InjectionWell *inj = dynamic_cast<InjectionWell*>(m->well(i));



        if(inj != 0)
        {
            well_names.push_back(inj->name());

            *out_ctrl << inj->name().toLatin1().constData() << " = [";
            for(int j = 0; j < inj->numberOfControls(); ++j)
            {
                *out_ctrl << inj->control(j)->controlVar()->value();
                if(j < inj->numberOfControls() - 1) *out_ctrl << ", ";
                else
                {
                    *out_ctrl << "]*";
                    if(inj->control(j)->type() == WellControl::BHP) *out_ctrl << "barsa;\n";
                    else *out_ctrl << "meter^3/day;\n";
                }
            }

        }


    }


    // then the production wells

    for(int i = 0; i < m->numberOfWells(); ++i)
    {

        ProductionWell *prod = dynamic_cast<ProductionWell*>(m->well(i));

        if(prod != 0)
        {
            well_names.push_back(prod->name());

            *out_ctrl << prod->name().toLatin1().constData() << " = [";
            for(int j = 0; j < prod->numberOfControls(); ++j)
            {
                *out_ctrl << prod->control(j)->controlVar()->value();
                if(j < prod->numberOfControls() - 1) *out_ctrl << ", ";
                else
                {
                    *out_ctrl << "]*";
                    if(prod->control(j)->type() == WellControl::BHP) *out_ctrl << "barsa;\n";
                    else *out_ctrl << "meter^3/day;\n";
                }
            }

        }


    }


    *out_ctrl << "u  = [";
    QString names;
    QString format;

    for(int i = 0; i < (well_names.size()-1); ++i)
    {
        *out_ctrl << well_names.at(i) << "; ";
        names.append(well_names.at(i) + "; ");
        format.append("%+12.6e ");
    }
    *out_ctrl << well_names.last() << "];\n\n";
    names.append(well_names.last());
    format.append("%+12.6e");

    *out_ctrl << "try" << "\n";
    *out_ctrl << " %write control file:" << "\n";
    *out_ctrl << " fid = fopen('" <<  m->reservoir()->file().split(".").at(0) << "_CONTROLS.TXT', 'w');" << "\n";
    *out_ctrl << " fprintf(fid, '" + format + "\\n',  [" + names + "]);" << "\n";
    *out_ctrl << " fclose(fid);" << "\n\n";
    *out_ctrl << " runSim2;" << "\n";
    *out_ctrl << "catch err" << "\n";
    *out_ctrl << " warning('something went wrong with MRST');" << "\n";
    *out_ctrl << " exit(3)" << "\n";
    *out_ctrl << "end" << "\n";

    // closing file
    ctrl_file.close();
    delete out_ctrl;



    return ok;



}

//-----------------------------------------------------------------------------------------------
// generates the MRST Run2 function
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateMRSTScript(Model *m, bool adjoints)
{
    QFile mrst_file(folder() + "/runSim2.m");

    if(!mrst_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to MRST script file: %s", mrst_file.fileName().toLatin1().constData());
        exit(1);
    }

    QTextStream *out_mrst = new QTextStream(&mrst_file);

    // base file name
    QString base_name = m->reservoir()->file().split(".").at(0);



    *out_mrst << "function [] = runSim2\n\n";
    *out_mrst << "mrstModule add ad-fi\n";
    *out_mrst << "mrstModule add deckformat\n\n";

    *out_mrst << "% input/output-files ------------------------------------------------------\n";
    *out_mrst << "caseNm  = '" << m->reservoir()->file() << "';\n";
    *out_mrst << "cntrNm  = '" << base_name << "_CONTROLS.TXT';\n";
    *out_mrst << "outNm   = '" << base_name << "_RES.TXT';\n";
    *out_mrst << "gradNm  = '" << base_name << "_GRAD.TXT';\n";
    *out_mrst << "% -------------------------------------------------------------------------\n\n";

    *out_mrst << "% Enable this to get convergence reports when solving schedules\n";
    *out_mrst << "mrstVerbose false\n\n";

    *out_mrst << "% check if initialized model already exists\n";
    *out_mrst << "[pth, nm, ext] = fileparts(caseNm);\n";
    *out_mrst << "modelNm = fullfile(pth,[nm,'.mat']);\n";
    *out_mrst << "doInitialize = isempty(dir(modelNm));\n";
    *out_mrst << "if doInitialize\n";
    *out_mrst << "    deck = readEclipseDeck(caseNm);\n";
    *out_mrst << "    % Convert to MRST units (SI)\n";
    *out_mrst << "    deck = convertDeckUnits(deck);\n";
    *out_mrst << "    % Create grid\n";
    *out_mrst << "    G = initEclipseGrid(deck);\n";
    *out_mrst << "    % Set up the rock structure\n";
    *out_mrst << "    rock  = initEclipseRock(deck);\n";
    *out_mrst << "    rock  = compressRock(rock, G.cells.indexMap);\n";
    *out_mrst << "    % Create fluid\n";
    *out_mrst << "    fluid = initDeckADIFluid(deck);\n";
    *out_mrst << "    % Get schedule\n";
    *out_mrst << "    schedule = deck.SCHEDULE;\n\n";
    *out_mrst << "    %% Compute constants\n";
    *out_mrst << "    G = computeGeometry(G);\n\n";
    *out_mrst << "    %% Set up reservoir\n";
    *out_mrst << "    % We turn on gravity and set up reservoir and scaling factors.\n";
    *out_mrst << "    gravity on\n";
    *out_mrst << "    state = initStateADI(G, fluid, deck);\n";
    *out_mrst << "    system = initADISystem({'Oil', 'Water'}, G, rock, fluid);\n\n";
    *out_mrst << "    % save initialized model\n";
    *out_mrst << "    save(modelNm, 'G', 'state', 'rock', 'fluid', 'schedule', 'system');\n";
    *out_mrst << "else\n";
    *out_mrst << "    load(modelNm, 'G', 'state', 'rock', 'fluid', 'schedule', 'system');\n";
    *out_mrst << "end\n\n";


    *out_mrst << "%if control input is given, edit schedule:\n";
    *out_mrst << "numContrSteps = numel(schedule.control);\n";
    *out_mrst << "if ~isempty(dir(cntrNm));\n";
    *out_mrst << "    fid = fopen(cntrNm);\n";
    *out_mrst << "    u   = fscanf(fid, '%g');\n";
    *out_mrst << "    numWells      = numel(u)/numContrSteps;\n";
    *out_mrst << "    vals     = mat2cell(u(:), numWells*ones(numContrSteps, 1), 1);\n";
    *out_mrst << "    schedule = updateSchedule(schedule, vals);\n";
    *out_mrst << "    fclose(fid);\n";
    *out_mrst << "end\n\n";


    *out_mrst << "%run simulation:\n";
    *out_mrst << "[wellSols, states] = runScheduleADI(state, G, rock, system, schedule);\n\n";

    *out_mrst << "%produce output\n";
    *out_mrst << "numWells = numel(wellSols{1});\n";
    *out_mrst << "numSteps = numel(wellSols);\n";
    *out_mrst << "wrats    = zeros(numWells, numSteps);\n";
    *out_mrst << "orats    = zeros(numWells, numSteps);\n";
    *out_mrst << "bhps     = zeros(numWells, numSteps);\n";
    *out_mrst << "for wn = 1:numWells\n";
    *out_mrst << "    wrats(wn,:) = cellfun(@(x)x(wn).qWs, wellSols)';\n";
    *out_mrst << "    orats(wn,:) = cellfun(@(x)x(wn).qOs, wellSols)';\n";
    *out_mrst << "    bhps(wn,:)  = cellfun(@(x)x(wn).pressure, wellSols)';\n";
    *out_mrst << "end\n\n";

    *out_mrst << "% take average over controlsteps\n";
    *out_mrst << "M = sparse((1:numSteps)', schedule.step.control, schedule.step.val, numSteps, numContrSteps);\n";
    *out_mrst << "wrats = (wrats*M)./(ones(numWells, 1)*sum(M));\n";
    *out_mrst << "orats = (orats*M)./(ones(numWells, 1)*sum(M));\n";
    *out_mrst << "bhps  = (bhps*M)./(ones(numWells, 1)*sum(M));\n\n";

    *out_mrst << "% write output\n";
    *out_mrst << "fid = fopen(outNm, 'w');\n";
    *out_mrst << "fprintf(fid, '%+12.6e %+12.6e %+12.6e\\n',  [wrats(:) orats(:) bhps(:)]');\n";
    *out_mrst << "fclose(fid);\n\n";

    if(adjoints)
    {
        *out_mrst << "% Compute gradients and print to file\n";
        *out_mrst << "sens = runAdjointWellSensitivities(G, rock, fluid, schedule, system, 'ForwardStates', states);\n";
        *out_mrst << "W = processWellsLocal(G, rock, schedule.control(1));\n";
        *out_mrst << "writeWellSensitivities(W, sens, gradNm);\n\n";

    }



    *out_mrst << "end\n\n";


    if(adjoints) generateMRSTScriptAdjoints(out_mrst);

    return true;

}

//-----------------------------------------------------------------------------------------------
// generates the Adjoints part of the MRST runSim2 function
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateMRSTScriptAdjoints(QTextStream *out_mrst)
{
    // ---- printing the function to calculate adjoints ------

    *out_mrst << "function grad = runAdjointWellSensitivities(G, rock, fluid, schedule, system, varargin) \n\n";
    *out_mrst << "   directory = fullfile(fileparts(mfilename('fullpath')), 'cache');\n\n";

    *out_mrst << "   opt = struct('Verbose',             mrstVerbose, ...\n";
    *out_mrst << "                'writeOutput',         false, ...\n";
    *out_mrst << "                'directory',           directory, ...\n";
    *out_mrst << "                'outputName',          'adjoint', ...\n";
    *out_mrst << "                'outputNameFunc',      [], ...\n";
    *out_mrst << "                'simOutputName',       'state', ...\n";
    *out_mrst << "                'simOutputNameFunc',   [], ...\n";
    *out_mrst << "                'ForwardStates',       [], ...\n";
    *out_mrst << "                'ControlVariables',    [],...\n";
    *out_mrst << "                'scaling',             []);\n\n";
    *out_mrst << "   opt = merge_options(opt, varargin{:});\n\n";
    *out_mrst << "   vb = opt.Verbose;\n";
    *out_mrst << "   states = opt.ForwardStates;\n\n";

    *out_mrst << "   if ~isempty(opt.scaling)\n";
    *out_mrst << "      scalFacs = opt.scaling;\n";
    *out_mrst << "   else\n";
    *out_mrst << "      scalFacs.rate = 1; scalFacs.pressure = 1;\n";
    *out_mrst << "   end\n\n";

    *out_mrst << "   %--------------------------------------------------------------------------\n";
    *out_mrst << "   dts = schedule.step.val;\n";
    *out_mrst << "   tm = cumsum(dts);\n";
    *out_mrst << "   dispif(vb, '*****************************************************************\\n')\n";
    *out_mrst << "   dispif(vb, '**** Starting adjoint simulation: %5.0f steps, %5.0f days *******\\n', numel(dts), tm(end)/day)\n";
    *out_mrst << "   dispif(vb, '*****************************************************************\\n')\n";
    *out_mrst << "   %--------------------------------------------------------------------------\n\n";

    *out_mrst << "   if opt.writeOutput\n";
    *out_mrst << "      % delete existing output\n";
    *out_mrst << "      % delete([opt.outputName, '*.mat']);\n";
    *out_mrst << "      % output file-names\n";
    *out_mrst << "      if isempty(opt.outputNameFunc)\n";
    *out_mrst << "         outNm  = @(tstep)fullfile(opt.directory, [opt.outputName, sprintf('%05.0f', tstep)]);\n";
    *out_mrst << "      else\n";
    *out_mrst << "         outNm  = @(tstep)fullfile(opt.directory, opt.outputNameFunc(tstep));\n";
    *out_mrst << "      end\n";
    *out_mrst << "   end\n";
    *out_mrst << "   if isempty(opt.simOutputNameFunc)\n";
    *out_mrst << "      inNm  = @(tstep)fullfile(opt.directory, [opt.simOutputName, sprintf('%05.0f', tstep)]);\n";
    *out_mrst << "   else\n";
    *out_mrst << "      inNm  = @(tstep)fullfile(opt.directory, opt.simOutputNameFunc(tstep));\n";
    *out_mrst << "   end\n\n";

    *out_mrst << "   nsteps = numel(dts);\n\n";

    *out_mrst << "   prevControl = inf;\n";
    *out_mrst << "   adjVec    = [];\n";
    *out_mrst << "   eqs_p       = [];\n";
    *out_mrst << "   gradFull = cell(1, numel(dts));\n\n";

    *out_mrst << "   % Load state - either from passed states in memory or on disk\n";
    *out_mrst << "   % representation.\n";
    *out_mrst << "   state = loadState(states, inNm, numel(dts));\n\n";

    *out_mrst << "   %-----------------------------------------------------------------------\n";
    *out_mrst << "   numControlSteps = numel(schedule.control);\n";
    *out_mrst << "   numWells        = [];\n\n";

    *out_mrst << "   timero = tic;\n";
    *out_mrst << "   for tstep = numel(dts):-1:1\n";
    *out_mrst << "       dispif(vb, 'Time step: %5.0f\\n', tstep); timeri = tic;\n";
    *out_mrst << "       control = schedule.step.control(tstep);\n";
    *out_mrst << "       if control~=prevControl\n";
    *out_mrst << "           if (control==0)\n";
    *out_mrst << "               W = processWellsLocal(G, rock, [], 'createDefaultWell', true);\n";
    *out_mrst << "           else\n";
    *out_mrst << "           W = processWellsLocal(G, rock, schedule.control(control), 'Verbose', opt.Verbose, ...\n";
    *out_mrst << "                             'DepthReorder', true);\n";
    *out_mrst << "           if isempty(numWells) %initialize\n";
    *out_mrst << "               numWells = numel(W); %assum numWells is equa for all cont steps\n";
    *out_mrst << "               %if isfield(state.wellSol(1), 'qGs')\n";
    *out_mrst << "               %    numWellProps = 4;\n";
    *out_mrst << "               %else\n";
    *out_mrst << "                   numWellProps = 3;\n";
    *out_mrst << "               %end\n";
    *out_mrst << "               numRHS  = numControlSteps*numWells*numWellProps;\n";
    *out_mrst << "               %numVars = 2*numel(state.pressure) + numWells*numWellProps;\n";
    *out_mrst << "           end\n";
    *out_mrst << "           end\n";
    *out_mrst << "       end\n\n";

    *out_mrst << "       state_m = loadState(states, inNm, tstep-1);\n";
    *out_mrst << "       eqs = system.getEquations(state_m, state  , dts(tstep), G, W, system.s, fluid);\n";
    *out_mrst << "       numVars = cellfun(@numval, eqs)';\n";
    *out_mrst << "       cumVars = cumsum(numVars);\n";
    *out_mrst << "       ii = [[1;cumVars(1:end-1)+1], cumVars];\n";
    *out_mrst << "       eqs = cat(eqs{:});\n";
    *out_mrst << "       rhs = getRHS1(control, [sum(numVars), numRHS], numWells, numWellProps);\n\n";

    *out_mrst << "       if tstep < nsteps\n";
    *out_mrst << "           eqs_p = system.getEquations(state  , state_p, dts(tstep+1), G, W_p, system.s, fluid, ...\n";
    *out_mrst << "               'reverseMode', true);\n";
    *out_mrst << "           eqs_p = cat(eqs_p{:});\n";
    *out_mrst << "           rhs = rhs - eqs_p.jac{1}'*adjVec;\n";
    *out_mrst << "       end\n\n";

    *out_mrst << "       adjVec = eqs.jac{1}'\\rhs;\n\n";

    *out_mrst << "       if isempty(opt.ControlVariables)\n";
    *out_mrst << "           gradFull{tstep} = -adjVec(ii(end,1):ii(end,2),:);\n";
    *out_mrst << "       else\n";
    *out_mrst << "           gradFull{tstep} = -adjVec(mcolon(ii(opt.ControlVariables,1),ii(opt.ControlVariables,2)),:);\n";
    *out_mrst << "       end\n\n";

    *out_mrst << "       state   = state_m;\n";
    *out_mrst << "       state_p = state;\n";
    *out_mrst << "       W_p = W;\n";
    *out_mrst << "       prevControl = control;\n";
    *out_mrst << "       dispif(vb, 'Done %6.2f\\n', toc(timeri))\n";
    *out_mrst << "       if opt.writeOutput\n";
    *out_mrst << "           save(outNm(tstep), 'adjVec');\n";
    *out_mrst << "       end\n";
    *out_mrst << "   end\n\n";

    *out_mrst << "   grad = cell(1, numel(schedule.control));\n";
    *out_mrst << "   %take gradient for controll-step as average of time-steps\n";
    *out_mrst << "   for k = 1:numel(schedule.control)\n";
    *out_mrst << "       ck = find((schedule.step.control == k));\n";
    *out_mrst << "       for m = 1:numel(ck)\n";
    *out_mrst << "           if m == 1\n";
    *out_mrst << "               grad{k} = gradFull{ck(1)}/numel(ck);\n";
    *out_mrst << "           else\n";
    *out_mrst << "               grad{k} = grad{k} + gradFull{ck(1)}/numel(ck);\n";
    *out_mrst << "           end\n";
    *out_mrst << "       end\n";
    *out_mrst << "   end\n\n";

    *out_mrst << "   dispif(vb, '************Simulation done: %7.2f seconds ********************\\n', toc(timero))\n";
    *out_mrst << "end\n";
    *out_mrst << "%--------------------------------------------------------------------------\n\n";

    *out_mrst << "function state = loadState(states, inNm, tstep)\n";
    *out_mrst << "if ~isempty(states)\n";
    *out_mrst << "    % State has been provided, no need to look for it.\n";
    *out_mrst << "    state = states{tstep+1};\n";
    *out_mrst << "    return\n";
    *out_mrst << "end\n";
    *out_mrst << "out = load(inNm(tstep));\n";
    *out_mrst << "fn  = fieldnames(out);\n";
    *out_mrst << "if numel(fn) ~= 1\n";
    *out_mrst << "    error('Unexpected format for simulation output')\n";
    *out_mrst << "else\n";
    *out_mrst << "    state = out.(fn{:});\n";
    *out_mrst << "end\n";
    *out_mrst << "end\n\n";


    *out_mrst << "function rhs = getRHS1(cStep, sz, nWells, nWQ)\n";
    *out_mrst << "j1 = (cStep-1)*nWells*nWQ +1;\n";
    *out_mrst << "j2 = cStep*nWells*nWQ;\n";
    *out_mrst << "i1 = sz(1)-nWells*nWQ +1;\n";
    *out_mrst << "i2 = sz(1);\n";
    *out_mrst << "rhs = sparse(i1:i2, j1:j2, -1, sz(1), sz(2));\n";
    *out_mrst << "end\n\n";


    // ----- printing the function to write adjoints to file ------


    *out_mrst << "function [] = writeWellSensitivities(W, sens, fnm)\n";
    *out_mrst << "fid = fopen(fnm, 'w');\n\n";

    *out_mrst << "nw = numel(W);\n";
    *out_mrst << "ns = numel(sens);\n";
    *out_mrst << "numPh = size(sens{1}, 2)/(nw*ns)-1;\n";
    *out_mrst << "%index mappings\n";
    *out_mrst << "if numPh == 2\n";
    *out_mrst << "    ix1 = repmat([1 3 4]', [1, ns*nw]);\n";
    *out_mrst << "    ix1 = ix1 + 4*ones(3,1)*(0:(nw*ns-1));\n";
    *out_mrst << "    ix1 = ix1(:);\n";
    *out_mrst << "    ix2 = repmat([nw+1, 1, 2*nw+1]', [1, nw]);\n";
    *out_mrst << "    ix2 = ix2 + ones(3,1)*(0:(nw-1));\n";
    *out_mrst << "    ix2 = ix2(:);\n";
    *out_mrst << "    ix2 = repmat(ix2, [1, ns]);\n";
    *out_mrst << "    ix2 = ix2 + 3*nw*ones(3*nw,1)*(0:(ns-1));\n";
    *out_mrst << "    ix2 = ix2(:);\n";
    *out_mrst << "end\n\n";

    *out_mrst << "%first, output well-names\n";
    *out_mrst << "for k = 1:nw\n";
    *out_mrst << "    fprintf(fid, [W(k).name, '\\n']);\n";
    *out_mrst << "end\n\n";

    *out_mrst << "%loop over control steps\n";
    *out_mrst << "fmt = '%+12.6e ';\n";
    *out_mrst << "lft = [repmat(fmt, [1, 4*nw*ns]), '\\n'];\n";
    *out_mrst << "MG  = zeros(nw, 4*nw*ns);\n";
    *out_mrst << "for cstep = 1:ns\n";
    *out_mrst << "    fprintf(fid, '\\nSTEP %2.0d\\n', cstep);\n";
    *out_mrst << "    M = full(sens{cstep});\n";
    *out_mrst << "    MG(:,ix1) = M(:, ix2);\n";
    *out_mrst << "    fprintf(fid, lft, MG');\n";
    *out_mrst << "end\n";
    *out_mrst << "fprintf(fid, '\\n');\n";
    *out_mrst << "fclose(fid);\n";
    *out_mrst << "end\n";


    return true;
}

//-----------------------------------------------------------------------------------------------
// generates the ECL include file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateEclIncludeFile(Model *m)
{
    QFile ecl_file(folder() + "/schedule.inc");

    if(!ecl_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to control file: %s", ecl_file.fileName().toLatin1().constData());
        exit(1);
    }

    QTextStream *out_ecl = new QTextStream(&ecl_file);



    // ----------------------- Welspecs ------------------------

    *out_ecl << "WELSPECS\n";
    *out_ecl << "--     WELL   GROUP LOCATION  BHP   PI\n";
    *out_ecl << "--     NAME   NAME    I  J   DEPTH DEFN\n";

    for(int i = 0; i < m->numberOfWells(); ++i)
    {
        Well *w = m->well(i);

        *out_ecl << "       '" << w->name() << "'   '" << w->group() << "'     " << w->connection(0)->i() << "  " << w->connection(0)->j() << "    1*  '";

        if(w->control(0)->type() == WellControl::QWAT) *out_ecl << "WAT'  /\n";
        else if(w->control(0)->type() == WellControl::QOIL)  *out_ecl << "OIL'  /\n";
        else if(w->control(0)->type() == WellControl::QOIL)  *out_ecl << "GAS'  /\n";
        else
        {
            if(w->bhpInj() == WellControl::QGAS) *out_ecl << "GAS'  /\n";
            else if(w->bhpInj() == WellControl::QOIL) *out_ecl << "OIL'  /\n";
            else *out_ecl << "WAT'  /\n";
        }
    }

    *out_ecl << "\n / \n\n";



    // -------------- Compdat ---------------------

    *out_ecl << "COMPDAT\n";
    *out_ecl << "--     WELL     -LOCATION- OPEN/ SAT CONN  WELL\n";
    *out_ecl << "--     NAME     I  J K1 K2 SHUT  TAB FACT  DIAM\n";


    for(int i = 0; i < m->numberOfWells(); ++i) // looping through the wells
    {
        Well *w = m->well(i);

        // looping through the connections
        for(int j = 0; j < w->numberOfConnections(); ++j)
        {
            *out_ecl << "       '" << w->name() << "'     "
                     << w->connection(j)->i() << "  "
                     << w->connection(j)->j() << "  "
                     << w->connection(j)->k1() << "  "
                     << w->connection(j)->k2() << "  "
                     << "'OPEN' 2*       0.3/\n";
        }
    }


    *out_ecl << "\n / \n\n";


    // ------------ Control Intervals -----------------

    for(int i = 0; i < m->numberOfMasterScheduleTimes(); ++i)
    {
        *out_ecl << "-- CONTROL INTERVAL " << i+1 << "\n";


        // WCONPROD (production wells)
        *out_ecl << "WCONPROD\n";
        *out_ecl << "--      WELL     OPEN/  CNTL   OIL  WATER   GAS  LIQU   RES   BHP\n";
        *out_ecl << "--      NAME     SHUT   MODE  RATE   RATE  RATE  RATE  RATE\n";

        for(int j = 0; j < m->numberOfWells(); ++j)
        {
            // checking if this is a production well
            ProductionWell *prod = dynamic_cast<ProductionWell*>(m->well(j));
            if(prod != 0)
            {
                WellControl *c = prod->control(i);

                *out_ecl << "        '" << prod->name() << "'    'OPEN'  '";

                // checking the control type
                if(c->type() == WellControl::BHP)
                {
                    *out_ecl << "BHP'   5*                            "
                             << c->controlVar()->value() << "/\n";
                }
                else if(c->type() == WellControl::QGAS)
                {
                    *out_ecl << "GRAT'  2*"
                             << c->controlVar()->value()
                             << "  3*  /\n";
                }
                else if(c->type() == WellControl::QOIL)
                {
                    *out_ecl << "ORAT'  "
                             << c->controlVar()->value()
                             << "  5*    /\n";
                }

            }
        }
        *out_ecl << "/ \n\n";


        // WCONINJE (injection wells)
        *out_ecl << "WCONINJE\n";
        *out_ecl << "--      WELL     INJ   OPEN/   CNTL    FLOW   RES    BHP\n";
        *out_ecl << "--      NAME    TYPE   SHUT    MODE    RATE   RATE  TARGET\n";

        for(int j = 0; j < m->numberOfWells(); ++j)
        {
            // checking if this is an injection well
            InjectionWell *inj = dynamic_cast<InjectionWell*>(m->well(j));
            if(inj != 0)
            {
                WellControl *c = inj->control(i);

                *out_ecl << "        '" << inj->name() << "'   '";

                if(c->type() == WellControl::QWAT)
                {
                    *out_ecl << "WATER' 'OPEN' 'RATE'   " << c->controlVar()->value() << "/\n";
                }
                else if(c->type() == WellControl::QGAS)
                {
                    *out_ecl << "GAS' 'OPEN' 'RATE'   " << c->controlVar()->value() << "/\n";
                }
                else if(c->type() == WellControl::QOIL)
                {
                    *out_ecl << "OIL' 'OPEN' 'RATE'   " << c->controlVar()->value() << "/\n";
                }
                else
                {
                    if(inj->bhpInj() == WellControl::QWAT)
                    {
                        *out_ecl << "WATER' 'OPEN' 'BHP'   2*   " << c->controlVar()->value() << "/\n";
                    }
                    else if(inj->bhpInj() == WellControl::QGAS)
                    {
                        *out_ecl << "GAS' 'OPEN' 'BHP'   2*   " << c->controlVar()->value() << "/\n";
                    }
                    else
                    {
                        *out_ecl << "OIL' 'OPEN' 'BHP'   2*   " << c->controlVar()->value() << "/\n";
                    }
                }
            }
        }

        *out_ecl << "/\n\n";

        // --------------- time stepping ------------

        *out_ecl << "TSTEP\n";

        // calculating the time period
        double time = m->masterScheduleTime(i);
        if(i > 0) time -= m->masterScheduleTime(i-1);

        // printing the time steps

        double current_step = 1;
        *out_ecl << current_step << " ";

        while(time > 0)
        {
            time -= current_step;



            current_step = current_step*2;
            if(current_step > 50) current_step = 50;

            if(current_step > time) current_step = time;

            if(current_step > 0) *out_ecl << current_step << " ";

        }

        *out_ecl << "\n/\n\n";




    }


    return true;

}

//-----------------------------------------------------------------------------------------------
// generates the input files for the GPRS launch
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateInputFiles(Model *m)
{
    bool ok = true;

    // copying the matlab scripts if this is the first time the model is launched.

    QString base_name = m->reservoir()->file().split(".").at(0);

    if(m_first_launch)
    {


        // removing old versions of the files
        QFile::remove(folder() + "/test2.m");
        QFile::remove(folder() + "/runSim2.m");
        QFile::remove(folder() + "/initStateADI.m");



        // copying the originals
        QFile::copy(m->driverPath() + "/initStateADI.m", folder() + "/initStateADI.m");

        // generating runsim2
        AdjointsCoupledModel *am = dynamic_cast<AdjointsCoupledModel*>(m);
        generateMRSTScript(m, am != 0);


        m_first_launch = false;

    }

    // generating schedule
    generateEclIncludeFile(m);


    // removing the .mat file every time, since inconsistent results occur when not removed
    QFile::remove(folder() + "/" + base_name + ".mat");

    // removing old output file
    QFile::remove(folder() + "/" + base_name + "_RES.TXT");
    QFile::remove(folder() + "/" + base_name + "_GRAD.TXT");

    // generating the control input file
    generateControlInputFile(m);





    return ok;
}

//-----------------------------------------------------------------------------------------------
// launches MRST in batch mode
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::launchSimulator()
{
    bool ok = true;

    QProcess mrst;

    cout << "Launching MRST in batch mode..." << endl;


    QString program = "/usr/local/MATLAB/R2013a/bin/matlab";  // linux virtual
    //QString program = "matlab";   // beehive
    //QString program = "/Applications/MATLAB_R2011b.app/bin/matlab";     // mac
    //QString program = "/Volumes/SSD\ BOOT/Applications/MATLAB_R2012B.APP/bin/matlab";     // mats mac
    //QString program = "/Applications/MATLAB_R2013a.app/bin/matlab";     // eirik mac

    QStringList args;
    args.push_back("-nosplash");
    args.push_back("-nodesktop");
    args.push_back("-r");
    args.push_back("test2;exit");

    // setting up the process
    mrst.setProcessChannelMode(QProcess::MergedChannels);

    cout << "launching mrst from: " << folder().toLatin1().constData() << endl;
    // setting the working directory
    mrst.setWorkingDirectory(folder());


    // starting MRST
    mrst.start(program, args);


    mrst.waitForStarted(-1);

    cout << "MRST is running..." << endl;


    mrst.waitForFinished(-1);

    // checking the exit code

    int exit_code = mrst.exitCode();



    //cout <<  QString(mrst.readAll()).toLatin1().constData() << endl;



    cout << "MRST finished with exit code = " << exit_code << endl;






    return ok;
}

//-----------------------------------------------------------------------------------------------
// read simulator output files
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readOutput(Model *m)
{
    bool ok = true;
    // reading rates and pressures
    if(!readStandardOutput(m)) ok = false;


    // reading adjoints if needed
    AdjointsCoupledModel *am = dynamic_cast<AdjointsCoupledModel*>(m);
    if(am != 0)
    {
        if(!readAdjoints(am)) ok = false;
    }
    else cout << "No adjoints model!" << endl;


    return ok;

}


//-----------------------------------------------------------------------------------------------
// read standard output file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readStandardOutput(Model *m)
{

    bool ok = true;



    //-------------------------------------------------

    QString base_name = m->reservoir()->file().split(".").at(0);



    // opening the input file
    QFile input(folder() + "/" + base_name + "_RES.TXT");

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "### File Error! ###" << endl;
        cout << "Could not open MRST output file: " << input.fileName().toLatin1().constData() << endl;
        exit(1);
    }






    // starting to read the file



    QStringList list;

    // looping through the time steps
    for(int i = 0; i < m->masterSchedule().size(); ++i)
    {
        // looping through the wells
        for(int j = 0; j < m->numberOfWells(); ++j)
        {
            Well *w = m->well(j);


            // reading from the file
            list = processLine(input.readLine());


            // processing the line
            double q_wat = list.at(0).toDouble();
            double q_oil = list.at(1).toDouble();
            double pres = list.at(2).toDouble();

            // converting the rates and pressures to the correct units
            // production rates are negative, injection positive
            q_wat = -86400 * q_wat;
            q_oil = -86400 * q_oil;
            pres = 1e-5 * pres;


            Stream *s = new Stream(m->masterScheduleTime(i), q_oil, 0.0, q_wat, pres);

            s->setInputUnits(Stream::METRIC);

            // setting it to the well
            if(!w->setStream(i, s))
            {
                cout << endl << "###  Runtime Error  ###" << endl
                     << "Well: " << w->name().toLatin1().constData() << endl
                     << "Did not accept a stream generated by MRST..." << endl
                     << "Time = " << w->control(i)->endTime() << endl << endl;
                exit(1);

            }





        } // well j
    }   // time step i



    ++run_number;

    return ok;

}

//-----------------------------------------------------------------------------------------------
// read adjoint output file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readAdjoints(AdjointsCoupledModel *m)
{
    cout << "Reading adjoints file..." << endl;

    QString base_name = m->reservoir()->file().split(".").at(0);


    // opening the input file
    QFile input(folder() + "/" + base_name + "_GRAD.TXT");

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "### File Error! ###" << endl;
        cout << "Could not open MRST adjoints file: " << input.fileName().toLatin1().constData() << endl;
        exit(1);
    }

    cout << "starting to read" << endl;


    // ---- starting to read the adjoints -----

    // reading the order of the wells
    QList<Well*> wells;
    QStringList list = processLine(input.readLine());


    while(!input.atEnd() && !list.at(0).startsWith("STEP"))
    {

        if(!isEmpty(list))
        {

            // finding the corresponding well
            Well *w = m->wellByName(list.at(0));

            if(w != 0) wells.push_back(w);
            else
            {
                cout << endl << "### Runtime Error ###" << endl
                     << "WELL: " << list.at(0).toLatin1().constData() <<  endl
                     << "found in adjoints file, but not in model... " << endl;

                exit(1);
            }
        }

        list = processLine(input.readLine());

    }




    // starting to loop through the time steps
    for(int ts = 0; ts < m->numberOfMasterScheduleTimes(); ++ts)
    {
        // looping through the wells for the time step
        for(int i = 0; i < wells.size(); ++i)
        {
            // reading the next line
            list = processLine(input.readLine());
            if(input.atEnd())
            {
                cout << endl << "### Runtime Error ###" << endl
                     << "Reached end of file before all adjoins have been read..." << endl;

                exit(1);
            }

            // finding the correct adjoints collection
            shared_ptr<RealVariable> var = wells.at(i)->control(ts)->controlVar();
            AdjointCollection *ac = m->adjointCollection(var);


            if(ac != 0)
            {

                // looping through the adjoints in the collection
                int col = 0;
                for(int ts_i = 0; ts_i < m->numberOfMasterScheduleTimes(); ++ts_i)
                {
                    for(int k = 0; k < wells.size(); ++k)
                    {
                        Adjoint *a = ac->adjoint(wells.at(k), ts_i);

                        if(a != 0)
                        {

                            // doing unit conversions
                            double c_p = 1.0;
                            double c_q = 1.0;

                            if(wells.at(i)->control(ts)->type() == WellControl::BHP)
                            {
                                // variable is pressure
                                if(wells.at(i)->type() != wells.at(k)->type())
                                {

                                }
                                else
                                {

                                }
                                c_q = -1e5 * 86400;

                            }
                            else
                            {
                                // variable is rate
                                if(wells.at(i)->type() != wells.at(k)->type())
                                {

                                }
                                else
                                {

                                }
                                c_p = -1e-5 / 86400;
                            }

                            // if one is prod and the other is injector, switch sign


                            a->setDqoDx(list.at(col++).toDouble() * c_q);
                            a->setDqgDx(list.at(col++).toDouble() * c_q);
                            a->setDqwDx(list.at(col++).toDouble() * c_q);
                            a->setDpDx(list.at(col++).toDouble() * c_p);
                        }
                        else cout << "!!! no adjoint !!!" << endl;
                    }
                }


            }
            else
            {
                cout << endl << "### Runtime Error ###" << endl
                     << "Model not set up to accept adjoints for variable..." << endl
                     << "VARIABLE: " << wells.at(i)->control(ts)->controlVar()->name().toLatin1().constData() <<  endl;


                exit(1);
            }






        }

        // finding the next STEP header in the file
        while(!input.atEnd() && !list.at(0).startsWith("STEP"))
        {
            list = processLine(input.readLine());
        }
    }




    return true;
}



//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList MrstBatchSimulator::processLine(const QString& line)
{

    QString temp = line.trimmed();   // removing trailing and leading whitespaces


    QStringList list = temp.split(QRegExp("\\s+")); // splitting the line at whitespace


    return list;
}

//-----------------------------------------------------------------------------------------------
// Checks if the line is empty or not (white spaces)
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::isEmpty(const QStringList &list)
{
    bool ok = true;

    for(int i = 0; i < list.size(); i++)
    {
        QString temp = list.at(i);

        temp.remove(QRegExp("\\s+"));

        if(temp.size() != 0)
        {
            ok = false;
            break;
        }

    }

    return ok;
}

} // namespace ResOpt
