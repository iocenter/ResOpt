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

#include "model.h"
#include "reservoirsimulator.h"
#include "case.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"

// for debug
#include "pipe.h"
#include "stream.h"
#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{

Launcher::Launcher(QObject *parent)
    : QObject(parent),
      p_model(0),
      p_simulator(0)
{
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
void Launcher::evaluate(Case *c)
{
    // setting the variable values according to the case
    for(int i = 0; i < p_model->realVariables().size(); ++i)    // real variables
    {
        // need to add a check to see if anything requiering the reservoir simulator to launch has changed
        p_model->realVariables().at(i)->setValue(c->realVariableValue(i));
    }
    for(int i = 0; i < p_model->binaryVariables().size(); ++i)  // binary variables
    {
        p_model->binaryVariables().at(i)->setValue(c->binaryVariableValue(i));
    }


    // running the reservoir simulator
    p_simulator->generateInputFiles(p_model);   // generating input based on the current Model
    p_simulator->launchSimulator();             // running the simulator
    p_simulator->readOutput(p_model);           // reading output from the simulator run, and setting to Model


    // update the streams in the pipe network
    p_model->updateStreams();

    // calculating pressures in the Pipe network
    p_model->calculatePipePressures();

    // updating the constraints (this must be done after the pressure calc)
    p_model->updateConstraints();

    // updating the objective
    p_model->updateObjectiveValue();


    // copying back the results to the case
    for(int i = 0; i < p_model->constraints().size(); ++i)
    {
        c->addConstraintValue(p_model->constraints().at(i)->value());
    }

    c->setObjectiveValue(p_model->objective()->value());

    //// debug code /////

    // checking the rates for all the pipes
    for(int i = 0; i < p_model->numberOfPipes(); ++i)
    {
        cout << "------ Pipe #" << p_model->pipe(i)->number() << " ------" << endl;

        for(int j = 0; j < p_model->pipe(i)->numberOfStreams(); ++j)
        {
            p_model->pipe(i)->stream(j)->printToCout();
        }
    }


    // letting the runner know the evaluation has finished
    emit finished(this);


}



} // namespace ResOpt
