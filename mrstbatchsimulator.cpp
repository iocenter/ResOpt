
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


#include "mrstbatchsimulator.h"
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

MrstBatchSimulator::MrstBatchSimulator()
{
}

MrstBatchSimulator::MrstBatchSimulator(const MrstBatchSimulator &m)
    : ReservoirSimulator(m)
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



    return ok;



}


//-----------------------------------------------------------------------------------------------
// generates the input files for the GPRS launch
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::generateInputFiles(Model *m)
{
    bool ok = true;


    // checking if the folder exists
   // QDir dir(".");

   // if(!dir.exists(folder())) dir.mkdir(folder());
    //QDir::setCurrent(folder());


    // generating the control file
   // if(!generateControlInputFile(m)) ok = false;

    return ok;
}

//-----------------------------------------------------------------------------------------------
// launches GPRS
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::launchSimulator()
{
    bool ok = true;

    QProcess mrst;

    cout << "Launching MRST in batch mode..." << endl;

    // /usr/local/MATLAB/R2012b/bin/glnxa64/MATLAB

    QString program = "/usr/local/MATLAB/R2012b/bin/glnxa64/MATLAB -r \"runSim2;exit\"";
    QStringList args;
    //args.push_back("-r");
    //args.push_back("\"runSim2;exit\"");

    // setting up the process
    mrst.setProcessChannelMode(QProcess::MergedChannels);
    //mrst.setWorkingDirectory(folder());
    mrst.setWorkingDirectory("/home/aleksaju/Work/postdoc/Mats/mats");


    cout << program.toAscii().data() << endl;
    //cout << args.at(0).toAscii().data() << endl;

    mrst.start(program, args);

    mrst.waitForStarted(-1);

    cout << "MRST is running..." << endl;


    mrst.waitForFinished(-1);

    // checking the exit code

    int exit_code = mrst.exitCode();



    cout << QString(mrst.readAll()).toAscii().data() << endl;



    cout << "MRST finished with exit code = " << exit_code << endl;


    exit(1);



    return ok;
}

//-----------------------------------------------------------------------------------------------
// read simulator output files
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readOutput(Model *m)
{
    bool ok = true;



    return ok;
}


//-----------------------------------------------------------------------------------------------
// read well output file
//-----------------------------------------------------------------------------------------------
bool MrstBatchSimulator::readWellOutput(Well *w, QString file_name)
{
    bool ok = true;



    return ok;

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
