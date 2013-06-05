/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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


#include "lshnomadevaluator.h"

#include "nomadevaluator.h"

#include <tr1/memory>
#include <iostream>

#include "lshoptimizer.h"
#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "case.h"
#include "casequeue.h"

using std::tr1::shared_ptr;
using std::endl;
using std::cout;

namespace ResOpt
{

LshNomadEvaluator::LshNomadEvaluator(const NOMAD::Parameters &p, LshOptimizer *o)
    : NOMAD::Evaluator(p),
      p_optimizer(o)
{

    cout << "LshNomadEvaluator constructor" << endl;
}

//-----------------------------------------------------------------------------------------------
// evaluates the model
//-----------------------------------------------------------------------------------------------
bool LshNomadEvaluator::eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const
{
    // generating a case from the evaluation point
    Case *c = generateCase(x);
    CaseQueue *cq = new CaseQueue();

    cq->push_back(c);

    // sending the case off for evaluation
    p_optimizer->sendCasesToOptimizer(cq);

    ///p_optimizer->runCase(c);

    // extracting the objective
    x.set_bb_output(0, -c->objectiveValue());





    // extracting the constraint values
    // the constraints in NOMAD must be on the form: c <= 0

    for(int i = 0; i < c->numberOfConstraints(); ++i)
    {
        double val_input;
        double val = c->constraintValue(i);
        double max = p_optimizer->runner()->model()->constraints().at(i)->max();
        double min = p_optimizer->runner()->model()->constraints().at(i)->min();


        if(val > max) val_input = val - max;

        else if(val < min) val_input = min - val;
        else
        {
            double u_slack = max - val;
            double l_slack = val - min;

            val_input = (u_slack > l_slack) ? -u_slack : -l_slack;
        }

        x.set_bb_output(i+1, val_input);
    }

    // deleting the case from the heap
    delete c;
    delete cq;

    return true;


}


//-----------------------------------------------------------------------------------------------
// generates a case from an Eval_Point
//-----------------------------------------------------------------------------------------------
Case* LshNomadEvaluator::generateCase(const NOMAD::Eval_Point &x) const
{
    Case *c = new Case();

    int var_num = 0;

    for(int i = 0; i < p_optimizer->runner()->model()->realVariables().size(); ++i)
    {
        c->addRealVariableValue(x[var_num].value());
        ++var_num;
    }




    return c;

}

} // namespace ResOpt
