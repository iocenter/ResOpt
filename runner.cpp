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

#include "launcher.h"
#include "modelreader.h"
#include "model.h"
#include "gprssimulator.h"
#include "stream.h"
#include "pressuredropcalculator.h"
#include "pipe.h"
#include "bonminoptimizer.h"
#include "runonceoptimizer.h"
#include "objective.h"
#include "binaryvariable.h"
#include "realvariable.h"
#include "constraint.h"
#include "casequeue.h"
#include "case.h"

// needed for debug
#include "productionwell.h"



using std::cout;
using std::endl;

namespace ResOpt
{



Runner::Runner(const QString &driver_file, QObject *parent)
    : QObject(parent),
      p_reader(0),
      p_model(0),
      p_optimizer(0),
      p_simulator(0),
      p_summary(0),
      m_number_of_runs(0),
      m_up_to_date(false)
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
    // reading the driver file and initializing the model
    p_model = p_reader->readDriverFile(this);

    // reading the pipe pressure drop definition files
    p_model->readPipeFiles();

    // resolving separator connections
    p_model->resolveSeparatorConnections();

    // resolving the pipe routing (this must be done before each launch of the model)
    p_model->resolvePipeRouting();

    cout << "Initializing the reservoir simulator..." << endl;
    // initializing the reservoir simulator
    p_simulator = new GprsSimulator();
    p_simulator->setFolder("output");


    cout << "Initializing the optimizer..." << endl;
    // if the optimizer has not been set yet, setting it to runonce

    if(p_optimizer == 0) p_optimizer = new RunonceOptimizer(this);

    p_optimizer->initialize();

    // setting up the summary file
    setSummaryFile("run_summary.out");
    writeProblemDefToSummary();

    /////// new multi thread ////////

