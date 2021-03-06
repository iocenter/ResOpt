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


#include "modelreader.h"

#include <iostream>
#include <tr1/memory>
#include <QThread>
#include <QFileInfo>
#include <QDir>

#include "runner.h"
#include "coupledmodel.h"
#include "decoupledmodel.h"
#include "adjointscoupledmodel.h"
#include "reservoir.h"
#include "injectionwell.h"
#include "productionwell.h"
#include "intvariable.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "wellcontrol.h"
#include "wellconnection.h"
#include "wellconnectionvariable.h"
#include "wellpath.h"
#include "pipeconnection.h"
#include "npvobjective.h"
#include "cumoilobjective.h"
#include "cumgasobjective.h"
#include "endpipe.h"
#include "midpipe.h"
#include "separator.h"
#include "pressurebooster.h"
#include "capacity.h"
#include "pipeconnection.h"
#include "cost.h"
#include "userconstraint.h"

#include "stream.h"
#include "beggsbrillcalculator.h"
#include "gprssimulator.h"

#include "opt/runonceoptimizer.h"
#include "opt/bonminoptimizer.h"
#include "opt/nomadoptimizer.h"
#include "opt/ipoptoptimizer.h"
#include "opt/lshoptimizer.h"
#include "opt/nomadipoptoptimizer.h"
#include "opt/eroptoptimizer.h"

#include "gprssimulator.h"
#include "vlpsimulator.h"
#include "mrstbatchsimulator.h"
#include "logger.h"

using std::tr1::shared_ptr;
using std::cout;
using std::endl;


namespace ResOpt
{

ModelReader::ModelReader(const QString &driver)
    : m_driver_file_name(driver)
{

}



//-----------------------------------------------------------------------------------------------
// Reads the driver file that defines the optimization problem
//-----------------------------------------------------------------------------------------------
Model* ModelReader::readDriverFile(Runner *r)
{

    if(m_driver_file_name.isEmpty())
    {

        r->logger()->error("Could not open driver file!!");

        return 0;
    }

    m_driver_file.setFileName(m_driver_file_name);

    //checking if file opened ok...
    if(!m_driver_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qWarning("Could not open driver file: %s", m_driver_file.fileName().toStdString());
        //qWarning("Could not open driver file!");
        //exit(1);

        r->logger()->error("Could not open driver file!!");

        return 0;
    }

    // setting the path of the driver file
    QFileInfo info(m_driver_file);
    m_path = info.absoluteDir().absolutePath();
    cout << "path to driver file: " << m_path.toLatin1().constData() << endl;




    Model *p_model = 0;

    cout << "****  Reading driver file: " << m_driver_file.fileName().toLatin1().constData() << "  ****" << endl;

    QStringList list;


    bool ok = true;


    list = processLine(m_driver_file.readLine());

    // the first keyword in the driver file must specify the type of model (COUPLED, DECOUPLED)
    bool found_model_def = false;
    while(!found_model_def && !m_driver_file.atEnd() && !list.at(0).startsWith("EOF"))
    {
        if(list.at(0).startsWith("COUPLED"))            // Coupled model
        {
            cout << "Generating a COUPLED model..." << endl;
            p_model = new CoupledModel();
            found_model_def = true;
        }
        else if(list.at(0).startsWith("DECOUPLED"))     // Decoupled model
        {
            cout << "Generating a DECOUPLED model..." << endl;
            p_model = new DecoupledModel();
            found_model_def = true;
        }
        else if(list.at(0).startsWith("ADJOINTS_COUPLED"))
        {
            cout << "Generating a COUPLED model with ADJOINTS..." << endl;
            AdjointsCoupledModel *am = new AdjointsCoupledModel();
            p_model = am;
            found_model_def = true;
        }
        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The first keyword must specify the model type..." << endl
                     << "Possible types: COUPLED, DECOUPLED, ADJOINTS_COUPLED" << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);
            }

        }

        list = processLine(m_driver_file.readLine());

    }


    // reading the remaining input


    while(!m_driver_file.atEnd() && !list.at(0).startsWith("EOF"))
    {


        if(list.at(0).startsWith("START")) // the start of a new well or reservoir
        {
            if(list.at(1).startsWith("RESERVOIR")) p_model->setReservoir(readReservoir());                      // reservoir
            else if(list.at(1).startsWith("WELL")) p_model->addWell(readWell());                                // new well
            else if(list.at(1).startsWith("OBJECTIVE")) p_model->setObjective(readObjective());                 // objective
            else if(list.at(1).startsWith("PIPE")) p_model->addPipe(readPipe());                                // pipe
            else if(list.at(1).startsWith("CAPACITY")) p_model->addCapacity(readCapacity());                    // capacity
            else if(list.at(1).startsWith("OPTIMIZER")) readOptimizer(r);                                       // optimizer
            else if(list.at(1).startsWith("MASTERSCHEDULE")) p_model->setMasterSchedule(readMasterSchedule());  // master schedule
            else if(list.at(1).startsWith("SEPARATOR")) p_model->addPipe(readSeparator());                      // separator
            else if(list.at(1).startsWith("BOOSTER")) p_model->addPipe(readPressureBooster());                  // booster
            else if(list.at(1).startsWith("CONSTRAINTS")) readUserDefinedConstraints(p_model);                  // user defined constraints

        }
        else if(list.at(0).startsWith("DEBUG"))
        {
            //cout << "model reader path: " << m_path.toLatin1().constData() << endl;
            r->setDebugFileName(m_path + "/" + list.at(1));                                    // setting the debug file
        }
        else if(list.at(0).startsWith("SIMULATOR"))     // reading the type of reservoir simulator to use
        {
            if(list.at(1).startsWith("GPRS")) r->setReservoirSimulator(new GprsSimulator());
            else if(list.at(1).startsWith("VLP")) r->setReservoirSimulator(new VlpSimulator());
            else if(list.at(1).startsWith("MRST_BATCH")) r->setReservoirSimulator(new MrstBatchSimulator());
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "Type of SIMULATOR not understood..." << endl
                     << "Possible types: GPRS, VLP" << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);
            }
        }

        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }


        if(!ok) break;

        list = processLine(m_driver_file.readLine());

    }

    if(ok)
    {
        // set to model


        cout << "Done reading input: All OK" << endl;
    }

    else
    {
        cout << "### Error: Malformed input! ###" << endl;
        exit(1);
    }


    // setting the driver file path to the model
    p_model->setDriverPath(m_path);


    return p_model;
}

