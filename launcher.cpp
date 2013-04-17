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


#include "launcher.h"

#include <QVector>

#include "model.h"
#include "adjointscoupledmodel.h"
#include "reservoirsimulator.h"
#include "case.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "component.h"
#include "well.h"
#include "pipe.h"
#include "separator.h"

#include "pressuredropcalculator.h"




// for debug

#include "stream.h"
#include <iostream>

#include <QThread>


#include <tr1/memory>

using std::tr1::shared_ptr;
using std::cout;
using std::endl;

namespace ResOpt
{

Launcher::Launcher(QObject *parent)
    : QObject(parent),
      p_model(0),
      p_simulator(0),
      m_number_of_runs(0)
{
}

Launcher::~Launcher()
{
    if(p_model != 0) delete p_model;
    if(p_simulator != 0) delete p_simulator;
}

//-----------------------------------------------------------------------------------------------
// initializing
//-----------------------------------------------------------------------------------------------
bool Launcher::initialize()
{

    if(p_model != 0 && p_simulator != 0)
    {
        // resolving separator connections
        p_model->resolveCapacityConnections();

        // resolving the pipe routing
        p_model->resolvePipeRouting();

        return true;

    }
    else return false;


}




//-----------------------------------------------------------------------------------------------
// Running the model, calculating results
//-----------------------------------------------------------------------------------------------
void Launcher::evaluate(Case *c, Component *comp)
{

    cout << endl << "---- Starting model evaluation # " << ++m_number_of_runs <<  " ----" << endl;

    if(comp == 0) evaluateEntireModel(c);   // the entire model should be evaluated

    else        // only a single component should be evaluated
    {
        // finding what type of component this is

        Pipe *p = dynamic_cast<Pipe*>(comp);
        Separator *s = dynamic_cast<Separator*>(comp);

        if(p != 0 && s == 0) evaluatePipe(c, p);  // this is a Pipe
        else
        {
            Well *w = dynamic_cast<Well*>(comp);
            if(w != 0) evaluateWell(c, w);  // this is a Well
        }


    }


    // letting the runner know the evaluation has finished
    emit finished(this, comp, c);


}


//-----------------------------------------------------------------------------------------------
// Running the entire model, calculating results
//-----------------------------------------------------------------------------------------------
void Launcher::evaluateEntireModel(Case *c)
{
    // checking that the case and the model have the same number of variables
    if(c->numberOfRealVariables() != p_model->realVariables().size())
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "When evaluating the model" << endl
             << "The Case does not have the same number of real variables as the Model" << endl
             << "CASE:  " << c->numberOfRealVariables() << endl
             << "MODEL: " << p_model->realVariables().size() << endl;
        exit(1);

    }
    if(c->numberOfBinaryVariables() != p_model->binaryVariables().size())
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "When evaluating the model" << endl
             << "The Case does not have the same number of binary variables as the Model" << endl
             << "CASE:  " << c->numberOfBinaryVariables() << endl
             << "MODEL: " << p_model->binaryVariables().size() << endl;
        exit(1);

    }
    if(c->numberOfIntegerVariables() != p_model->integerVariables().size())
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "When evaluating the model" << endl
             << "The Case does not have the same number of integer variables as the Model" << endl
             << "CASE:  " << c->numberOfIntegerVariables() << endl
             << "MODEL: " << p_model->integerVariables().size() << endl;
        exit(1);

    }



    // checking if the reservoir simulator must be rerun
    bool run_res_sim = rerunReservoirSimulator(c);

    // setting the variable values according to the case
    for(int i = 0; i < p_model->realVariables().size(); ++i)    // real variables
    {
        p_model->realVariables().at(i)->setValue(c->realVariableValue(i));
    }
    for(int i = 0; i < p_model->binaryVariables().size(); ++i)  // binary variables
    {
        p_model->binaryVariables().at(i)->setValue(c->binaryVariableValue(i));
    }
    for(int i = 0; i < p_model->integerVariables().size(); ++i)  // binary variables
    {
        p_model->integerVariables().at(i)->setValue(c->integerVariableValue(i));
    }

    // the variable values have changed, so the status of the model is no longer up to date
    p_model->setUpToDate(false);


    // running the reservoir simulator, if needed
    if(run_res_sim)
    {
        emit runningReservoirSimulator();

        p_simulator->generateInputFiles(p_model);   // generating input based on the current Model
        p_simulator->launchSimulator();             // running the simulator
        p_simulator->readOutput(p_model);           // reading output from the simulator run, and setting to Model
    }
    else
    {
        cout << "No need to run reservoir simulator, it is already up to date..." << endl;
    }

    // process the model
    // this will update the streams in the pipe network,
    // calculate pressures, update constraint values, and objective value
    p_model->process();

    // checking if this is an adjoints model
    AdjointsCoupledModel *am = dynamic_cast<AdjointsCoupledModel*>(p_model);

    if(am != 0)
    {
        *c = *am->results();
    }
    else
    {

        // copying back the results to the case
        for(int i = 0; i < p_model->constraints().size(); ++i)
        {
            c->addConstraintValue(p_model->constraints().at(i)->value());
        }

        c->setObjectiveValue(p_model->objective()->value());
    }



}


