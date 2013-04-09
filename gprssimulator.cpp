/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
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


#include "gprssimulator.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <iostream>

#include "model.h"
#include "reservoir.h"
#include "well.h"
#include "wellconnection.h"
#include "wellcontrol.h"
#include "realvariable.h"
#include "intvariable.h"
#include "stream.h"

namespace ResOpt
{


using std::cout;
using std::endl;

GprsSimulator::GprsSimulator()
{
}

GprsSimulator::GprsSimulator(const GprsSimulator &g)
    : ReservoirSimulator(g)
{
}

GprsSimulator::~GprsSimulator()
{}

//-----------------------------------------------------------------------------------------------
// generates the main input file
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::generateMainInputFile(Model *m)
{
    bool ok = true;

    // setting up the file

    QFile main_file(folder() + "/gprs.in");

    if(!main_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to output file: %s", main_file.fileName().toAscii().data());
        exit(1);
    }

    QTextStream *out_main = new QTextStream(&main_file);


    // starting to generate the file


    // field section
    *out_main << "# --- Field Data ----------------------------------" << "\n";
    *out_main << "Field_Name: unknown" << "\n\n";

    // reservoir section
    *out_main << "# --- Reservoirs Data ------------------------------" << "\n";
    *out_main << "NumOfReservoirs  1" << "\n";
    *out_main << "INCLUDE " << m->reservoir()->file().toAscii().data() << "\n\n";
    *out_main << "END_RESERVOIRS" << "\n\n";


    // well section
    *out_main << "# --- Wells Data ----------------------------------" << "\n";
    *out_main << "NumOfWells  " << m->numberOfWells() << "\n";
    *out_main << "INCLUDE wells.in" << "\n\n";
    *out_main << "END_WELLS" << "\n\n";

    // control section
    *out_main << "# --- Control Data --------------------------------" << "\n";
    *out_main << "INCLUDE control.in" << "\n";


    // closing file
    main_file.close();
    delete out_main;

    return ok;


}

//-----------------------------------------------------------------------------------------------
// generates the well input file
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::generateWellInputFile(Model *m)
{
    bool ok = true;

    // setting up the file

    QFile well_file(folder() + "/wells.in");

    if(!well_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to output file: %s", well_file.fileName().toAscii().data());
        exit(1);
    }

    QTextStream *out_well = new QTextStream(&well_file);


    // starting to generate the file


    // looping through each well
    for(int i = 0; i < m->numberOfWells(); i++)
    {
        Well *w = m->well(i);

        // headers
        *out_well << "# //////// WELL No. " << i+1 << " ////////////////////" << "\n";
        *out_well << "# --- well definition -----" << "\n";
        *out_well << "#WELLSPECS" << "\n";
        *out_well << "#  WELL_NAME\tGROUP\tRES_NAME\tTYPE\tSTATUS" << "\n";

        // wellspecs
        *out_well << "   "
                  << w->name().toAscii().data() << "\t"
                  << w->group().toAscii().data() << "\t"
                  << m->reservoir()->name().toAscii().data() << "\t"
                  << w->typeAsString().toAscii().data() << "\t"
                  << "OPEN" << "\n\n";

        *out_well << "END" << "\n\n";

        // well connections
        *out_well << "# --- well connections (trajestry) -------" << "\n";
        *out_well << "#COMPDAT" << "\n";
        *out_well << "number_of_connections " << w->numberOfConnections() << "\n";
        *out_well << "#LOC(ijk)\tWI" << "\n";

        for(int j = 0; j < w->numberOfConnections(); j++)   // looping through conections
        {
            *out_well << " " << w->connection(j)->cell() << "\t"
                      << w->connection(j)->wellIndex() << "\n";
        }

        *out_well << "\n" << "END" << "\n\n";

        // well control
        *out_well << "# --- well control ----------------------" << "\n";
        *out_well << "#WCONPROD" << "\n";

        double start_time = 0.0;

        // checking if this is a producer or injector
        if(w->type() == Well::PROD)     // producer
        {
            *out_well << "# ctrl\tt_start\tt_end\trate\tbhp\tstd_den\n";

            for(int k = 0; k < w->numberOfControls(); k++)  // looping through all the schedule elements
            {
                WellControl *wc = w->control(k);

                // checking if the well is installed
                if(!w->isInstalled(k))
                {
                    // the well is not installed, printing zero rate
                    *out_well << "  TORATE" << "\t"
                              << start_time << "\t"
                              << wc->endTime() << "\t"
                              << "0.0" << "\t"
                              << w->bhpLimit() << "\t"
                              << m->reservoir()->oilDensity() << "\n";

                }
                else    // the well is installed
                {

                    // the type of control
                    if(wc->type() == WellControl::BHP)  // BHP control
                    {
                        *out_well << "  TBHP" << "\t"
                                  << start_time << "\t"
                                  << wc->endTime() << "\t\t"
                                  << wc->controlVar()->value() << "\n";

                    }
                    else
                    {
                        // control type
                        if(wc->type() == WellControl::QGAS) *out_well << "  TGRATE" << "\t";        // gas producer
                        else if(wc->type() == WellControl::QOIL) *out_well << "  TORATE" << "\t";   // oil producer
                        else *out_well << "  TWRATE" << "\t";                                       // water producer

                        // times, rate, ++
                        *out_well << start_time << "\t"
                                  << wc->endTime() << "\t"
                                  << wc->controlVar()->value() << "\t"
                                  << w->bhpLimit() << "\t";

                        // density
                        if(wc->type() == WellControl::QGAS) *out_well << m->reservoir()->gasDensity() << "\n";        // gas producer
                        else if(wc->type() == WellControl::QOIL) *out_well << m->reservoir()->oilDensity() << "\n";   // oil producer
                        else *out_well << m->reservoir()->waterDensity() << "\n";                                     // water producer


                    }
                }

                // updating the start time
                start_time = wc->endTime();
            }

        }
        else    // injector
        {


            *out_well << "# ctrl\tt_start\tt_end\trate\tbhp\tstd_den\tnc\tc1\tc2\tc3\n";

            double n_phases = m->reservoir()->numberOfPhases();

            for(int k = 0; k < w->numberOfControls(); k++)  // looping through all the schedule elements
            {
                WellControl *wc = w->control(k);

                // checking if the well is installed
                if(!w->isInstalled(k))
                {
                    // the well is not installed, printing zero pressure
                    *out_well << "  TBHP" << "\t"
                          << start_time << "\t"
                          << wc->endTime() << "\t\t"
                          << "0.0" << "\t\t"
                          << n_phases << "\t";

                    // checking what phase should be injected, and what phases are present
                    bool gas = m->reservoir()->gasPhase();
                    bool wat = m->reservoir()->waterPhase();


                    if(n_phases == 3)   // gas oil water
                    {
                        if(w->bhpInj() == WellControl::QWAT)    // injecting water
                        {
                            *out_well << "0.0\t0.0\t1.0" << "\n";
                        }
                        else                                    // injecting gas
                        {
                                *out_well << "1.0\t0.0\t0.0" << "\n";
                        }
                    }
                    else if(n_phases == 2) // gas oil, gas water, or oil water
                    {
                        if(gas && wat)
                        {
                            if(w->bhpInj() == WellControl::QWAT) *out_well << "0.0\t1.0" << "\n";
                            else *out_well << "1.0\t0.0" << "\n";
                        }
                        else *out_well << "1.0\t0.0" << "\n";   // oil is never injected...
                    }


                }

                else    // the well is installed
                {


                    // the type of control
                    if(wc->type() == WellControl::BHP)  // BHP control
                    {
                        *out_well << "  TBHP" << "\t"
                                  << start_time << "\t"
                                  << wc->endTime() << "\t\t"
                                  << wc->controlVar()->value() << "\t\t"
                                  << n_phases << "\t";

                        // checking what phase should be injected, and what phases are present
                        bool gas = m->reservoir()->gasPhase();
                        bool wat = m->reservoir()->waterPhase();


                        if(n_phases == 3)   // gas oil water
                        {
                            if(w->bhpInj() == WellControl::QWAT)    // injecting water
                            {
                                *out_well << "0.0\t0.0\t1.0" << "\n";
                            }
                            else                                    // injecting gas
                            {
                                    *out_well << "1.0\t0.0\t0.0" << "\n";
                            }
                        }
                        else if(n_phases == 2) // gas oil, gas water, or oil water
                        {
                            if(gas && wat)
                            {
                                if(w->bhpInj() == WellControl::QWAT) *out_well << "0.0\t1.0" << "\n";
                                else *out_well << "1.0\t0.0" << "\n";
                            }
                            else *out_well << "1.0\t0.0" << "\n";   // oil is never injected...
                        }



                    }
                    else
                    {
                        // control type
                        if(wc->type() == WellControl::QGAS) *out_well << "  TGRATE" << "\t";        // gas injector
                        else *out_well << "  TWRATE" << "\t";                                       // water injector

                        // times, rate, ++
                        *out_well << start_time << "\t"
                                  << wc->endTime() << "\t"
                                  << wc->controlVar()->value() << "\t"
                                  << w->bhpLimit() << "\t";

                        // density
                        if(wc->type() == WellControl::QGAS) *out_well << m->reservoir()->gasDensity() << "\t";        // gas producer
                        else if(wc->type() == WellControl::QOIL) *out_well << m->reservoir()->oilDensity() << "\t";   // oil producer
                        else *out_well << m->reservoir()->waterDensity() << "\t";                                     // water producer

                        *out_well << n_phases << "\t";  // number of phases


                        // checking what phase should be injected, and what phases are present
                        bool gas = m->reservoir()->gasPhase();
                        bool wat = m->reservoir()->waterPhase();


                        if(n_phases == 3)   // gas oil water
                        {
                            if(wc->type() == WellControl::QWAT)    // injecting water
                            {
                                *out_well << "0.0\t0.0\t1.0" << "\n";
                            }
                            else                                    // injecting gas
                            {
                                *out_well << "1.0\t0.0\t0.0" << "\n";
                            }
                        }
                        else if(n_phases == 2) // gas oil, gas water, or oil water
                        {
                            if(gas && wat)
                            {
                                if(wc->type() == WellControl::QWAT) *out_well << "0.0\t1.0" << "\n";
                                else *out_well << "1.0\t0.0" << "\n";
                            }
                            else *out_well << "1.0\t0.0" << "\n";   // oil is never injected...
                        }



                    }
                }

                // updating the start time
                start_time = wc->endTime();
            }



        }

        // done with the schedule
        *out_well << "END" << "\n";

        //done with the well
        *out_well << "END_WELL" << "\n\n";


    }


    // closing file
    well_file.close();
    delete out_well;

    return ok;


}


//-----------------------------------------------------------------------------------------------
// generates the control input file
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::generateControlInputFile(Model *m)
{
    bool ok = true;

    // setting up the file

    //QFile ctrl_file("control.in");
    QFile ctrl_file(folder() + "/control.in");

    if(!ctrl_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to output file: %s", ctrl_file.fileName().toAscii().data());
        exit(1);
    }

    QTextStream *out_ctrl = new QTextStream(&ctrl_file);


    // starting to generate the file



    *out_ctrl << "#TUNING\n"
              << "#-- TSINIT TSMAXZ  TSMIN\n"
              << "1   60  0.1\n\n"
              << "#TSTEP\n";

    *out_ctrl << m->reservoir()->endTime() << "\n\n";

    *out_ctrl << "# --- control parameters ------\n"
              << "# --- iteration control -----\n"
              << "# minNewtonIter	maxNewtonIter	fixedNewtonIter\n"
              << "1	20	30\n\n"
              << "# --- time step size control -------\n"
              << "# dp	ds	dx	w\n"
              << " 300    0.3	 0.05    0.5\n\n"
              << "# --- convergence control ----\n"
              << "# relMBError	relPEError	relPChange  absSChange	absMFChange relWellEqError\n"
              << "0.15	0.03	0.0002	  0.007	  0.002 0.002\n\n"
              << "# --- solution method control ---\n"
             << "# variType	nImpTypes  Percentages(AIM)\n"
             << "1	1\n"
             << "# nImpVars  transOption  CFL_Limit\n"
             << "3	4	1.0\n\n"
             << "# --- linear solver control ---\n"
             << "# LinearSolverChoice  Preconditioner  tol  maxIter reStartNo\n"
             << "4		    3         1E-6   1500		10\n\n"
             << "# --- debug information ---\n"
             << "#Flag   NumOfTimeSteps\n"
             << "1	10\n\n"
             << "# --- Re-start file setting information ---\n"
             << "#Flag   NumOfTimeSteps output-control\n"
             << "0	  1		1\n\n"
             << "END_CONTROL\n";


    // closing file
    ctrl_file.close();
    delete out_ctrl;

    return ok;



}


//-----------------------------------------------------------------------------------------------
// generates the input files for the GPRS launch
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::generateInputFiles(Model *m)
{
    bool ok = true;


    // first generating the main input file (gprs.in)
    if(!generateMainInputFile(m)) ok = false;

    // generating the well include file
    if(!generateWellInputFile(m)) ok = false;

    // generating the control file
    if(!generateControlInputFile(m)) ok = false;

    return ok;
}

//-----------------------------------------------------------------------------------------------
// launches GPRS
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::launchSimulator()
{
    bool ok = true;

    QProcess gprs;

    cout << "Launching GPRS..." << endl;

    QString program = "gprs";
    QStringList args;

    // setting up the process
    gprs.setProcessChannelMode(QProcess::MergedChannels);
    gprs.setWorkingDirectory(folder());




    gprs.start(program, args);

    gprs.waitForStarted(-1);

    cout << "GPRS is running..." << endl;


    gprs.waitForFinished(-1);

    // checking the exit code

    int exit_code = gprs.exitCode();



    //cout << QString(gprs.readAll()).toAscii().data() << endl;



    cout << "GPRS finished with exit code = " << exit_code << endl;



    return ok;
}

//-----------------------------------------------------------------------------------------------
// read simulator output files
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::readOutput(Model *m)
{
    bool ok = true;

    // looping through the wells
    for(int i = 0; i < m->numberOfWells(); i++)
    {
        // generating the file name
        QString fn = folder() + "/" + m->reservoir()->name() + "_" + m->well(i)->name() + ".out";

        // reading the well output
        readWellOutput(m->well(i), fn);
    }

    // checking that an equal number of time steps was read for each well
    if(m->numberOfWells() > 1)
    {
        int n_stream = m->well(0)->numberOfStreams();

        for(int i = 1; i < m->numberOfWells(); i++)
        {
            if(n_stream != m->well(i)->numberOfStreams())
            {
                cout << endl << "### Warning ###" << endl
                     << "From: GPRS" << endl
                     << "Did not read the same number of time steps from each well..." << endl << endl;


                ok = false;
            }
        }
    }

    return ok;
}


//-----------------------------------------------------------------------------------------------
// read well output file
//-----------------------------------------------------------------------------------------------
bool GprsSimulator::readWellOutput(Well *w, QString file_name)
{
    bool ok = true;

    // opening the input file
    QFile input(file_name);

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Could not open WELL output file: %s", file_name.toAscii().data());
        exit(1);
    }


