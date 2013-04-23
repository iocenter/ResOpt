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
#include <QEventLoop>

#include "launcher.h"
#include "modelreader.h"
#include "model.h"
#include "gprssimulator.h"
#include "stream.h"
#include "pressuredropcalculator.h"
#include "pipe.h"
#include "endpipe.h"
#include "midpipe.h"
#include "separator.h"
#include "pressurebooster.h"
#include "pipeconnection.h"
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
#include "cost.h"

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
      p_debug(0),
      m_number_of_runs(1),
      m_number_of_res_sim_runs(0),
      p_cases(0),
      p_last_run_launcher(0),
      m_paused(false)
{
    p_reader = new ModelReader(driver_file);
}

Runner::~Runner()
{
    if(p_model != 0) delete p_model;
    if(p_simulator != 0) delete p_simulator;
    if(p_reader != 0) delete p_reader;
    if(p_optimizer != 0) delete p_optimizer;
    if(p_summary != 0) delete p_summary;
    if(p_debug != 0) delete p_debug;

    for(int i = 0; i < m_launchers.size(); ++i) delete m_launchers.at(i);



}

//-----------------------------------------------------------------------------------------------
// Reads the driver file, and makes the model ready for launch
//-----------------------------------------------------------------------------------------------
void Runner::initialize()
{
    // checking if the output folder exists, creating if not.
    QDir dir(p_reader->driverFilePath());
    if(!dir.exists("output")) dir.mkdir("output");


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
    p_simulator->setFolder(p_reader->driverFilePath() + "/output");


    cout << "Initializing the optimizer..." << endl;
    // if the optimizer has not been set yet, setting it to runonce

    if(p_optimizer == 0) p_optimizer = new RunonceOptimizer(this);

    p_optimizer->initialize();

    // setting up the summary file
    setSummaryFile("run_summary.out");



    // setting up the launchers
    initializeLaunchers();



    cout << "Done initializing the model..." << endl;


}

