
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
#include "cumgasobjective.h"
#include "cumoilobjective.h"
#include "npvobjective.h"

namespace ResOpt
{


using std::cout;
using std::endl;

MrstBatchSimulator::MrstBatchSimulator() :
    m_first_launch(true)
{
}

MrstBatchSimulator::MrstBatchSimulator(const MrstBatchSimulator &m)
    : ReservoirSimulator(m),
      m_first_launch(true)
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

    // checking that the model corresponds to the fixed case we are looking at
    if(m->numberOfWells() != 3)
    {
        cout << "### Error! ###" << endl;
        cout << "The number of wells is not 3..." << endl;
        cout << "NUMBER OF WELLS = " << m->numberOfWells() << endl;

        exit(1);
    }


    // done checking, starting to generate the control file


    QFile ctrl_file(folder() + "/test2.m");

    if(!ctrl_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to control file: %s", ctrl_file.fileName().toAscii().data());
        exit(1);
    }

    QTextStream *out_ctrl = new QTextStream(&ctrl_file);


    // starting to generate the file

    *out_ctrl << "mrstPath = '/home/aleksaju/Work/postdoc/MRST/mrst-2012b';" << "\n";

    //*out_ctrl << "mrstPath = '/usr/local/MRST/mrst-2012b';" << "\n";
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

            *out_ctrl << inj->name().toAscii().data() << " = [";
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

            *out_ctrl << prod->name().toAscii().data() << " = [";
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

    for(int i = 0; i < (well_names.size()-1); ++i)
    {
        *out_ctrl << well_names.at(i) << "; ";
    }
    *out_ctrl << well_names.last() << "];\n\n";

    *out_ctrl << "try" << "\n";
    *out_ctrl << " %write control file:" << "\n";
    *out_ctrl << " fid = fopen('" <<  m->reservoir()->file().split(".").at(0) << "_CONTROLS.TXT', 'w');" << "\n";
    *out_ctrl << " fprintf(fid, '%+12.6e %+12.6e %+12.6e\\n',  [i1; p1; p2]);" << "\n";
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
bool MrstBatchSimulator::generateMRSTScript(Model *m)
{
    QFile mrst_file(folder() + "/runSim2.m");

    if(!mrst_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to MRST script file: %s", mrst_file.fileName().toAscii().data());
        exit(1);
    }

    QTextStream *out_mrst = new QTextStream(&mrst_file);

    // base file name
    QString base_name = m->reservoir()->file().split(".").at(0);

    // product prices for objective
    double price_oil = 0.0;
    double price_gas = 0.0;
    double price_wat_prod = 0.0;
    double price_wat_inj = 0.0;
    double dcf = 0.0;

    CumgasObjective *cg = dynamic_cast<CumgasObjective*>(m->objective());
    if(cg != 0) price_gas = 1.0;


    CumoilObjective *co = dynamic_cast<CumoilObjective*>(m->objective());
    if(co != 0) price_oil = 1.0;

    NpvObjective *npv = dynamic_cast<NpvObjective*>(m->objective());
    if(npv != 0)
    {
        price_oil = npv->oilPrice();
        price_gas = npv->gasPrice();
        price_wat_prod = npv->waterPrice();
        dcf = npv->dcf();

    }




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


    *out_mrst << "% Create objective functions and compute gradient\n";
    *out_mrst << "objective = @(tstep)NPVOW(G, wellSols, schedule, 'ComputePartials', true, ...\n";
    *out_mrst << "'tStep', tstep, ...\n";
    *out_mrst << "'OilPrice',             " << price_oil << " , ...\n";
    *out_mrst << "'WaterProductionCost',  " << price_wat_prod << " , ...\n";
    *out_mrst << "'WaterInjectionCost',   " << price_wat_inj << " , ...\n";
    *out_mrst << "'DiscountFactor',       " << dcf << " );\n\n";

    *out_mrst << "gradient = runAdjointADI(G, rock, fluid, schedule, objective, system, 'ForwardStates', states);\n";
    *out_mrst << "gradient = full(cell2mat(gradient));\n";
    *out_mrst << "fid = fopen(gradNm, 'w');\n";
    *out_mrst << "fprintf(fid, '%+12.6e\\n', gradient);\n";
    *out_mrst << "fclose(fid);\n\n";
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
        qWarning("Could not connect to control file: %s", ecl_file.fileName().toAscii().data());
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

    if(m_first_launch)
    {
        QString base_name = m->reservoir()->file().split(".").at(0);

        // removing old versions of the files
        QFile::remove(folder() + "/test2.m");
        QFile::remove(folder() + "/runSim2.m");
        QFile::remove(folder() + "/initStateADI.m");
        QFile::remove(folder() + "/" + base_name + ".mat");


        // copying the originals
        QFile::copy(m->driverPath() + "/initStateADI.m", folder() + "/initStateADI.m");

        // generating schedule and runsim2
        generateEclIncludeFile(m);
        generateMRSTScript(m);


        m_first_launch = false;

    }


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


    QString program = "/usr/local/MATLAB/R2013a/bin/matlab";
    //QString program = "matlab";
    QStringList args;
    args.push_back("-nosplash");
    args.push_back("-nodesktop");
    args.push_back("-r");
    args.push_back("test2;exit");

    // setting up the process
    mrst.setProcessChannelMode(QProcess::MergedChannels);

    cout << "launching mrst from: " << folder().toAscii().data() << endl;
    // setting the working directory
    mrst.setWorkingDirectory(folder());


    // starting MRST
    mrst.start(program, args);


    mrst.waitForStarted(-1);

    cout << "MRST is running..." << endl;


    mrst.waitForFinished(-1);

    // checking the exit code

    int exit_code = mrst.exitCode();



    //cout <<  QString(mrst.readAll()).toAscii().data() << endl;



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
        cout << "Could not open MRST output file: " << input.fileName().toAscii().data() << endl;
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
                     << "Well: " << w->name().toAscii().data() << endl
                     << "Did not accept a stream generated by MRST..." << endl
                     << "Time = " << w->control(i)->endTime() << endl << endl;
                exit(1);

            }





        } // well j
    }   // time step i




    return ok;

}

//-----------------------------------------------------------------------------------------------
// read adjoint output file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readAdjoints(AdjointsCoupledModel *m)
{

    QString base_name = m->reservoir()->file().split(".").at(0);


    // opening the input file
    QFile input(folder() + "/" + base_name + "_GRAD.TXT");

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "### File Error! ###" << endl;
        cout << "Could not open MRST adjoints file: " << input.fileName().toAscii().data() << endl;
        exit(1);
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


} // namespace ResOpt