//-----------------------------------------------------------------------------------------------
// Reads the master schedule part of the driver file
//-----------------------------------------------------------------------------------------------
QVector<double> ModelReader::readMasterSchedule()
{
    cout << "Reading master schedule..." << endl;

    QStringList list;

    QVector<double> l_schedule;

    bool ok = true;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(!isEmpty(list))
        {
            l_schedule.push_back(list.at(0).toDouble(&ok));

        }

        if(!ok) break;


        list = processLine(m_driver_file.readLine());

    }

    // checking if the numbers were read ok
    if(!ok)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "MASTERSCHEDULE is not in the right format..."
             << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

        exit(1);
    }

    // checking that the schedule is in ascending order
    for(int i = 1; i < l_schedule.size(); ++i)
    {
        if(l_schedule.at(i-1) >= l_schedule.at(i))
        {
            cout << endl << "### Error detected in input file! ###" << endl
                 << "MASTERSCHEDULE is not in ascending order..."
                 << l_schedule.at(i-1) << " >= " << l_schedule.at(i) << endl << endl;

            exit(1);


        }
    }

    // everything ok

    return l_schedule;


}

//-----------------------------------------------------------------------------------------------
// Reads a reservoir definition
//-----------------------------------------------------------------------------------------------
Reservoir* ModelReader::readReservoir()
{
    cout << "Reading reservoir definition..." << endl;

    Reservoir *res = new Reservoir();


    QStringList list;

    QString l_name = "";
    QString l_file = "";
    double l_endtime = -1.0;
    bool ok = true;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.at(0).startsWith("NAME")) l_name = list.at(1);                          // getting the reservoir name
        else if(list.at(0).startsWith("FILE")) l_file = list.at(1);                     // getting the file name
        else if(list.at(0).startsWith("MRST")) res->setMrstPath(list.at(1));            // setting the MRST path
        else if(list.at(0).startsWith("MATLAB")) res->setMatlabPath(list.at(1));        // setting the Matlab path
        else if(list.at(0).startsWith("SCRIPT")) res->setMrstScript(list.at(1));        // setting a custom MRST script
        else if(list.at(0).startsWith("KEEP_MAT_FILE")) res->setKeepMatFile(true);      // setting that the .mat file should not be deleted between runs
        else if(list.at(0).startsWith("TIME")) l_endtime = list.at(1).toDouble(&ok);    // getting the file name
        else if(list.at(0).startsWith("PHASES"))                                        // getting the phases present in the reservoir
        {
            if(list.length() != 4) // wrong number of arguments
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "PHASES keyword is not in the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);

            }

            bool l_gas = false;
            bool l_oil = false;
            bool l_wat = false;
            if(list.at(1).toInt() == 1) l_gas = true;
            if(list.at(2).toInt() == 1) l_oil = true;
            if(list.at(3).toInt() == 1) l_wat = true;

            res->setPhases(l_gas, l_oil, l_wat);        // setting the phases to reservoir
        }

        else if(list.at(0).startsWith("DENS"))                         // getting the phase densities
        {
            if(list.length() != 4) // wrong number of arguments
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "DENS keyword is not in the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);

            }

            double l_gas = list.at(1).toDouble();
            double l_oil = list.at(2).toDouble();
            double l_wat = list.at(3).toDouble();

            res->setDensities(l_gas, l_oil, l_wat);     // setting the phase densities to the reservoir

        }

        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }


        list = processLine(m_driver_file.readLine());

    }

    if(l_name.compare("") == 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The reservoir NAME was not defined..." << endl;
        exit(1);

    }
    if(l_file.compare("") == 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The reservoir FILE was not defined..." << endl;
        exit(1);

    }

    if(!ok || l_endtime < 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The reservoir TIME was not defined propperly..." << endl
             << "TIME = " << l_endtime << endl;
        exit(1);

    }


    // everything ok, setting to reservoir
    res->setName(l_name);
    res->setFile(l_file);
    res->setEndTime(l_endtime);


    return res;
}

//-----------------------------------------------------------------------------------------------
// Reads a well definition
//-----------------------------------------------------------------------------------------------
Well* ModelReader::readWell()
{
    cout << "Reading well definition..." << endl;

    Well *w = 0;


    QStringList list;

    QString l_name = "";
    QString l_group = "";
    double l_bhp_limit = 0.0;
    WellControl::contol_type l_bhp_inj = WellControl::QWAT;
    bool ok_bhp = false;

    Cost *well_cost = 0;
    shared_ptr<IntVariable> var_install;




    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.at(0).startsWith("NAME")) l_name = list.at(1);                                      // getting the reservoir name
        else if(list.at(0).startsWith("GROUP")) l_group = list.at(1);                               // getting the file name
        else if(list.at(0).startsWith("BHPLIMIT")) l_bhp_limit = list.at(1).toDouble(&ok_bhp);      // getting the BHP limit
        else if(list.at(0).startsWith("COST"))                                                      // the cost
        {

            bool ok_cost = true;
            double cost = list.at(1).toDouble(&ok_cost);

            if(!ok_cost)
            {
                cout << endl << "### Error detected in input file! ###" << endl
                                << "Could not convert COST to number..." << endl
                                << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);

            }

            well_cost = new Cost();
            well_cost->setConstant(cost);


        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            var_install = shared_ptr<IntVariable>(new IntVariable(w));


            bool ok = true;

            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);

                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }

            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "INSTALLTIME for WELL not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }

            if(!ok)
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "INSTALLTIME for WELL not in correct format..." << endl
                     << "Problem detected when converting to numbers."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }

        }

        else if(list.at(0).startsWith("BHPINJ"))                                                    // getting the default injection phase
        {
            if(list.at(1).startsWith("WATER")) l_bhp_inj = WellControl::QWAT;
            else if(list.at(1).startsWith("GAS")) l_bhp_inj = WellControl::QGAS;
            else if(list.at(1).startsWith("OIL")) l_bhp_inj = WellControl::QOIL;
        }
        else if(list.at(0).startsWith("TYPE"))                                                      // getting the type of well
        {
            if(list.at(1).startsWith("P"))         // producer
            {
                ProductionWell *prod_well = new ProductionWell();
                if(l_name.compare("") != 0) prod_well->setName(l_name);

                w = prod_well;
                w->setAutomaticType();

            }
            else if(list.at(1).startsWith("I"))    // injector
            {
                w = new InjectionWell();
                w->setAutomaticType();
                if(l_name.compare("") != 0) w->setName(l_name);
            }
            else                                                        // not recognized
            {
                cout << endl << "### Error detected in input file! ###" << endl
                                << "Well TYPE: " << list.at(1).toLatin1().constData() << endl
                                << "Is not a recognized well type." << endl << endl;

                exit(1);
            }
        }


        else if(list.at(0).startsWith("START"))                             // A subsection (connections, schedule)
        {
            // first checking that the well has been initialized
            if(w == 0)
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The well TYPE must be specified before any of the START sections..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;

                exit(1);
            }

            if(list.at(1).startsWith("SCHEDULE")) readWellSchedule(w);                  // reading SCHEDULE
            else if(list.at(1).startsWith("CON")) readWellConnections(w);               // reading CONNECTIONS
            else if(list.at(1).startsWith("VARCON")) readVariableWellConnections(w);    // reading VARCONNECTIONS
            else if(list.at(1).startsWith("WELLPATH")) readWellPath(w);                 // reading WELLPATH
            else if(list.at(1).startsWith("OUTLETPIPE"))                                // reading OUTLETPIPES
            {

                // checking if this is a production well
                ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);
                if(prod_well == 0)
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << " OUTLETPIPES can not be defined for INJECTION wells..."
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                    exit(1);

                }

                // getting the pipe connections
                readPipeConnections(prod_well);

            }
            else if(list.at(1).startsWith("GASLIFT"))                    // reading GASLIFT
            {

                // checking if this is a production well
                ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);
                if(prod_well == 0)
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << " GASLIFT can not be defined for INJECTION wells..."
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                    exit(1);

                }

                // getting the pipe connections
                readGasLiftSchedule(prod_well);

            }
            else                                                            // unknown keyword
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);

            }
        }


        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }


        list = processLine(m_driver_file.readLine());

    }

    // checking remaining input

    if(l_name.compare("") == 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The well NAME was not defined..." << endl;
        exit(1);

    }
    if(l_group.compare("") == 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The well GROUP was not defined..." << endl;
        exit(1);

    }
    if(!ok_bhp)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The well BHPLIMIT was not defined..." << endl
             << "Assuming a BHPLIMIT of 15 psia...";

        l_bhp_limit = 15.0;


    }
    if(l_name.contains("_"))
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The well NAME can not contain underscore (_)..." << endl
             << "Well name: " <<l_name.toLatin1().constData() << endl;
        exit(1);

    }


    // everything ok, setting to well
    w->setName(l_name);
    w->setGroup(l_group);
    w->setBhpLimit(l_bhp_limit);
    w->setBhpInj(l_bhp_inj);

    if(well_cost != 0) w->setCost(well_cost);
    if(var_install != 0)
    {
        var_install->setName("Installation time variable for well: " + l_name);
        w->setInstallTime(var_install);
    }



    return w;
}


