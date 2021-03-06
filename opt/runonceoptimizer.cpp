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
    : Optimizer(r),
      p_c(0)
{
}

RunonceOptimizer::~RunonceOptimizer()
{
   if(p_c != 0) delete p_c;
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


    if(p_c != 0) delete p_c;

    // creating a case for the current values in the model
    p_c = new Case(runner()->model());


    // launching the runner on the case
    runCase(p_c);


    // best case (needed...)
    runner()->writeBestCaseToSummary(p_c);


    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}



//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString RunonceOptimizer::description() const
{
    QString str("START OPTIMIZER\n");
    str.append(" TYPE RUNONCE \n");
    str.append(" ITERATIONS " + QString::number(maxIterations()) + "\n");
    str.append(" PERTURBATION " + QString::number(pertrurbationSize()) + "\n");
    str.append(" PARALLELRUNS " + QString::number(parallelRuns()) + "\n");
    str.append("END OPTIMIZER\n\n");
    return str;
}





} // namespace ResOpt
