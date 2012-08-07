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


#include "optimizer.h"
#include "runner.h"
#include "casequeue.h"
#include <QEventLoop>

#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{


Optimizer::Optimizer(Runner *r)
    : QObject(r),
      p_runner(r),
      m_max_iter(1),
      m_parallel_runs(1),
      m_perturbation_size(0.0001),
      m_initialized(false)
{
    // the finished() signal should be emitted when the optimizer has converged
    connect(this, SIGNAL(finished()), p_runner, SIGNAL(optimizationFinished()));
}

Optimizer::~Optimizer()
{
    // nothing to delete
}

//-----------------------------------------------------------------------------------------------
// sends off a queue of cases to the runner for evaluation
//-----------------------------------------------------------------------------------------------
void Optimizer::runCases(CaseQueue *cases, Component *comp)
{
    // creating an event loop that waits for all the cases to finish in the runner
    QEventLoop loop;

    // this makes the event loop wait for the casesFinished() signal to quit
    connect(p_runner, SIGNAL(casesFinished()), &loop, SLOT(quit()));

    // sending the cases to the runner
    p_runner->evaluate(cases, comp);

    // waiting for the runner to finish
    loop.exec();
}

//-----------------------------------------------------------------------------------------------
// sends a single case to the runner for evaluation
//-----------------------------------------------------------------------------------------------
void Optimizer::runCase(Case *c, Component *comp)
{
    // making a queue
    CaseQueue *queue = new CaseQueue();

    // adding the case
    queue->push_back(c);

    // running the queue
    runCases(queue, comp);

    // deleting the queue
    delete queue;
    queue = 0;
}




//-----------------------------------------------------------------------------------------------
// sends the best case to the runner for printing
//-----------------------------------------------------------------------------------------------
void Optimizer::sendBestCaseToRunner(Case *c)
{
    connect(this, SIGNAL(bestCase(Case*)), p_runner, SLOT(writeBestCaseToSummary(Case*)));

    emit bestCase(c);

    disconnect(this, SIGNAL(bestCase(Case*)), p_runner, SLOT(writeBestCaseToSummary(Case*)));

}

} // namespace ResOpt