//-----------------------------------------------------------------------------------------------
// Reads the well SCHEDULE
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellSchedule(Well *w)
{
    cout << "        schedule..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(list.size() == 5)        // correct number of elements (t_end, value, max, min, type)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 4; i++)  // looping through t_end, value, max, min
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                shared_ptr<RealVariable> var(new RealVariable(w));
                var->setValue(nums.at(1));
                var->setMax(nums.at(2));
                var->setMin(nums.at(3));
                var->setName("Control variable for well: " + w->name() + ", until time = " + QString::number(nums.at(0)));

                WellControl *control = new WellControl();

                control->setEndTime(nums.at(0));
                control->setControlVar(var);


                // checking the "type"
                if(list.at(4).startsWith("OIL")) control->setType(WellControl::QOIL);
                else if(list.at(4).startsWith("GAS")) control->setType(WellControl::QGAS);
                else if(list.at(4).startsWith("WAT")) control->setType(WellControl::QWAT);
                else if(list.at(4).startsWith("BHP")) control->setType(WellControl::BHP);
                else
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The SCHEDULE type was not recognized..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                    delete control;
                    control = 0;

                    exit(1);

                }

                w->addControl(control);
            }
            else     // error when reading numbers in the schedule line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The SCHEDULE entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }

        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The SCHEDULE entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;

}

//-----------------------------------------------------------------------------------------------
// Reads the GASLIFT section for a production well
//-----------------------------------------------------------------------------------------------
bool ModelReader::readGasLiftSchedule(ProductionWell *w)
{
    cout << "        gas lift schedule..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(list.size() == 4)        // correct number of elements (t_end, value, max, min)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 4; i++)  // looping through t_end, value, max, min
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                shared_ptr<RealVariable> var(new RealVariable(w));
                var->setValue(nums.at(1));
                var->setMax(nums.at(2));
                var->setMin(nums.at(3));
                var->setName("Gas lift variable for well: " + w->name() + ", until time = " + QString::number(nums.at(0)));

                WellControl *control = new WellControl();

                control->setEndTime(nums.at(0));
                control->setType(WellControl::QGAS);
                control->setControlVar(var);

                w->addGasLiftControl(control);
            }



            else     // error when reading numbers in the schedule line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The GASLIFT entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }

        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The GASLIFT entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;

}


//-----------------------------------------------------------------------------------------------
// Reads the well CONNECTIONS (perforations)
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellConnections(Well *w)
{
    cout << "        connections table..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(list.size() == 2)        // correct number of elements (cell, wi)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 2; i++)  // looping through cell, wi
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                WellConnection *con = new WellConnection();

                con->setCell(nums.at(0));
                con->setWellIndex(nums.at(1));


                w->addConnection(con);
            }
            else     // error when reading numbers in the connections line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The CONNECTIONS entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }
        else if(list.size() == 5) // correct number of elements (i, j, k1, k2, PI)
        {
            // checking the numbers
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 5; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {

                WellConnection *con = new WellConnection();

                con->setI(nums.at(0));
                con->setJ(nums.at(1));
                con->setK1(nums.at(2));
                con->setK2(nums.at(3));
                con->setWellIndex(nums.at(4));

                w->addConnection(con);
            }
            else     // error when reading numbers in the connections line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The CONNECTIONS entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }



        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The CONNECTIONS entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;

}

//-----------------------------------------------------------------------------------------------
// Reads the well VARCONNECTIONS (variable perforations)
//-----------------------------------------------------------------------------------------------
bool ModelReader::readVariableWellConnections(Well *w)
{
    cout << "        variable connections table..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.size() == 9) // correct number of elements (i, i_max, i_min, j, j_max, j_min, k1, k2, PI)
        {
            // checking the numbers
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 9; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                WellConnectionVariable *wcv = new WellConnectionVariable();

                shared_ptr<IntVariable> i_var(new IntVariable(w));
                i_var->setValue(nums.at(0));
                i_var->setMax(nums.at(1));
                i_var->setMin(nums.at(2));
                wcv->setIVariable(i_var);

                shared_ptr<IntVariable> j_var(new IntVariable(w));
                j_var->setValue(nums.at(3));
                j_var->setMax(nums.at(4));
                j_var->setMin(nums.at(5));
                wcv->setJVariable(j_var);

                wcv->setK1(nums.at(6));
                wcv->setK2(nums.at(7));
                wcv->setWellIndex(nums.at(8));

                w->addVariableConnection(wcv);

            }
            else     // error when reading numbers in the connections line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The VARCONNECTIONS entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }



        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The VARCONNECTIONS entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;

}