    // setting up the launchers and asociated threads
    for(int i = 0; i < p_optimizer->parallelRuns(); ++i)
    {
        // creating a launcher
        Launcher *l = new Launcher();

        // copying the base model to the launcher
        l->setModel(new Model(*model()));

        // setting up the reservoir simulator
        ReservoirSimulator *r = new GprsSimulator();
        r->setFolder("output/" + QString::number(i));

        l->setReservoirSimulator(r);

        // initializing the launcher
        if(!l->initialize())
        {
            // something went wrong with initialization

            cout << endl << "### Runtime Error ###" << endl
                << "Could not initialize the launcher..." << endl << endl;

            exit(1);
        }

        // connecting signals and slots
        connect(l, SIGNAL(finished(Launcher*)), this, SLOT(onLauncherFinished(Launcher*)));


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


    cout << "Done initializing the model..." << endl;


}


//-----------------------------------------------------------------------------------------------
// Main control loop
//-----------------------------------------------------------------------------------------------
void Runner::run()
{

    // checking if the model has been initialized
    if(p_model == 0) initialize();


    // starting the optimizer
    p_optimizer->start();


    ///// debug code  //////



    /*

    cout << "*****  DEBUG  *****" << endl;



    // running the model once

    evaluate();



    // testing arithmetic manipulation of streams
    Stream str;
    str.setGasRate(10000);
    str.setOilRate(1000);
    str.setWaterRate(1.0);
    str.setTime(10);
    str.setPressure(14.7);

    Stream result = str*0.23;

    cout << "--- Streams multiplication ---" << endl;
    result.printToCout();



    // testing the flow fractions code

    ProductionWell *prod1 = dynamic_cast<ProductionWell*>(p_model->well(0));
    if(prod1 != 0)
    {
        cout << "calculating the flow fraction from prod1 to Pipe 1..." << endl;

        double frac = prod1->flowFraction(p_model->pipe(0));

        cout <<"fraction = " << frac << endl;


        cout << "calculating the flow fraction from prod1 to Pipe 2..." << endl;

        frac = prod1->flowFraction(p_model->pipe(1));

        cout <<"fraction = " << frac << endl;

        cout << "calculating the flow fraction from prod1 to Pipe 3..." << endl;

        frac = prod1->flowFraction(p_model->pipe(2));

        cout <<"fraction = " << frac << endl;



    }



    // making a stream
    Stream *str = new Stream();
    str->setGasRate(10000);
    str->setOilRate(1000);
    str->setWaterRate(0.0);
    str->setTime(10);

    // getting the calculator for the first pipe
    PressureDropCalculator *calc = p_model->pipe(1)->calculator();

    // calculating the pressure drop for the stream
    double dp = calc->pressureDrop(str, 57.9);

    cout << "Calculated pressure drop = " << dp << endl;


    // testing the reservoir simulator input file generation
    p_simulator->setFolder("output");
    p_simulator->generateInputFiles(p_model);
    p_simulator->launchSimulator();
    p_simulator->readOutput(p_model);


    // calculating the pipe inlet pressures
    p_model->calculatePipePressures();


    // printing the calculated pressures and rates for PIPE 1

    Pipe *p = p_model->pipe(0);

    cout << "Calculated pressures for PIPE: " << p->number() << endl;
    for(int i = 0; i < p->numberOfStreams(); i++)
    {
        cout << p->stream(i)->time() << "    " << p->stream(i)->pressure() << endl;
    }


    // updating the separator constraints
    p_model->updateSeparatorConstraints();

    */
}


//-----------------------------------------------------------------------------------------------
// Running the model, calculating results
//-----------------------------------------------------------------------------------------------
bool Runner::evaluate()
{
    bool ok = true;
    m_number_of_runs += 1;

    cout << endl << "***** Starting new iteration *****" << endl << endl;

    // running the reservoir simulator
    p_simulator->generateInputFiles(p_model);   // generating input based on the current Model
    p_simulator->launchSimulator();             // running the simulator
    p_simulator->readOutput(p_model);           // reading output from the simulator run, and setting to Model


    // calculating pressures in the Pipe network
    p_model->calculatePipePressures();

    // updating the constraints (this must be done after the pressure calc)
    p_model->updateConstraints();

    // updating the objective
    p_model->updateObjectiveValue();

    // changing the status to up to date
    m_up_to_date = true;

    // writing to summary file
    writeIterationToSummary();

    return ok;
}

//-----------------------------------------------------------------------------------------------
// Running a set of cases for the optimizer
//-----------------------------------------------------------------------------------------------
void Runner::evaluate(CaseQueue *cases)
{
    // updating the case queue
    p_cases = cases;

    // checking that none of the launchers are working

    // sending out the first batch of cases to the launchers

    for(int i = 0; i < m_launchers.size(); ++i)
    {
        Case *c = p_cases->next();
        if(c != 0)
        {
            m_launcher_running.replace(i, true);
            m_launchers.at(i)->evaluate(c);
        }
        else break;

    }


    //emit casesFinished();

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
        out << "Number of separators = " << p_model->numberOfSeparators() << "\n\n";


        QVector<shared_ptr<BinaryVariable> >  binary_vars = p_model->binaryVariables();
        QVector<shared_ptr<RealVariable> > real_vars = p_model->realVariables();
        QVector<shared_ptr<Constraint> > cons = p_model->constraints();


        out << "OPTIMIZATION PROBLEM:" << "\n";
        out << "Number of contineous variables  = " << real_vars.size() << "\n";
        out << "Number of binary variables      = " << binary_vars.size() << "\n";
        out << "Number of constraints           = " << cons.size() << "\n\n";

        out << "CONTINEOUS VARIABLES:" << "\n";
        for(int i = 0; i < real_vars.size(); i++)
        {
            out << "VAR_C" << i +1 << ": " << real_vars.at(i)->name();
            out << ", bounds: (" << real_vars.at(i)->min() << " < " << real_vars.at(i)->value() << " < " << real_vars.at(i)->max() << ")\n";
        }
        out << "\n";

        out << "BINARY VARIABLES:" << "\n";
        for(int i = 0; i < binary_vars.size(); i++)
        {
            out << "VAR_B" << i +1 << ": " << binary_vars.at(i)->name();
            out << ", bounds: (" << binary_vars.at(i)->min() << " < " << binary_vars.at(i)->value() << " < " << binary_vars.at(i)->max() << ")\n";
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


        for(int i = 0; i < real_vars.size(); i++)
        {
            out << "VAR_C" << i +1 << "\t";
        }

        for(int i = 0; i < binary_vars.size(); i++)
        {
            out << "VAR_B" << i + 1 << "\t";
        }

        for(int i = 0; i < cons.size(); i++)
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
void Runner::writeIterationToSummary()
{
    if(p_summary != 0)
    {
        QTextStream out(p_summary);

        out << m_number_of_runs << "\t" << p_model->objective()->value() << "\t";

        QVector<shared_ptr<BinaryVariable> >  binary_vars = p_model->binaryVariables();
        QVector<shared_ptr<RealVariable> > real_vars = p_model->realVariables();
        QVector<shared_ptr<Constraint> > cons = p_model->constraints();

        for(int i = 0; i < real_vars.size(); i++)
        {
            out << real_vars.at(i)->value() << "\t";
        }

        for(int i = 0; i < binary_vars.size(); i++)
        {
            out << binary_vars.at(i)->value() << "\t";
        }

        for(int i = 0; i < cons.size(); i++)
        {
            out << cons.at(i)->value() << "\t";
        }


        out << "\n";

        p_summary->flush();

    }

}

//-----------------------------------------------------------------------------------------------
// This function is called whenever a Launcher emits finished
//-----------------------------------------------------------------------------------------------
void Runner::onLauncherFinished(Launcher *l)
{
    // check if there are more cases to run
    Case *c = p_cases->next();

    if(c != 0)
    {
        // sending the case to the launcher
        l->evaluate(c);
    }
    else
    {
        // setting the status of this launcher to not running
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

        m_launcher_running.replace(place, false);

        // checking if any launchers are still running
        bool all_finished = true;
        for(int i = 0; i < m_launcher_running.size(); ++i)
        {
            if(m_launcher_running.at(i))
            {
                all_finished = false;
                break;
            }
        }

        // if all launchers are finished, letting the optimizer know
        if(all_finished) emit casesFinished();
    }


}

} // namespace ResOpt