    // starting to read the file

    //cout << "Reading simulator output for WELL: " << w->name().toAscii().data() << "..." << endl;


    QStringList list;

    int n_streams = 0;

    QVector<Stream*> raw_output;    // output streams from the simulator, this must be averaged


    while(!input.atEnd() && ok)
    {
        list = processLine(input.readLine());   // reading a line from the output file

        if(list.size() == 6)    // a data line has 6 entries (time, bhp, temp, qg, qo, qw)
        {
            // trying to convert line to numbers
            bool ok_line = true;
            QVector<double> nums;

            for(int i = 0; i < 6; i++)
            {
                nums.push_back(list.at(i).toDouble(&ok_line));
                if(!ok_line) break;
            }

            // checking if the line was converted ok
            if(ok_line)
            {
                Stream *s = new Stream();

                s->setTime(nums.at(0));
                s->setPressure(nums.at(1));
                s->setGasRate(nums.at(3));
                s->setOilRate(nums.at(4));
                s->setWaterRate(nums.at(5));

                raw_output.push_back(s);
                n_streams++;

            }
        }

    }

   // cout << "Read " << n_streams << " streams from the simulator..." << endl;


    // now averaging the raw streams according to the control schedule

    int current_place = 0;
    double t_start = 0;

    for(int i = 0; i < w->numberOfControls(); ++i)
    {

        // creating a subset of the raw data for averaging
        QVector<Stream*> raw_subset;
        while(current_place < raw_output.size() && raw_output.at(current_place)->time() <= w->control(i)->endTime())
        {
            raw_subset.push_back(raw_output.at(current_place));
            ++current_place;
        }

        // making an average stream
        Stream *avg_s = new Stream();

        // checking if the raw subset contains any data (if not, the simulator probably didnt converge and quit before it was time)
        if(raw_subset.size() > 0)
        {
            // checking that the last stream in the subset corresponds to the end time of the control
            if(raw_subset.at(raw_subset.size()-1)->time() < w->control(i)->endTime())
            {

                cout << endl << "###  Warning  ###" << endl
                     << "Problem detected with GPRS output..." << endl
                     << "The simulator did not run to the end." << endl
                     << "Last time step: " << raw_subset.at(raw_subset.size()-1)->time() << "(days)" << endl
                     << "Expected: " << w->control(i)->endTime() << "(days)" << endl << endl;

                // adding an empty stream for the remainder of the time
                Stream *s_add = new Stream();
                s_add->setGasRate(0);
                s_add->setOilRate(0);
                s_add->setWaterRate(0);
                s_add->setPressure(0);
                s_add->setTime(w->control(i)->endTime());

                raw_subset.push_back(s_add);



            }
            avg_s->avg(raw_subset, t_start);

        }
        else
        {
            cout << endl << "###  Warning  ###" << endl
                 << "Problem detected with GPRS output..." << endl
                 << "The simulator did not run to the end." << endl
                 << "Last time step: 0 (days)" << endl
                 << "Expected: " << w->control(i)->endTime() << "(days)" << endl << endl;


            // didnt find any info from the simulator, just making an empty stream
            avg_s->setGasRate(0);
            avg_s->setOilRate(0);
            avg_s->setWaterRate(0);
            avg_s->setPressure(0);
            avg_s->setTime(w->control(i)->endTime());




        }
        // setting it to the well
        if(!w->setStream(i, avg_s))
        {
            cout << endl << "###  Runtime Error  ###" << endl
                 << "Well: " << w->name().toAscii().data() << endl
                 << "Did not accept a stream generated by GPRS..." << endl
                 << "Time = " << w->control(i)->endTime() << endl << endl;
            exit(1);

        }

        t_start = w->control(i)->endTime();
    }

    // deleting the raw output
    for(int i = 0; i < raw_output.size(); ++i) delete raw_output.at(i);


    return ok;

}

//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList GprsSimulator::processLine(const QString& line)
{

    QString temp = line.trimmed();   // removing trailing and leading whitespaces


    QStringList list = temp.split(QRegExp("\\s+")); // splitting the line at whitespace


    return list;
}


} // namespace ResOpt