//-----------------------------------------------------------------------------------------------
// Reads the well WELLPATH
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellPath(Well *w)
{
    cout << "        well path..." << endl;

    bool ok = true;
    WellPath *wp = new WellPath();


    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END") && !list.at(1).startsWith("WELLPATH"))
    {


        if(list.at(0).startsWith("START") && list.at(1).startsWith("TOE")) readWellPathToe(wp, w);
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("HEEL")) readWellPathHeel(wp, w);
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("OPTIONS")) readWellPathOptions(wp, w);

        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The WELLPATH entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }

        list = processLine(m_driver_file.readLine());
    }


    if(!wp->initialize())
    {
        cout << "WELLPATH did not initialize" << endl;
        ok = false;
    }


    if(ok)
    {
        w->setWellPath(wp);
    }

    else
    {
        cout << endl << "### Error detected in input file! ###" << endl
             <<  "The WELLPATH entry does not have the right format..."
             << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);
    }



}

//-----------------------------------------------------------------------------------------------
// Reads the well WELLPATH TOE
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellPathToe(WellPath *wp, Well *w)
{
    cout << "        toe..." << endl;
    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {




        if(list.size() == 4) // correct number of elements (type, value, max, min)
        {
            bool ok;

            // checking the type
            if(list.at(0).startsWith("I") || list.at(0).startsWith("J") || list.at(0).startsWith("K")) ok = true;
            else ok = false;



            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }



            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));

                if(list.at(0).startsWith("I"))
                {

                    wp->setToeI(v);
                }
                else if(list.at(0).startsWith("J"))
                {

                    wp->setToeJ(v);
                }
                else if(list.at(0).startsWith("K"))
                {

                    wp->setToeK(v);
                }


            }
            else     // error when reading
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The WELLPATH TOE entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }



        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The WELLPATH TOE entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    cout << "end toe" << endl;

    return true;
}

//-----------------------------------------------------------------------------------------------
// Reads the well WELLPATH HEEL
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellPathHeel(WellPath *wp, Well *w)
{
    cout << "        heel..." << endl;

    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.size() == 4) // correct number of elements (type, value, max, min)
        {
            bool ok;

            // checking the type
            if(list.at(0).startsWith("I") || list.at(0).startsWith("J") || list.at(0).startsWith("K")) ok = true;
            else ok = false;



            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }



            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));

                if(list.at(0).startsWith("I")) wp->setHeelI(v);
                else if(list.at(0).startsWith("J")) wp->setHeelJ(v);
                else if(list.at(0).startsWith("K")) wp->setHeelK(v);


            }
            else     // error when reading
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The WELLPATH HEEL entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }



        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The WELLPATH HEEL entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;
}

//-----------------------------------------------------------------------------------------------
// Reads the well WELLPATH OPTIONS
//-----------------------------------------------------------------------------------------------
bool ModelReader::readWellPathOptions(WellPath *wp, Well *w)
{
    cout << "        options..." << endl;

    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.size() == 4) // variable (name, value, max, min)
        {
            bool ok;

            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }



            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));

                v->setName(list.at(0));

                wp->addOptionVariable(v);

            }
            else     // error when reading
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The WELLPATH OPTION entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }

        else if(list.size() == 2) // constant (name, value)
        {
            bool ok= true;

            // checking the number
            double value = list.at(1).toDouble(&ok);



            // all ok
            if(ok)
            {

                wp->addOptionConstant(list.at(0), value);

            }
            else     // error when reading
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The WELLPATH OPTION entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }



        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The WELLPATH OPTION entry does not have the right format..."
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    return true;
}



//-----------------------------------------------------------------------------------------------
// Reads the well OUTLETPIPES section for a well
//-----------------------------------------------------------------------------------------------
bool ModelReader::readPipeConnections(ProductionWell *w)
{
    cout << "        outlet pipes table..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(list.size() == 2 && !isEmpty(list))        // correct number of elements (pipe#, fraction)
        {
            // checking if the line is in the right format

            bool ok1 = true;
            bool ok2 = true;

            int pipe_num = list.at(0).toInt(&ok1);
            double frac = list.at(1).toDouble(&ok2);


            // got all the numbers ok
            if(ok1 && ok2)
            {
                PipeConnection *pipe_con = new PipeConnection();
                shared_ptr<BinaryVariable> var(new BinaryVariable(w));

                var->setValue(frac);
                var->setName("Routing variable for well: "+ w->name() + " to Pipe #" + QString::number(pipe_num));
                var->setIsVariable(false);
                pipe_con->setPipeNumber(pipe_num);
                pipe_con->setVariable(var);


                w->addPipeConnection(pipe_con);
            }
            else     // error when reading numbers in the connections line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The OUTLETPIPES entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }
        else if(list.size() == 3 && !isEmpty(list)) // pipe#, fraction, BIN
        {
            // checking if the third argument is BIN
            if(list.at(2).startsWith("BIN"))
            {
                bool ok1 = true;
                bool ok2 = true;

                int pipe_num = list.at(0).toInt(&ok1);
                double frac = list.at(1).toDouble(&ok2);


                // got all the numbers ok
                if(ok1 && ok2)
                {
                    PipeConnection *pipe_con = new PipeConnection();
                    shared_ptr<BinaryVariable> var(new BinaryVariable(w));

                    var->setValue(frac);
                    var->setName("Routing variable for well: "+ w->name() + " to Pipe #" + QString::number(pipe_num));
                    var->setIsVariable(true);
                    pipe_con->setPipeNumber(pipe_num);
                    pipe_con->setVariable(var);


                    w->addPipeConnection(pipe_con);
                }
                else     // error when reading numbers in the connections line
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The OUTLETPIPES entry could not be read..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                    exit(1);


                }

            }

            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The OUTLETPIPES entry does not have the right format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }

        }

        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The OUTLETPIPES entry does not have the right format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    // if the well has more than one connection, they should be considered variables
    if(w->numberOfConnections() > 1)
    {
        for(int i = 0; i < w->numberOfConnections(); ++i)
        {
            w->pipeConnection(i)->variable()->setIsVariable(true);
        }
    }

    // if the well only has one connection, and it is not a variable, the value should be 1.0
    if(w->numberOfConnections() == 1)
    {
        if(!w->pipeConnection(0)->variable()->isVariable()) w->pipeConnection(0)->variable()->setValue(1.0);
    }

    return true;


}

