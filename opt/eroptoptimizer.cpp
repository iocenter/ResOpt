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


#include "eroptoptimizer.h"

#include "runner.h"
#include "model.h"
#include "reservoirsimulator.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "case.h"
#include "minlpevaluator.h"

#include <tr1/memory>
#include <iostream>

using std::tr1::shared_ptr;
using std::cout;
using std::endl;

namespace ResOpt
{


EroptOptimizer::EroptOptimizer(Runner *r)
    : Optimizer(r)
{
    p_evaluator = new MINLPEvaluator(this);
}

EroptOptimizer::~EroptOptimizer()
{
    delete p_evaluator;
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::initialize()
{

    cout << "Initializing EROPT..." << endl;

    // setting up the initial prefered move directions
    setInitialDirections();
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::start()
{
    // generating the base-case
    Case *base_case = generateBaseCase();

    //solving the contineous sub-problem for the base-case...
    Case *result_base_case = p_evaluator->solveContineousProblem(base_case);
    delete base_case;

    bool converged = false;

    // starting the algorithm
    Case *result;

    while(!converged && maxIterations() >= p_evaluator->iterations())
    {
        converged = true;

        result = solve(result_base_case, 0, &converged);
        result_base_case = result;

        cout << "done with solve!" << endl;
    }



    if(converged)
    {
        cout << "#################################" << endl;
        cout << "###      EROPT converged!     ###" << endl;
        cout << "#################################" << endl;
    }
    else
    {
        cout << "#################################" << endl;
        cout << "###  EROPT did not converge!  ###" << endl;
        cout << "#################################" << endl;
    }

    // sending best case to runner
    sendBestCaseToRunner(result);




    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// solves the problem
//-----------------------------------------------------------------------------------------------
Case* EroptOptimizer::solve(Case *base_case, int start_var, bool *converged)
{
    if(start_var >= base_case->numberOfIntegerVariables()) return base_case;


    Case *result = new Case();


    Case *c = new Case(*base_case);
    perturbVariable(c, start_var, m_directions.at(start_var));

    // solving the contineous sub-problem
    Case *result_i = p_evaluator->solveContineousProblem(c);
    delete c;

    if(isBetter(result_i, base_case))
    {
        // move to next var
        *converged = false;
        result = solve(result_i, start_var+1, converged);
    }
    else
    {
        // try other direction
        c = new Case(*base_case);
        perturbVariable(c, start_var, switchDirection(m_directions.at(start_var)));
        result_i = p_evaluator->solveContineousProblem(c);
        delete c;

        // checking if this direction was better than the base case
        if(isBetter(result_i, base_case))
        {
            m_directions.replace(start_var, switchDirection(m_directions.at(start_var)));

            // move to next variable
            *converged = false;
            result = solve(result_i, start_var+1, converged);
        }
        else
        {
            // this variable is at the optimum
            // move to next variable
            //*converged = true;
            result = solve(base_case, start_var+1, converged);
        }


    }


    return result;


}


//-----------------------------------------------------------------------------------------------
// generates a case with the starting point values for int and bin variables
//-----------------------------------------------------------------------------------------------
Case* EroptOptimizer::generateBaseCase()
{
    Case *c = new Case();

    // integer variables
    for(int i = 0; i < runner()->model()->numberOfIntegerVariables(); ++i)
    {
        c->addIntegerVariableValue(runner()->model()->integerVariableValue(i));
    }

    // binary variables
    for(int i = 0; i < runner()->model()->numberOfBinaryVariables(); ++i)
    {
        c->addBinaryVariableValue(runner()->model()->binaryVariableValue(i));
    }

    return c;
}


//-----------------------------------------------------------------------------------------------
// changes the value of the specified variable
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::perturbVariable(Case *c, int i_var, direction move_direction, int step)
{
    // getting the bounds
    int bound_u = runner()->model()->integerVariables().at(i_var)->max();
    int bound_l = runner()->model()->integerVariables().at(i_var)->min();

    int value_new_up = c->integerVariableValue(i_var) + step;
    int value_new_down = value_new_up - 2*step;

    // perturbing the variable
    if(move_direction == UP)
    {
        if(value_new_up <= bound_u) c->setIntegerVariableValue(i_var, value_new_up);
        else if(value_new_down >= bound_l) c->setIntegerVariableValue(i_var, value_new_down);
    }
    else
    {
        if(value_new_down >= bound_l) c->setIntegerVariableValue(i_var, value_new_down);
        else if(value_new_up <= bound_u) c->setIntegerVariableValue(i_var, value_new_up);
    }
}


//-----------------------------------------------------------------------------------------------
// sets up the initial prefered move directions according to distance from bounds
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::setInitialDirections()
{
    Model *m = runner()->model();

    for(int i = 0; i < m->numberOfIntegerVariables(); ++i)
    {
        shared_ptr<IntVariable> var = m->integerVariables().at(i);

        int span_u = var->max() - var->value();
        int span_l = var->value() - var->min();

        if(span_u >= span_l) m_directions.push_back(UP);
        else m_directions.push_back(DOWN);
    }
}


//-----------------------------------------------------------------------------------------------
// compares c against a base case to se if it is better
//-----------------------------------------------------------------------------------------------
bool EroptOptimizer::isBetter(Case *c, Case *base_case)
{
    return c->objectiveValue() > base_case->objectiveValue();
}


//-----------------------------------------------------------------------------------------------
// switches the direction UP/DOWN
//-----------------------------------------------------------------------------------------------
EroptOptimizer::direction EroptOptimizer::switchDirection(direction d)
{
    return (d == UP) ? DOWN : UP;
}

//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString EroptOptimizer::description() const
{
    QString str("START OPTIMIZER\n");
    str.append(" TYPE EROPT \n");
    str.append(" ITERATIONS " + QString::number(maxIterations()) + "\n");
    str.append(" PERTURBATION " + QString::number(pertrurbationSize()) + "\n");
    str.append(" PARALLELRUNS " + QString::number(parallelRuns()) + "\n");
    str.append(" TERMINATION " + QString::number(termination()) + " " + QString::number(terminationStart()) + "\n");
    str.append("END OPTIMIZER\n\n");
    return str;
}





} // namespace ResOpt