//-----------------------------------------------------------------------------------------------
// initializes the launchers used for parallel runs
//-----------------------------------------------------------------------------------------------
void Runner::initializeLaunchers()
{
    //test code
    for(int i = 0; i < m_launchers.size(); ++i) delete m_launchers.at(i);
    m_launchers.resize(0);

    for(int i = 0; i < m_threads.size(); ++i)
    {
        //m_threads.at(i)->terminate();
        delete m_threads.at(i);
    }
    m_threads.resize(0);

    m_launcher_running.resize(0);



    // setting up the launchers and asociated threads
    for(int i = 0; i < p_optimizer->parallelRuns(); ++i)
    {
        cout << "initializing launcher #" << i+1 << endl;



        // copying the reservoir description file to the results folder for this launcher
        QString folder_str =  QString::number(i+1);

        QString res_file_new = p_simulator->folder() + "/" + folder_str + "/" + p_model->reservoir()->file();

        // checking if the folder exists
        QDir dir(p_simulator->folder());
        if(!dir.exists(folder_str)) dir.mkdir(folder_str);          // creating the sub folder if it does not exist
        QFile::remove(res_file_new);                                // deleting old version if exists
        QFile::copy(p_model->driverPath() + "/" + p_model->reservoir()->file(), res_file_new);    // copies the reservoir file to the sub folder


        // creating a launcher
        Launcher *l = new Launcher();

        // copying the base model to the launcher
        l->setModel(model()->clone());

        // setting up the reservoir simulator
        ReservoirSimulator *r = p_simulator->clone();
        r->setFolder(p_simulator->folder() + "/" + folder_str);       // setting the folder for the simulator

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
        connect(l, SIGNAL(finished(Launcher*, Component*, Case*)), this, SLOT(onLauncherFinished(Launcher*, Component*, Case*)));
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
    writeProblemDefToSummary();

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
// Initializes the debug file
//-----------------------------------------------------------------------------------------------
void Runner::setDebugFile(const QString &f)
{
    p_debug = new QFile(p_simulator->folder() + "/" + f);

    if(!p_debug->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to debug file: %s", p_debug->fileName().toAscii().data());

        delete p_debug;
        p_debug = 0;
    }
    else
    {
        // deleting content from previous launches
        p_debug->resize(0);
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

        out << "#\t" << "FEAS\t" << "OBJ\t";


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

            bool feas = isFeasible(c);

            // run #
            out << m_number_of_runs << "\t";

            // is feasible?
            if(feas) out << "yes\t";
            else out << "no\t";

            // objective value
            out << c->objectiveValue() << "\t";

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

            // if the solution is feasible:
            if(feas)
            {
                for(int j = 0; j < c->numberOfConstraints(); ++j)
                {
                    out << c->constraintValue(j) << "\t";
                }
            }

            // if the solution is not feasible
            else
            {
                for(int j = 0; j < c->numberOfConstraints(); ++j)
                {
                    double max = model()->constraints().at(j)->max();
                    double min = model()->constraints().at(j)->min();

                    if(c->constraintValue(j) > max || c->constraintValue(j) < min)
                    {
                        out << c->constraintValue(j) << "*\t";
                    }
                    else out << c->constraintValue(j) << "\t";
                }
            }

            out << "\n";

            ++m_number_of_runs;

        }





        p_summary->flush();

    }


}

//-----------------------------------------------------------------------------------------------
// Checks if the case is feasible
//-----------------------------------------------------------------------------------------------
bool Runner::isFeasible(Case *c)
{
    bool ok = true;
    // first checking that the number of constraints match
    if(c->numberOfConstraints() != model()->constraints().size()) return false;
    else
    {
        for(int i = 0; i < c->numberOfConstraints(); ++i)
        {
            double max = model()->constraints().at(i)->max();
            double min = model()->constraints().at(i)->min();

            if(c->constraintValue(i) > max || c->constraintValue(i) < min)
            {
                ok = false;
                break;
            }
        }
    }

    return ok;
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

        out << "#\t\t" << "OBJ\t";


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


        out << "xx" << "\t\t" << c->objectiveValue() << "\t";

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
void Runner::onLauncherFinished(Launcher *l, Component *comp, Case *finished_case)
{


    // this is connected to the GUI...
    emit newCaseFinished(finished_case);

    //update the last run launcher pointer
    p_last_run_launcher = l;

    // printing debug info if enabled
    if(p_debug != 0) printDebug(l);

    // check if the optimization should be paused
    if(m_paused)
    {
        cout << "pausing optimization..." << endl;

        QEventLoop pause_loop;

        connect(this, SIGNAL(resumePaused()), &pause_loop, SLOT(quit()));

        pause_loop.exec();

        cout << "pause ended...." << endl;

    }

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

//-----------------------------------------------------------------------------------------------
// Transfers the current variable values and streams from launcher to runner
//-----------------------------------------------------------------------------------------------
void Runner::transferModelStateFromLauncher()
{
    if(p_last_run_launcher != 0)
    {
        *p_model = *p_last_run_launcher->model();
    }
}

//-----------------------------------------------------------------------------------------------
// Prints debug information on the current model state
//-----------------------------------------------------------------------------------------------
void Runner::printDebug(Launcher *l)
{
    QTextStream out(p_debug);

    Model *m = l->model();

    out << "CURRENT MODEL STATE:" << "\n";
    out << "###############################\n\n";


    // printing the rate matrix
    out << "------------------------------\n";
    out << "RATE MATRIX:\n";
    out << "------------------------------\n\n";

    // looping through the time steps
    for(int i = 0; i < m->numberOfMasterScheduleTimes(); ++i)
    {
        // printing the header
        out << "time = " << m->masterSchedule().at(i) << " days\n";

        out << "COMP\t" << "QO\t" << "QG\t" << "QW\t" << "P\n";
        out << "----\t" << "--\t" << "--\t" << "--\t" << "-\n";

        // the wells
        for(int j = 0; j < m->numberOfWells(); ++j)
        {
            Well *w = m->well(j);

            out << w->name() << "\t" << w->stream(i)->oilRate(true) << "\t" << w->stream(i)->gasRate(true) << "\t" << w->stream(i)->waterRate(true) << "\t" << w->stream(i)->pressure(true) << "\n";
        }

        // the pipes
        for(int j = 0; j < m->numberOfPipes(); ++j)
        {
            Pipe *p = m->pipe(j);

            out << "#" << p->number() << "\t" << p->stream(i)->oilRate(true) << "\t" << p->stream(i)->gasRate(true) << "\t" << p->stream(i)->waterRate(true) << "\t" << p->stream(i)->pressure(true) << "\n";
        }

        out << "\n";
    }


    // looping through the wells
    for(int i = 0; i < m->numberOfWells(); ++i)
    {
        Well *w = m->well(i);

        out << "------------------------------\n";
        out << "WELL: " << w->name() << "\n";
        out << "------------------------------\n\n";

        // printing controls
        out << "------Controls-------\n";
        for(int j = 0; j < w->numberOfControls(); ++j)
        {
            out << "time       = " << w->control(j)->endTime() << "\n";
            out << "value      = " << w->control(j)->controlVar()->value() << "\n";

            out << "type       = ";
            if(w->control(j)->type() == WellControl::BHP) out << "BHP";
            else if(w->control(j)->type() == WellControl::QGAS) out << "GAS";
            else if(w->control(j)->type() == WellControl::QOIL) out << "OIL";
            else if(w->control(j)->type() == WellControl::QWAT) out << "WATER";

            out << "\n\n";
        }

        // checking if this is a production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);

        if(prod_well != 0)
        {
            if(prod_well->hasGasLift())
            {
                out << "------Gas Lift-------\n";
                for(int j = 0; j < prod_well->numberOfGasLiftControls(); ++j)
                {
                    out << "time       = " << prod_well->gasLiftControl(j)->endTime() << "\n";
                    out << "value      = " << prod_well->gasLiftControl(j)->controlVar()->value() << "\n\n";

                }
            }

            out << "------Routing-------\n";
            for(int j = 0; j < prod_well->numberOfPipeConnections(); ++j)
            {
                out << "destination pipe = " << prod_well->pipeConnection(j)->pipeNumber() << "\n";
                out << "value            = " << prod_well->pipeConnection(j)->variable()->value() << "\n\n";
            }

        }

        // looping through the streams
        out << "------Rate Data-------\n";
        for(int j = 0; j < w->numberOfStreams(); ++j)
        {
            Stream *s = w->stream(j);

            QString ql_unit = (s->inputUnits() == Stream::FIELD) ? " (bbl/d)" : " (m^3/d)";
            QString qg_unit = (s->inputUnits() == Stream::FIELD) ? " (mcf/d)" : " (m^3/d)";
            QString p_unit = (s->inputUnits() == Stream::FIELD) ? " (psia)" : " (bara)";

            out << "time       = " << s->time() << " (days)" << "\n";
            out << "gas rate   = " << s->gasRate(true) << qg_unit.toAscii().data() << "\n";
            out << "oil rate   = " << s->oilRate(true) << ql_unit.toAscii().data() <<  "\n";
            out << "water rate = " << s->waterRate(true) << ql_unit.toAscii().data() << "\n";
            out << "pressure   = " << s->pressure(true) << p_unit.toAscii().data() << "\n\n";

        }
    }

    // looping through the pipes
    for(int i = 0; i < m->numberOfPipes(); ++i)
    {
        Pipe *p = m->pipe(i);

        // checking what type of pipe
        EndPipe *end_pipe = dynamic_cast<EndPipe*>(p);
        MidPipe *mid_pipe = dynamic_cast<MidPipe*>(p);
        Separator *sep = dynamic_cast<Separator*>(p);
        PressureBooster *boost = dynamic_cast<PressureBooster*>(p);

        out << "------------------------------\n";
        if(end_pipe != 0)
        {
            out << "END PIPE: " << p->number() << "\n";
            out << "------------------------------\n\n";

            QString p_unit_out = (end_pipe->outletUnit() == Stream::METRIC) ? " (bara)" : " (psia)";

            out << "Minimum outlet pressure = " << end_pipe->outletPressure() << p_unit_out.toAscii().data() << "\n\n";
        }

        else if(mid_pipe != 0)
        {
            out << "MID PIPE: " << p->number() << "\n";
            out << "------------------------------\n\n";

            out << "------Routing-------\n";
            for(int j = 0; j < mid_pipe->numberOfOutletConnections(); ++j)
            {
                out << "destination pipe = " << mid_pipe->outletConnection(j)->pipeNumber() << "\n";
                out << "value            = " << mid_pipe->outletConnection(j)->variable()->value() << "\n\n";
            }
        }
        else if(sep != 0)
        {
            out << "SEPARATOR: " << p->number() << "\n";
            out << "------------------------------\n\n";

            out << "type              = ";
            if(sep->type() == Separator::WATER) out << "WATER\n";
            else if(sep->type() == Separator::GAS) out << "GAS\n";
            out << "Downstream pipe   = " << sep->outletConnection()->pipeNumber() << "\n";
            out << "Installation time = " << sep->installTime()->value() << "\n";
            out << "Remove fraction   = " << sep->removeFraction()->value() << "\n";
            out << "Max capacity      = " << sep->removeCapacity()->value() << "\n";
            out << "Current cost      = " << sep->cost()->value() << "\n\n";

        }

        else if(boost != 0)
        {
            out << "BOOSTER: " << p->number() << "\n";
            out << "------------------------------\n\n";


            out << "Downstream pipe   = " << boost->outletConnection()->pipeNumber() << "\n";
            out << "Installation time = " << model()->masterSchedule().at(boost->installTime()->value()) << "\n";
            out << "Boost Pressure    = " << boost->pressureVariable()->value() << "\n";
            out << "Max capacity      = " << boost->capacityVariable()->value() << "\n";
            out << "Current cost      = " << boost->cost()->value() << "\n\n";



        }



        out << "------Rate Data-------\n";
        // looping through the streams
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            Stream *s = p->stream(j);

            QString ql_unit = (s->inputUnits() == Stream::FIELD) ? " (bbl/d)" : " (m^3/d)";
            QString qg_unit = (s->inputUnits() == Stream::FIELD) ? " (mcf/d)" : " (m^3/d)";
            QString p_unit = (s->inputUnits() == Stream::FIELD) ? " (psia)" : " (bara)";

            out << "time            = " << s->time() << " (days)" << "\n";
            out << "gas rate        = " << s->gasRate(true) << qg_unit.toAscii().data() << "\n";
            out << "oil rate        = " << s->oilRate(true) << ql_unit.toAscii().data() <<  "\n";
            out << "water rate      = " << s->waterRate(true) << ql_unit.toAscii().data() << "\n";
            out << "inlet pressure  = " << s->pressure(true) << p_unit.toAscii().data() << "\n\n";



        }
    }







}

//-----------------------------------------------------------------------------------------------
// pauses / restarts the optimization
//-----------------------------------------------------------------------------------------------
void Runner::setPaused(bool paused)
{
    if(paused) cout << "runner: starting pause" << endl;
    else cout << "runner: ending pause" << endl;

    m_paused = paused;

    if(!paused) emit resumePaused();
}

} // namespace ResOpt