//-----------------------------------------------------------------------------------------------
// Reads the well OUTLETPIPES section for a pipe
//-----------------------------------------------------------------------------------------------
bool ModelReader::readPipeConnections(MidPipe *p)
{
    cout << "        outlet pipes table..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(list.size() == 2 && !isEmpty(list))        // correct number of elements (pipe#, fraction)
        {
            // checking if the line is in the right format

            bool ok1 = true;
            bool ok2 = true;

            int pipe_num = list.at(0).toInt(&ok1);
            double frac = list.at(1).toDouble(&ok2);


            // got all the numbers ok
            if(ok1  && ok2)
            {

                PipeConnection *pipe_con = new PipeConnection();
                shared_ptr<BinaryVariable> var(new BinaryVariable(p));

                var->setValue(frac);
                var->setName("Routing variable for Pipe #"+ QString::number(p->number()) + " to Pipe #" + QString::number(pipe_num));
                var->setIsVariable(false);
                pipe_con->setPipeNumber(pipe_num);
                pipe_con->setVariable(var);


                p->addOutletConnection(pipe_con);


            }
            else     // error when reading numbers in the connections line
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The OUTLETPIPES entry could not be read..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }
        }
        else if(list.size() == 3 && !isEmpty(list)) // (pipe#, fraction, BIN)
        {
            // checking that the thrid item is BIN
            if(list.at(2).startsWith("BIN"))
            {
                bool ok1 = true;
                bool ok2 = true;

                int pipe_num = list.at(0).toInt(&ok1);
                double frac = list.at(1).toDouble(&ok2);


                // got all the numbers ok
                if(ok1  && ok2)
                {

                    PipeConnection *pipe_con = new PipeConnection();
                    shared_ptr<BinaryVariable> var(new BinaryVariable(p));

                    var->setValue(frac);
                    var->setName("Routing variable for Pipe #"+ QString::number(p->number()) + " to Pipe #" + QString::number(pipe_num));
                    var->setIsVariable(true);
                    pipe_con->setPipeNumber(pipe_num);
                    pipe_con->setVariable(var);


                    p->addOutletConnection(pipe_con);


                }
                else     // error when reading numbers in the connections line
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The OUTLETPIPES entry could not be read..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                    exit(1);


                }

            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The OUTLETPIPES entry does not have the right format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     <<  "The OUTLETPIPES entry does not have the right format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);
            }
        }





        list = processLine(m_driver_file.readLine());

    }

    // if the pipe has more than one connection, they should be considered variables
    if(p->numberOfOutletConnections() > 1)
    {
        for(int i = 0; i < p->numberOfOutletConnections(); ++i)
        {
            p->outletConnection(i)->variable()->setIsVariable(true);
        }
    }

    // if the well only has one connection, and it is not a variable, the value should be 1.0
    if(p->numberOfOutletConnections() == 1)
    {
        if(!p->outletConnection(0)->variable()->isVariable()) p->outletConnection(0)->variable()->setValue(1.0);
    }


    return true;


}


//-----------------------------------------------------------------------------------------------
// Reads an objective definition
//-----------------------------------------------------------------------------------------------
Objective* ModelReader::readObjective()
{
    cout << "Reading objective definition..." << endl;

    Objective *o = 0;

    double l_dcf = 0.0;
    double l_p_oil = 0.0;
    double l_p_gas = 0.0;
    double l_p_wat = 0.0;

    bool ok = true;

    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.at(0).startsWith("TYPE"))              // getting the type of objective
        {
            if(list.at(1).startsWith("NPV")) o = new NpvObjective();                // this is an NPV type objective
            else if(list.at(1).startsWith("CUMOIL")) o = new CumoilObjective();     // this is an CUMOIL type objective
            else if(list.at(1).startsWith("CUMGAS")) o = new CumgasObjective();     // this is an CUMGAS type objective
            else                                                                    // not recognized
            {
                cout << endl << "### Error detected in input file! ###" << endl
                                << "Objective TYPE: " << list.at(1).toLatin1().constData() << endl
                                << "Is not a recognized objective type." << endl << endl;

                exit(1);


            }
        }
        else if(list.at(0).startsWith("DCF")) l_dcf = list.at(1).toDouble(&ok);            // getting the discount factor
        else if(list.at(0).startsWith("OILPRICE")) l_p_oil = list.at(1).toDouble(&ok);     // getting the oil price
        else if(list.at(0).startsWith("GASPRICE")) l_p_gas = list.at(1).toDouble(&ok);     // getting the gas price
        else if(list.at(0).startsWith("WATERPRICE")) l_p_wat = list.at(1).toDouble(&ok);   // getting the water price

        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(m_driver_file.readLine());

    }

    // checking remaining input

    if(o == 0 || !ok)
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "OBJECTIVE definition is incomplete..." << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);


    }

    // checking if the objective is NPV
    NpvObjective *npv = dynamic_cast<NpvObjective*>(o);
    if(npv != 0)
    {
        npv->setDcf(l_dcf);
        npv->setGasPrice(l_p_gas);
        npv->setOilPrice(l_p_oil);
        npv->setWaterPrice(l_p_wat);
    }





    // everything ok

    return o;
}


//-----------------------------------------------------------------------------------------------
// Reads a PIPE definition
//-----------------------------------------------------------------------------------------------
Pipe* ModelReader::readPipe()
{
    cout << "Reading pipe definition..." << endl;

    Pipe *p = 0;

    int l_number = -1;
    QString l_file = "";

    bool l_mustroute = false;

    bool ok = true;

    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("PIPE")))
    {



        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);        // getting the id number of the pipe
        else if(list.at(0).startsWith("FILE")) l_file = list.at(1);                  // getting the file name
        else if(list.at(0).startsWith("MUSTROUTE")) l_mustroute = true;              // setting that the pipe must be routed upstream
        else if(list.at(0).startsWith("OUTLETPRESSURE"))                             // getting the outlet pressure, this is an end pipe
        {
            if(p != 0)  // outlet pipes have allready been defined
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "PIPE definition is overdefined..." << endl
                     << "Did you speficy both OUTLETPIPES and OUTLETPRESSURE for the pipe?"
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                exit(1);

            }
            else
            {
                EndPipe *p_end = new EndPipe();

                double l_outletpressure = list.at(1).toDouble(&ok);
                if(!ok)
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "OUTLETPRESSURE definition not in correct format" << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                    exit(1);
                }

                p_end->setOutletPressure(l_outletpressure);

                // checking if there is a unit definition for the pressure
                if(list.size() > 2)
                {
                    if(list.at(2).startsWith("BAR"))
                    {
                        cout << "Setting OUTLETPRESSURE uniuts to BAR..." << endl;
                        p_end->setOutletUnit(Stream::METRIC);
                    }
                    else if(list.at(2).startsWith("PSI"))
                    {
                        cout << "Setting OUTLETPRESSURE uniuts to PSI..." << endl;
                        p_end->setOutletUnit(Stream::FIELD);
                    }
                }


                p = p_end;

            }

        }
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("OUTLETPIPE"))  // getting outletpipes, this is a mid pipe
        {
            if(p != 0)  // outlet pipes have allready been defined
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "PIPE definition is overdefined..." << endl
                     << "Did you speficy both OUTLETPIPES and OUTLETPRESSURE for the pipe?"
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                exit(1);

            }
            else
            {
                MidPipe *p_mid = new MidPipe();
                if(l_number > 0) p_mid->setNumber(l_number);
                readPipeConnections(p_mid);
                p = p_mid;


            }

        }


        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(m_driver_file.readLine());

    }

    // checking remaining input

    if(!ok || p == 0)                                             // error with number reading
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "PIPE definition is incomplete..." << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);

    }



    // everything is ok, setting common parameters


    // setting comon parameters

    p->setFileName(m_path + "/" + l_file);
    p->setNumber(l_number);

    if(l_mustroute)
    {
        MidPipe *mp = dynamic_cast<MidPipe*>(p);

        if(mp != 0)
        {
            mp->setMustRoute();

            cout << "setting that the pipe must be routed to an upstream element..." << endl;
        }
        else
        {
            cout << endl << "### Error detected in input file! ###" << endl
            << "PIPE definition is incorrect..." << endl
            << "MUSTROUTE keyword can only be used for pipes that are not top nodes!" << endl;

            exit(1);


        }
    }



    return p;
}


