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


#include "runonceoptimizer.h"

#include <iostream>
#include "runner.h"
#include "case.h"
#include "casequeue.h"

#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"
#include "pipe.h"
#include <QVector>

#include <tr1/memory>



using std::tr1::shared_ptr;



using std::cout;
using std::endl;

namespace ResOpt
{


RunonceOptimizer::RunonceOptimizer(Runner *r)
    : Optimizer(r)
{
}

RunonceOptimizer::~RunonceOptimizer()
{
    // nothing to do
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::initialize()
{
    // nothing to do here

    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::start()
{
    cout << "Starting RUN_ONCE..." << endl;

    // creating a case for the current values in the model
    Case *c = new Case(runner()->model());


    // launching the runner on the case
    runCase(c);




    delete c;



    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// builds a case for a pipe
//-----------------------------------------------------------------------------------------------
Case* RunonceOptimizer::buildCase(Pipe *p)
{
    Case *c = new Case();

    c->addRealVariableValue(0);
    c->addRealVariableValue(100);
    c->addRealVariableValue(0);
    c->addRealVariableValue(200);

    return c;
}




} // namespace ResOpt
