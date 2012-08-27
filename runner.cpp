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


#include "runner.h"

#include <iostream>
#include <QTextStream>
#include <QThread>
#include <QDir>

#include "launcher.h"
#include "modelreader.h"
#include "model.h"
#include "gprssimulator.h"
#include "stream.h"
#include "pressuredropcalculator.h"
#include "pipe.h"
#include "reservoir.h"
#include "bonminoptimizer.h"
#include "runonceoptimizer.h"
#include "objective.h"
#include "binaryvariable.h"
#include "realvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "casequeue.h"
#include "case.h"

// needed for debug
#include "productionwell.h"
#include "userconstraint.h"
#include "vlpsimulator.h"
#include "vlptable.h"




using std::cout;
using std::endl;

namespace ResOpt
{


Runner::Runner(const QString &driver_file, QObject *parent)
    : QObject(parent),
      p_reader(0),
      p_model(0),
      p_simulator(0),
      p_optimizer(0),
      p_summary(0),
      m_number_of_runs(1),
      m_number_of_res_sim_runs(0),
      p_cases(0)
{
    p_reader = new ModelReader(driver_file);
}

Runner::~Runner()
{
    if(p_model != 0) delete p_model;
    if(p_simulator != 0) delete p_simulator;
    if(p_reader != 0) delete p_reader;
    if(p_optimizer != 0) delete p_optimizer;

    for(int i = 0; i < m_launchers.size(); ++i) delete m_launchers.at(i);



}

//-----------------------------------------------------------------------------------------------
// Reads the driver file, and makes the model ready for launch
//-----------------------------------------------------------------------------------------------
void Runner::initialize()
{
    // reading the driver file
    p_model = p_reader->readDriverFile(this);

    // reading the pipe pressure drop definition files
    p_model->readPipeFiles();



    // validating the model
    if(!p_model->validate())
    {
        cout << "### Model failed to validate ###" << endl;
        exit(1);
    }

    // resolving separator connections
    p_model->resolveCapacityConnections();

    // resolving the pipe routing
    p_model->resolvePipeRouting();


    // initializing the model (setting up constraints and streams)
    p_model->initialize();


    cout << "Initializing the reservoir simulator..." << endl;
    // initializing the reservoir simulator
    if(p_simulator == 0) p_simulator = new VlpSimulator();
    p_simulator->setFolder("output");


    cout << "Initializing the optimizer..." << endl;
    // if the optimizer has not been set yet, setting it to runonce

    if(p_optimizer == 0) p_optimizer = new RunonceOptimizer(this);

    p_optimizer->initialize();

    // setting up the summary file
    setSummaryFile("run_summary.out");
    writeProblemDefToSummary();


    // setting up the launchers
    initializeLaunchers();



    cout << "Done initializing the model..." << endl;


}

//-----------------------------------------------------------------------------------------------
// initializes the launchers used for parallel runs
//-----------------------------------------------------------------------------------------------
void Runner::initializeLaunchers()
{
    // setting up the launchers and asociated threads
    for(int i = 0; i < p_optimizer->parallelRuns(); ++i)
    {
        cout << "initializing launcher #" << i+1 << endl;



        // copying the reservoir description file to the results folder for this launcher
        QString folder_str = "output/" + QString::number(i+1);

        QString res_file_new = folder_str + "/" + p_model->reservoir()->file();

        // checking if the folder exists
        QDir dir(".");
        if(!dir.exists(folder_str)) dir.mkdir(folder_str);          // creating the sub folder if it does not exist
        QFile::remove(res_file_new);                                // deleting old version if exists
        QFile::copy(p_model->reservoir()->file(), res_file_new);    // copies the reservoir file to the sub folder


        // creating a launcher
        Launcher *l = new Launcher();

        // copying the base model to the launcher
        l->setModel(model()->clone());

        // setting up the reservoir simulator
        ReservoirSimulator *r = p_simulator->clone();
        r->setFolder(folder_str);       // setting the folder for the simulator

        l->setReservoirSimulator(r);    // assigning the simulator to the launcher



        // initializing the launcher
        if(!l->initialize())
        {
            // something went wrong with initialization

            cout << endl << "### Runtime Error ###" << endl
                << "Could not initialize the launcher..." << endl << endl;

            exit(1);
        }

        // connecting signals and slots
        connect(l, SIGNAL(finished(Launcher*, Component*)), this, SLOT(onLauncherFinished(Launcher*, Component*)));
        connect(l, SIGNAL(runningReservoirSimulator()), this, SLOT(incrementReservoirSimRuns()));


        // creating the thread where the launcher shouild live
        QThread *t = new QThread();

        // moving the launcher to the thread
        l->moveToThread(t);

        // adding launcher and thread to the vectors
        m_launchers.push_back(l);
        m_launcher_running.push_back(false);
        m_threads.push_back(t);

        // starting the thread
        t->start();

    }
}

//-----------------------------------------------------------------------------------------------
// Main control loop
//-----------------------------------------------------------------------------------------------
void Runner::run()
{
    m_start_time = time(NULL);     // storing the start time of the run

    // checking if the model has been initialized
    if(p_model == 0) initialize();


    // starting the optimizer
    p_optimizer->start();

}


//-----------------------------------------------------------------------------------------------
// Running a set of cases for the optimizer
//-----------------------------------------------------------------------------------------------
void Runner::evaluate(CaseQueue *cases, Component *comp)
{
    // TODO: checking that none of the launchers are working


    // updating the case queue
    p_cases = cases;


    // sending out the first batch of cases to the launchers
    for(int i = 0; i < m_launchers.size(); ++i)
    {
        Case *c = p_cases->next();
        if(c != 0)
        {
            m_launcher_running.replace(i, true);

            // connecting the launcher
            connect(this, SIGNAL(sendCase(Case*, Component*)), m_launchers.at(i), SLOT(evaluate(Case*, Component*)));

            // sending the case to the launcher
            emit sendCase(c, comp);

            // disconnecting the launcher
            disconnect(this, SIGNAL(sendCase(Case*, Component*)), m_launchers.at(i), SLOT(evaluate(Case*, Component*)));

        }
        else break; // no more cases to run

    }

}



//-----------------------------------------------------------------------------------------------
// Initializes the summary file
//-----------------------------------------------------------------------------------------------
void Runner::setSummaryFile(const QString &f)
{
    p_summary = new QFile(p_simulator->folder() + "/" + f);

    if(!p_summary->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to summary file: %s", p_summary->fileName().toAscii().data());

        delete p_summary;
        p_summary = 0;
    }
    else
    {
        // deleting content from previous launches
        p_summary->resize(0);
    }

}

//-----------------------------------------------------------------------------------------------
// Writes the problem definition to the summary file
//-----------------------------------------------------------------------------------------------
void Runner::writeProblemDefToSummary()
{
    if(p_summary != 0)
    {
        QTextStream out(p_summary);

        out << "----------------------------------------------------------------------\n";
        out << "------------------------ ResOpt Summary File -------------------------\n";
        out << "----------------------------------------------------------------------\n\n";

        out << "MODEL DESCRIPTION:" << "\n";
        out << "Number of wells      = " << p_model->numberOfWells() << "\n";
        out << "Number of pipes      = " << p_model->numberOfPipes() << "\n";
        out << "Number of separators = " << p_model->numberOfCapacities() << "\n\n";


        QVector<shared_ptr<BinaryVariable> >  binary_vars = p_model->binaryVariables();
        QVector<shared_ptr<RealVariable> > real_vars = p_model->realVariables();
        QVector<shared_ptr<IntVariable> > int_vars = p_model->integerVariables();
        QVector<shared_ptr<Constraint> > cons = p_model->constraints();


        out << "OPTIMIZATION PROBLEM:" << "\n";
        out << "Number of contineous variables  = " << real_vars.size() << "\n";
        out << "Number of binary variables      = " << binary_vars.size() << "\n";
        out << "Number of constraints           = " << cons.size() << "\n\n";

        out << "CONTINEOUS VARIABLES:" << "\n";
        for(int i = 0; i < real_vars.size(); ++i)
        {
            out << "VAR_C" << i +1 << ": " << real_vars.at(i)->name();
            out << ", bounds: (" << real_vars.at(i)->min() << " < " << real_vars.at(i)->value() << " < " << real_vars.at(i)->max() << ")\n";
        }
        out << "\n";

        out << "BINARY VARIABLES:" << "\n";
        for(int i = 0; i < binary_vars.size(); ++i)
        {
            out << "VAR_B" << i +1 << ": " << binary_vars.at(i)->name();
            out << ", bounds: (" << binary_vars.at(i)->min() << " < " << binary_vars.at(i)->value() << " < " << binary_vars.at(i)->max() << ")\n";
        }

        out << "\n";

        out << "INTEGER VARIABLES:" << "\n";
        for(int i = 0; i < int_vars.size(); ++i)
        {
            out << "VAR_I" << i +1 << ": " << int_vars.at(i)->name();
            out << ", bounds: (" << int_vars.at(i)->min() << " < " << int_vars.at(i)->value() << " < " << int_vars.at(i)->max() << ")\n";
        }

        out << "\n";

        out << "CONSTRAINTS:" << "\n";
        for(int i = 0; i < cons.size(); i++)
        {
            out << "CON" << i +1 << ": " << cons.at(i)->name();
            out << ", bounds: (" << cons.at(i)->min() << " < c < " << cons.at(i)->max() << ")\n";
        }


        out << "\nMODEL EVALUATIONS:" << "\n";
        out << "----------------------------------------------------------------------\n";


        // header

        out << "#\t" << "OBJ\t";


        for(int i = 0; i < real_vars.size(); ++i)
        {
            out << "VAR_C" << i +1 << "\t";
        }

        for(int i = 0; i < binary_vars.size(); ++i)
        {
            out << "VAR_B" << i + 1 << "\t";
        }

        for(int i = 0; i < int_vars.size(); ++i)
        {
            out << "VAR_I" << i + 1 << "\t";
        }


        for(int i = 0; i < cons.size(); ++i)
        {
            out << "CON" << i +1 << "\t";
        }


        out << "\n";


        p_summary->flush();

    }
}

//-----------------------------------------------------------------------------------------------
// Writes the results from the current iteration to the summary file
//-----------------------------------------------------------------------------------------------
void Runner::writeCasesToSummary()
{
    if(p_summary != 0)
    {
        QTextStream out(p_summary);

        // looping through the cases, writing to sumary

        for(int i = 0; i < p_cases->size(); ++i)
        {
            Case *c = p_cases->at(i);

            out << m_number_of_runs << "\t" << c->objectiveValue() << "\t";

            // real variables
            for(int j = 0; j < c->numberOfRealVariables(); ++j)
            {
                out << c->realVariableValue(j) << "\t";
            }

            // binary variables
            for(int j = 0; j < c->numberOfBinaryVariables(); ++j)
            {
                out << c->binaryVariableValue(j) << "\t";
            }

            // integer variables
            for(int j = 0; j < c->numberOfIntegerVariables(); ++j)
            {
                out << c->integerVariableValue(j) << "\t";
            }


            // constraints
            for(int j = 0; j < c->numberOfConstraints(); ++j)
            {
                out << c->constraintValue(j) << "\t";
            }

            out << "\n";

            ++m_number_of_runs;

        }





        p_summary->flush();

    }


}

//-----------------------------------------------------------------------------------------------
// Writes the results of the best case to the summary file
//-----------------------------------------------------------------------------------------------
void Runner::writeBestCaseToSummary(Case *c)
{
    if(p_summary != 0)
    {
        QTextStream out(p_summary);


        out << "\nBEST CASE:\n";


        // header

        out << "#\t" << "OBJ\t";


        for(int i = 0; i < c->numberOfRealVariables(); ++i)
        {
            out << "VAR_C" << i +1 << "\t";
        }

        for(int i = 0; i < c->numberOfBinaryVariables(); ++i)
        {
            out << "VAR_B" << i + 1 << "\t";
        }

        for(int i = 0; i < c->numberOfIntegerVariables(); ++i)
        {
            out << "VAR_I" << i + 1 << "\t";
        }



        out << "\n";


        // data


        out << "xx" << "\t" << c->objectiveValue() << "\t";

        // real variables
        for(int j = 0; j < c->numberOfRealVariables(); ++j)
        {
            out << c->realVariableValue(j) << "\t";
        }


        // binary variables
        for(int j = 0; j < c->numberOfBinaryVariables(); ++j)
        {
            out << c->binaryVariableValue(j) << "\t";
        }

        // integer variables
        for(int j = 0; j < c->numberOfIntegerVariables(); ++j)
        {
            out << c->integerVariableValue(j) << "\t";
        }


        // constraints
        for(int j = 0; j < c->numberOfConstraints(); ++j)
        {
            out << c->constraintValue(j) << "\t";
        }

        out << "\n\n";

        time_t end_time = time(NULL);

        out << "Total execution time: " << difftime(end_time, m_start_time) << " seconds\n";
        out << "Total number of model evaluations: " << m_number_of_runs -1 << "\n";
        out << "Number of reservoir simulator launches: " << m_number_of_res_sim_runs << "\n";



        p_summary->flush();

    }

}


//-----------------------------------------------------------------------------------------------
// This function is called whenever a Launcher emits finished
//-----------------------------------------------------------------------------------------------
void Runner::onLauncherFinished(Launcher *l, Component *comp)
{

    // check if there are more cases to run
    Case *c = p_cases->next();

    if(c != 0)      // found a new case, sending it to the launcher
    {
        // connecting the launcher
        connect(this, SIGNAL(sendCase(Case*, Component*)), l, SLOT(evaluate(Case*, Component*)));

        // sending the case to the launcher
        emit sendCase(c, comp);

        // disconnecting the launcher
        disconnect(this, SIGNAL(sendCase(Case*, Component*)), l, SLOT(evaluate(Case*, Component*)));
    }
    else    // no more cases, checking if all launchers have finished
    {
        int place = 0;

        // finding which launcher this is
        for(int i = 0; i < m_launchers.size(); ++i)
        {
            if(l == m_launchers.at(i))
            {
                place = i;
                break;
            }
        }

        // setting the status of this launcher to not running
        m_launcher_running.replace(place, false);

        // checking if any launchers are still running
        bool all_finished = true;
        for(int i = 0; i < m_launcher_running.size(); ++i)
        {
            if(m_launcher_running.at(i))    // found a launcher that is still running
            {
                all_finished = false;
                break;
            }
        }

        // if all launchers are finished, letting the optimizer know
        if(all_finished)
        {
            writeCasesToSummary();
            emit casesFinished();
        }

    }


}

} // namespace ResOpt