//-----------------------------------------------------------------------------------------------
// Reads a capacity definition
//-----------------------------------------------------------------------------------------------
Capacity* ModelReader::readCapacity()
{
    cout << "Reading capacity definition..." << endl;

    Capacity *s = new Capacity();

    QVector<int> pipe_nums;

    bool consr_added = false;   // set true when a constraint is added to the separator


    QStringList list;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.at(0).startsWith("NAME")) s->setName(list.at(1));                      // getting the name of the separator
        else if(list.at(0).startsWith("PIPE"))                                      // getting the input pipe numbers
        {
            bool pipe_ok = true;

            for(int i = 1; i < list.size(); i++)
            {
                pipe_nums.push_back(list.at(i).toInt(&pipe_ok));
                if(!pipe_ok) break;
            }
            if(!pipe_ok)
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "The CAPACITY input PIPES were not correctly defined..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                exit(1);


            }
        }
        else if(list.at(0).startsWith("WATER"))                     // getting water constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;

                double max = list.at(1).toDouble(&ok_max);


                if(ok_max)    // got the number
                {

                    s->setMaxWater(max);

                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The CAPACITY WATER constraint was not correctly defined..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                    exit(1);


                }

            }
        }
        else if(list.at(0).startsWith("GAS"))                     // getting gas constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;

                double max = list.at(1).toDouble(&ok_max);

                if(ok_max)    // got the numbers ok
                {

                    s->setMaxGas(max);

                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The CAPACITY GAS constraint was not correctly defined..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                    exit(1);


                }

            }
        }
        else if(list.at(0).startsWith("OIL"))                     // getting oil constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;

                double max = list.at(1).toDouble(&ok_max);


                if(ok_max)    // got the numbers ok
                {

                    s->setMaxOil(max);

                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The CAPACITY OIL constraint was not correctly defined..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                    exit(1);


                }

            }
        }
        else if(list.at(0).startsWith("LIQ"))                     // getting liquid constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;

                double max = list.at(1).toDouble(&ok_max);


                if(ok_max)    // got the numbers ok
                {

                    s->setMaxLiquid(max);

                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The CAPACITY LIQUID constraint was not correctly defined..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;


                    exit(1);


                }

            }
        }


        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }




        list = processLine(m_driver_file.readLine());

    }

    // checking remaining input

    if(!consr_added)    // no constraint
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "CAPACITY definition is incomplete..." << endl
        << "No constraint of any kind detected" << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);


    }

    if(pipe_nums.size() == 0)    // no pipes
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "CAPACITY definition is incomplete..." << endl
        << "No input PIPES defined" << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);

    }

    // everything ok


    // adding pipe numbers
    for(int i = 0; i < pipe_nums.size(); i++) s->addFeedPipeNumber(pipe_nums.at(i));


    return s;
}