//-----------------------------------------------------------------------------------------------
// Evaluating only a single pipe in the model
//-----------------------------------------------------------------------------------------------
void Launcher::evaluatePipe(Case *c, Pipe *p)
{
    // the variables in the case should be: qo, qg, qw, p_out

    // checking that the case has the right number of variables
    if(c->numberOfRealVariables() != 4)
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "When evaluating the pressure drop in PIPE #" << p->number() << endl
             << "The Case does not have the correct number of variables" << endl
             << "They should be: qo, qg, qw, p" << endl;
        exit(1);

    }

    // creating a stream for the pressure drop calculation
    Stream *s = new Stream(0, c->realVariableValue(0), c->realVariableValue(1), c->realVariableValue(2), 0);

    // calculating the pressure drop
    // TODO: the hardcoding of METRIC here is not right!
    double dp = p->calculator()->pressureDrop(s, c->realVariableValue(3), Stream::METRIC);

    // setting the pressure drop as the objective
    c->setObjectiveValue(dp);


    /*

    // finding the real variables asociated with the pipe
    QVector<shared_ptr<RealVariable> > pipe_vars = model()->realVariables(p);

    // checking that the case has the correct number of variables
    if(c->numberOfRealVariables() != pipe_vars.size())
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "The Case does not have the same number of variables as the Pipe..." << endl << endl;
        exit(1);

    }

    // updating the values of the variables
    for(int i = 0; i < c->numberOfRealVariables(); ++i)
    {
        pipe_vars.at(i)->setValue(c->realVariableValue(i));
    }

    // calculating the pressure drop
    p->calculateInletPressure();

    // extracting the inlet pressure, setting it as the objective in the case
    c->setObjectiveValue(p->stream(0)->pressure());

    */

}

//-----------------------------------------------------------------------------------------------
// Evaluating a well in the model
//-----------------------------------------------------------------------------------------------
void Launcher::evaluateWell(Case *c, Well *w)
{
    // finding the real variables asociated with the well
    QVector<shared_ptr<RealVariable> > well_vars = model()->realVariables(w);

    // checking that the case has the correct number of variables
    if(c->numberOfRealVariables() != well_vars.size())
    {
        cout << endl << "###  Runtime Error  ###" << endl
             << "The Case does not have the same number of variables as the Well..." << endl
             << "CASE: " << c->numberOfRealVariables() << endl
             << "WELL: " << well_vars.size() << endl << endl;
        exit(1);

    }

    // updating the values of the variables
    for(int i = 0; i < c->numberOfRealVariables(); ++i)
    {
        well_vars.at(i)->setValue(c->realVariableValue(i));
    }

    // running the reservoir simulator
    emit runningReservoirSimulator();

    p_simulator->generateInputFiles(p_model);   // generating input based on the current Model
    p_simulator->launchSimulator();             // running the simulator
    p_simulator->readOutput(p_model);           // reading output from the simulator run, and setting to Model

    // finding the well in this copy of the model
    Well *w_m = p_model->wellById(w->id());

    // setting the rates and pressures as constraints: qo, qg, qw, pbh
    for(int i = 0; i < w_m->numberOfStreams(); ++i)
    {
        c->addConstraintValue(w_m->stream(i)->oilRate(true));
        c->addConstraintValue(w_m->stream(i)->gasRate(true));
        c->addConstraintValue(w_m->stream(i)->waterRate(true));
        c->addConstraintValue(w_m->stream(i)->pressure(true));
    }




}


//-----------------------------------------------------------------------------------------------
// Checks if the reservoir simulator has to be rerun
//-----------------------------------------------------------------------------------------------
bool Launcher::rerunReservoirSimulator(Case *c)
{
    bool rerun = false;

    if(!p_model->isUpToDate()) return true; // checks if the model has been evaluated yet

    // looping through the real variables
    for(int i = 0; i < p_model->realVariables().size(); ++i)
    {
        // checking if the variable is asociated with a well
        Well *w = dynamic_cast<Well*>(p_model->realVariables().at(i)->parent());

        if(w != 0)  // this var is a well control variable
        {


            if(c->realVariableValue(i) != p_model->realVariables().at(i)->value())  // the  variable value has changed from whats in the model
            {
                rerun = true;
                break;  // no need to check any more variables
            }
        }

    }

    return rerun;
}


} // namespace ResOpt