//-----------------------------------------------------------------------------------------------
// Reads a separator definition
//-----------------------------------------------------------------------------------------------
Pipe* ModelReader::readSeparator()
{

    cout << "Reading separator definition..." << endl;

    Separator *p_sep = new Separator();
    QStringList list;

    bool ok = true;
    int l_number = -1;
    int l_outlet_pipe = -1;
    double l_cost_constant = -1.0;
    double l_cost_fraction = -1.0;
    double l_cost_capacity = -1.0;
    double l_cost_frac_exp = 1.0;
    double l_cost_cap_exp = 1.0;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("SEPARATOR")))
    {



        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);                   // getting the id number of the separator
        else if(list.at(0).startsWith("OUTLETPIPE")) l_outlet_pipe = list.at(1).toInt(&ok);     // getting the outlet pipe number
        else if(list.at(0).startsWith("TYPE"))                                                  // getting the type
        {
            if(list.at(1).startsWith("WATER")) p_sep->setType(Separator::WATER);
            else if(list.at(1).startsWith("GAS")) p_sep->setType(Separator::GAS);
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "SEPARATOR TYPE not recognized..." << endl
                     << "The TYPE can either be WATER or GAS" << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);
            }
        }
        else if(list.at(0).startsWith("COST"))                                                  // getting the cost
        {
            if(list.size() == 4)  // the right number of numbers
            {
                bool ok1, ok2, ok3 = true;
                l_cost_constant = list.at(1).toDouble(&ok1);
                l_cost_fraction = list.at(2).toDouble(&ok2);
                l_cost_capacity = list.at(3).toDouble(&ok3);

                if(!ok1 || !ok2 || !ok3) ok = false;

            }
            else if(list.size() == 7) // including exponents
            {
                if(list.at(4).startsWith("EXP"))
                {
                    bool ok1, ok2, ok3, ok4, ok5 = true;
                    l_cost_constant = list.at(1).toDouble(&ok1);
                    l_cost_fraction = list.at(2).toDouble(&ok2);
                    l_cost_capacity = list.at(3).toDouble(&ok3);
                    l_cost_frac_exp = list.at(5).toDouble(&ok4);
                    l_cost_cap_exp = list.at(6).toDouble(&ok5);

                    if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5) ok = false;

                }
                else ok = false;
            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "COST for SEPARATOR not in correct format..." << endl
                     << "The COST keyword must be followed by three numbers: constant term, fraction multiplier, capacity multiplier" << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }
        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            shared_ptr<IntVariable> var_install = shared_ptr<IntVariable>(new IntVariable(p_sep));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);

                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }

            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "INSTALLTIME for SEPARATOR not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }

            p_sep->setInstallTime(var_install);

        }

        else if(list.at(0).startsWith("REMOVE"))                                           // getting the remove fraction
        {

            shared_ptr<RealVariable> var_fraction = shared_ptr<RealVariable>(new RealVariable(p_sep));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_fraction->setValue(value);
                var_fraction->setMax(value);
                var_fraction->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_fraction->setValue(value);
                var_fraction->setMax(max);
                var_fraction->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "REMOVE keyword for SEPARATOR not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);

            }

            p_sep->setRemoveFraction(var_fraction);

        } // REMOVE kwrd

        else if(list.at(0).startsWith("CAPACITY"))                                           // getting the water removal capacity
        {

            shared_ptr<RealVariable> var_capacity = shared_ptr<RealVariable>(new RealVariable(p_sep));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_capacity->setValue(value);
                var_capacity->setMax(value);
                var_capacity->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_capacity->setValue(value);
                var_capacity->setMax(max);
                var_capacity->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "CAPACITY keyword for SEPARATOR not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);

            }

            p_sep->setRemoveCapacity(var_capacity);

        } // CAPACITY kwrd


        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(m_driver_file.readLine());

    }

    if(!ok)                                             // error with number reading
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "PIPE definition is incomplete..." << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);

    }



    // everything is ok, setting common parameters

    p_sep->setNumber(l_number);


    // the cost
    Cost *c = new Cost();
    c->setConstant(l_cost_constant);
    c->setFractionMultiplier(l_cost_fraction);
    c->setCapacityMultiplier(l_cost_capacity);

    c->setFraction(p_sep->removeFraction()->value());
    c->setCapacity(p_sep->removeCapacity()->value());

    c->setCapacityExponent(l_cost_cap_exp);
    c->setFractionExponent(l_cost_frac_exp);

    p_sep->setCost(c);

    // the outlet connection
    PipeConnection *p_con = new PipeConnection();
    p_con->setPipeNumber(l_outlet_pipe);

    shared_ptr<BinaryVariable> routing_var = shared_ptr<BinaryVariable>(new BinaryVariable(p_sep));
    routing_var->setValue(1.0);
    routing_var->setIsVariable(false);
    routing_var->setName("Dummy variable for separator outlet connection");
    p_con->setVariable(routing_var);

    p_sep->setOutletConnection(p_con);

    // setting the names of the removal variables
    p_sep->installTime()->setName("Installation time variable for separator #" + QString::number(l_number));
    p_sep->removeFraction()->setName("Water removal fraction for separator #" + QString::number(l_number));
    p_sep->removeCapacity()->setName("Water removal capacity for separator #" + QString::number(l_number));



    return p_sep;

}


//-----------------------------------------------------------------------------------------------
// Reads a booster definition
//-----------------------------------------------------------------------------------------------
Pipe* ModelReader::readPressureBooster()
{

    cout << "Reading booster definition..." << endl;

    PressureBooster *p_boost = new PressureBooster();
    QStringList list;

    bool ok = true;
    int l_number = -1;
    int l_outlet_pipe = -1;
    double l_cost_constant = -1.0;
    double l_cost_fraction = -1.0;
    double l_cost_cap_exp = 1.0;
    double l_cost_frac_exp = 1.0;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("BOOSTER")))
    {



        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);                   // getting the id number of the booster
        else if(list.at(0).startsWith("OUTLETPIPE")) l_outlet_pipe = list.at(1).toInt(&ok);     // getting the outlet pipe number
        else if(list.at(0).startsWith("COST"))                                                  // getting the cost
        {
            if(list.size() == 3)  // the right number of numbers
            {
                bool ok1, ok2 = true;
                l_cost_constant = list.at(1).toDouble(&ok1);
                l_cost_fraction = list.at(2).toDouble(&ok2);


                if(!ok1 || !ok2) ok = false;

            }
            else if(list.size() == 6) // including exponents
            {
                if(list.at(3).startsWith("EXP"))
                {
                    bool ok1, ok2, ok3, ok4 = true;
                    l_cost_constant = list.at(1).toDouble(&ok1);
                    l_cost_fraction = list.at(2).toDouble(&ok2);
                    l_cost_frac_exp = list.at(4).toDouble(&ok3);
                    l_cost_cap_exp = list.at(5).toDouble(&ok4);

                    if(!ok1 || !ok2 || !ok3 || !ok4) ok = false;

                }
                else ok = false;
            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "COST for BOOSTER not in correct format..." << endl
                     << "The COST keyword must be followed by two numbers: constant term, multiplier" << endl
                     << "or it can include exponents: cost term, multiplier, EXP, pressure exponent, capacity exponent" << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }
        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            shared_ptr<IntVariable> var_install = shared_ptr<IntVariable>(new IntVariable(p_boost));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);

                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }

            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "INSTALLTIME for BOOSTER not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;


                exit(1);

            }

            p_boost->setInstallTime(var_install);

        }

        else if(list.at(0).startsWith("PRESSUREBOOST"))                                           // getting the boost pressure
        {

            shared_ptr<RealVariable> var_pressure = shared_ptr<RealVariable>(new RealVariable(p_boost));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_pressure->setValue(value);
                var_pressure->setMax(value);
                var_pressure->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_pressure->setValue(value);
                var_pressure->setMax(max);
                var_pressure->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "PRESSUREBOOST keyword for BOOSTER not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);

            }

            p_boost->setPressureVariable(var_pressure);

        } // PRESSUREBOOST kwrd

        else if(list.at(0).startsWith("CAPACITY"))                                           // getting the capacity
        {

            shared_ptr<RealVariable> var_capacity = shared_ptr<RealVariable>(new RealVariable(p_boost));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_capacity->setValue(value);
                var_capacity->setMax(value);
                var_capacity->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_capacity->setValue(value);
                var_capacity->setMax(max);
                var_capacity->setMin(min);


                if(!ok1 || !ok2 || !ok3) ok = false;

            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "CAPACITY keyword for BOOSTER not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);

            }

            p_boost->setCapacityVariable(var_capacity);

        } // CAPACITY kwrd


        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(m_driver_file.readLine());

    }

    if(!ok)                                             // error with number reading
    {
        cout << endl << "### Error detected in input file! ###" << endl
        << "BOOSTER definition is incomplete..." << endl
        << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);

    }



    // everything is ok, setting common parameters

    p_boost->setNumber(l_number);


    // the cost
    Cost *c = new Cost();
    c->setConstant(l_cost_constant);
    c->setFractionMultiplier(l_cost_fraction);
    c->setCapacityMultiplier(0.0);

    c->setFractionExponent(l_cost_frac_exp);
    c->setCapacityExponent(l_cost_cap_exp);

    c->setFraction(p_boost->pressureVariable()->value());
    c->setCapacity(p_boost->capacityVariable()->value());

    c->setLinear(false);

    p_boost->setCost(c);

    // the outlet connection
    PipeConnection *p_con = new PipeConnection();
    p_con->setPipeNumber(l_outlet_pipe);

    shared_ptr<BinaryVariable> routing_var = shared_ptr<BinaryVariable>(new BinaryVariable(p_boost));
    routing_var->setValue(1.0);
    routing_var->setIsVariable(false);
    routing_var->setName("Dummy variable for booster outlet connection");
    p_con->setVariable(routing_var);

    p_boost->setOutletConnection(p_con);

    // setting the names of the removal variables
    p_boost->installTime()->setName("Installation time variable for booster #" + QString::number(l_number));
    p_boost->pressureVariable()->setName("Pressure variable for booster #" + QString::number(l_number));
    p_boost->capacityVariable()->setName("Capacity variable for booster #" + QString::number(l_number));



    return p_boost;

}


//-----------------------------------------------------------------------------------------------
// Reads the optimizer definition
//-----------------------------------------------------------------------------------------------
void ModelReader::readOptimizer(Runner *r)
{
    cout << "Reading optimizer definition..." << endl;
    Optimizer *o = 0;



    QStringList list;

    double l_perturb = 0.0001;
    int l_max_iter = 1;
    int l_max_iter_cont = 100;
    int l_parallel_runs = 1;
    double l_term = 0.0;
    int l_term_start = 5;
    bool l_startingpoint_update = false;

    QList<int> l_eropt_steps;

    bool ok = true;


    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {


        if(list.at(0).startsWith("TYPE"))                           // getting the type
        {
            if(list.at(1).startsWith("BONMIN")) o = new BonminOptimizer(r);
            else if(list.at(1).startsWith("RUNONCE")) o = new RunonceOptimizer(r);
            else if(list.at(1).startsWith("NOMAD")) o = new NomadOptimizer(r);
            else if(list.at(1).startsWith("IPOPT")) o = new IpoptOptimizer(r);
            else if(list.at(1).startsWith("LSH")) o = new LshOptimizer(r);
            else if(list.at(1).startsWith("NOIP")) o = new NomadIpoptOptimizer(r);
            else if(list.at(1).startsWith("EROPT")) o = new EroptOptimizer(r);

        }
        else if(list.at(0).startsWith("ITERATIONS")) l_max_iter = list.at(1).toInt(&ok);    // getting the max number if iterations
        else if(list.at(0).startsWith("CONT_ITER")) l_max_iter_cont = list.at(1).toInt(&ok); // getting the max number if iterations for the contienous solver
        else if(list.at(0).startsWith("PERTURB")) l_perturb = list.at(1).toDouble(&ok);     // getting the perturbation size
        else if(list.at(0).startsWith("STARTINGPOINT_UPDATE")) l_startingpoint_update = true;     // using the starting-point from the best sub-problem
        else if(list.at(0).startsWith("TERMINATION"))                                       // getting the termination options
        {
            l_term = list.at(1).toDouble(&ok);
            if(list.size() == 3)
            {
                l_term_start = list.at(2).toInt(&ok);
            }
        }
        else if(list.at(0).startsWith("STEPS"))                 // list of steps for EROPT only
        {
            bool ok_steps = true;

            for(int i = 1; i < list.size(); ++i)
            {
                int stp = list.at(i).toInt(&ok_steps);
                if(!ok_steps)
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "Could not read STEPS in OPTIMIZER definition" << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;
                    exit(1);

                }

                l_eropt_steps.push_back(stp);
            }
        }

        else if(list.at(0).startsWith("PARALLEL"))                                           // getting the number of parallel runs
        {
            if(list.at(1).startsWith("IDEAL"))              // find the ideal number of parallel runs
            {
                l_parallel_runs = QThread::idealThreadCount();
                cout << "Using IDEAL number of parallel runs = " << l_parallel_runs << endl;
            }
            else
            {
                l_parallel_runs = list.at(1).toInt(&ok);   // user specified number
                cout << "Using user-specified number of parallel runs = " << l_parallel_runs << endl;
            }
            // checking that the number of parallel runs is at least 1
            if(l_parallel_runs < 1) l_parallel_runs = 1;
        }
        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in input file! ###" << endl
                << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }


        list = processLine(m_driver_file.readLine());

    }


    if(!ok || o == 0)
    {
        cout << endl << "### Error detected in input file! ###" << endl
             << "The optimizer was not defined propperly..." << endl;

        exit(1);

    }


    // everything ok, setting to optimizer
    o->setMaxIterations(l_max_iter);
    o->setMaxIterContineous(l_max_iter_cont);
    o->setParallelRuns(l_parallel_runs);
    o->setPerturbationSize(l_perturb);
    o->setStartingpointUpdate(l_startingpoint_update);
    o->setTermination(l_term);
    o->setTerminationStart(l_term_start);

    // EROPT specific input
    if(l_eropt_steps.size() > 0)
    {
        EroptOptimizer *p_eropt = dynamic_cast<EroptOptimizer*>(o);
        if(p_eropt != 0)
        {
            p_eropt->setSteps(l_eropt_steps);
        }
        else
        {
            cout << endl << "### Error detected in input file! ###" << endl
                 << "Keyword: " << "STEPS" << endl
                 << "Is only applicable for the EROPT optimizer" << endl << endl;
            exit(1);

        }
    }

    // setting optimizer to runner
    r->setOptimizer(o);

}



//-----------------------------------------------------------------------------------------------
// Reads the user defined constraints
//-----------------------------------------------------------------------------------------------
void ModelReader::readUserDefinedConstraints(Model *m)
{
    cout << "Reading user defined constraints..." << endl;

    QStringList list;



    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {

        if(!isEmpty(list))
        {

            UserConstraint *uc = new UserConstraint(m);

            uc->setExpression(list.join(" "));

            m->addUserDefinedConstraint(uc);



        }


        list = processLine(m_driver_file.readLine());

    }



}




//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList ModelReader::processLine(const QString& line)
{

    QString temp = line.split("!").at(0);   // removing everything after !
    temp = temp.trimmed();                  // removing trailing and leadnig whitespaces

    QStringList list = temp.split(QRegExp("\\s+"));

    while(list.size() < 2)
    {
        list.push_back(" ");
    }

    return list;
}

//-----------------------------------------------------------------------------------------------
// Checks if the line is empty or not (white spaces)
//-----------------------------------------------------------------------------------------------
bool ModelReader::isEmpty(const QStringList &list)
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
