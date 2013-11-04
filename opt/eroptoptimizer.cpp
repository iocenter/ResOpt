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

#include <QTextStream>

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
    delete p_debug_file;
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::initialize()
{

    cout << "Initializing EROPT..." << endl;

    // checking the stepping
    if(m_steps.size() == 0) m_steps.push_back(1);


    // setting up the initial prefered move directions
    setInitialDirections();

    // setting up the debug file
    p_debug_file = new QFile(runner()->reservoirSimulator()->folder() + "/eropt.out");

    if(!p_debug_file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning("Could not connect to EROPT debug file: %s", p_debug_file->fileName().toLatin1().constData());

        delete p_debug_file;
        p_debug_file = 0;

        exit(1);
    }
    else
    {
        // deleting content from previous launches
        p_debug_file->resize(0);

    }

}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::start()
{
    if(m_directions.size() == 0) initialize();


    QTextStream out(p_debug_file);

    out << "------------------------\n";
    out << "------------------------\n";
    out << "--- EROPT DEBUG FILE ---\n";
    out << "------------------------\n";
    out << "------------------------\n\n";

    out.flush();
    p_debug_file->flush();





    cout << "#################################" << endl;
    cout << "###      Starting EROPT       ###" << endl;
    cout << "###  running starting point   ###" << endl;
    cout << "#################################" << endl;

    // generating the base-case
    Case *base_case = generateBaseCase();

    //solving the contineous sub-problem for the base-case...
    Case *result_base_case = p_evaluator->solveContineousProblem(base_case);




    // new loop
    bool converged;
    Case *result;

    for(int i = 0; i < m_steps.size(); ++i)
    {
        cout << "#################################" << endl;
        cout << "###           EROPT           ###" << endl;
        cout << "###       step length: " << m_steps.at(i) << "      ###" << endl;
        cout << "#################################" << endl;

        converged = false;

        // starting the algorithm
        while(!converged && maxIterations() >= p_evaluator->iterations())
        {
            converged = true;

            result = solve(result_base_case, 0, &converged, m_steps.at(i));
            result_base_case = result;


        }
    }




    // old loop:


    /*
    bool converged = false;

    // starting the algorithm
    Case *result;

    //cout << "EROPT: staring opt with step length 1" << endl;
    while(!converged && maxIterations() >= p_evaluator->iterations())
    {
        converged = true;

        result = solve(result_base_case, 0, &converged, 1);
        result_base_case = result;


    }

    cout << "#################################" << endl;
    cout << "###      Starting EROPT       ###" << endl;
    cout << "###      step length = 2      ###" << endl;
    cout << "#################################" << endl;

    converged = false;
    while(!converged && maxIterations() >= p_evaluator->iterations())
    {
        converged = true;

        result = solve(result_base_case, 0, &converged, 2);
        result_base_case = result;


    }

    cout << "#################################" << endl;
    cout << "###      Starting EROPT       ###" << endl;
    cout << "###      step length = 1      ###" << endl;
    cout << "###       second round        ###" << endl;
    cout << "#################################" << endl;

    converged = false;
    while(!converged && maxIterations() >= p_evaluator->iterations())
    {
        converged = true;

        result = solve(result_base_case, 0, &converged, 1);
        result_base_case = result;


    }

*/


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


    delete base_case;

    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// solves the problem
//-----------------------------------------------------------------------------------------------
Case* EroptOptimizer::solve(Case *base_case, int start_var, bool *converged, int step_length)
{
    // debug printing

    //cout << "EROPT: solve start" << endl;

    QTextStream out(p_debug_file);

    out << "---------------------------------------------\n";
    out << "current variable: " << start_var << "\n";
    out << "step length:      " << step_length << "\n\n";
    out << "Best solution before perturbing this variable:\n";
    for(int i = 0; i < base_case->numberOfIntegerVariables(); ++i)
    {
        out << "VAR " << i << " = " << base_case->integerVariableValue(i) << "\n";
    }

    out << "OBJ   = " << base_case->objectiveValue() << "\n";
    out << "\n";

    out.flush();
    p_debug_file->flush();

    //cout << "EROPT: done writing to debug file" << endl;

    if(start_var >= base_case->numberOfIntegerVariables()) return base_case;

    //cout << "EROPT: setting up results case" << endl;

    Case *result = new Case();
    //cout << "EROPT: done setting up results case" << endl;

    Case *c = new Case(*base_case);

    //cout << "EROPT: done setting up int variables case" << endl;

    //cout << "startvar = " << start_var << endl;

    //if(start_var >= m_directions.size()) cout << "startvar too big!" << endl;

    perturbVariable(c, start_var, m_directions.at(start_var), step_length);

    //cout << "EROPT: done perturbing variable" << endl;

    // solving the contineous sub-problem
    Case *result_i = p_evaluator->solveContineousProblem(c);
    delete c;

    if(isBetter(result_i, base_case))
    {
        out << "Found better solution: \n";
        out << "move direction = original\n";
        out << "VAR " << start_var << " = " << result_i->integerVariableValue(start_var) << "\n";
        out << "OBJ   = " << result_i->objectiveValue() << "\n\n";
        out.flush();
        p_debug_file->flush();


        // move to next var
        *converged = false;
        result = solve(result_i, start_var+1, converged, step_length);
    }
    else
    {
        // try other direction
        c = new Case(*base_case);
        perturbVariable(c, start_var, switchDirection(m_directions.at(start_var)), step_length);
        result_i = p_evaluator->solveContineousProblem(c);
        delete c;

        // checking if this direction was better than the base case
        if(isBetter(result_i, base_case))
        {
            m_directions.replace(start_var, switchDirection(m_directions.at(start_var)));


            out << "Found better solution: \n";
            out << "move direction = switched\n";
            out << "VAR " << start_var << " = " << result_i->integerVariableValue(start_var) << "\n";
            out << "OBJ   = " << result_i->objectiveValue() << "\n\n";
            out.flush();
            p_debug_file->flush();



            // move to next variable
            *converged = false;
            result = solve(result_i, start_var+1, converged, step_length);
        }
        else
        {
            // this variable is at the optimum
            // move to next variable
            //*converged = true;

            out << "Did not find better solution \n";
            out << "going to next variable\n\n";
            out.flush();
            p_debug_file->flush();


            result = solve(base_case, start_var+1, converged, step_length);
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
    for(int i = 0; i < runner()->model()->integerVariables().size(); ++i)
    {
        c->addIntegerVariableValue(runner()->model()->integerVariableValue(i));
    }

    // binary variables
    for(int i = 0; i < runner()->model()->binaryVariables().size(); ++i)
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
    //cout << "EROPT: pertrubVariable start" << endl;

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

    //cout << "EROPT: pertrubVariable end" << endl;
}


//-----------------------------------------------------------------------------------------------
// sets up the initial prefered move directions according to distance from bounds
//-----------------------------------------------------------------------------------------------
void EroptOptimizer::setInitialDirections()
{
    Model *m = runner()->model();

    for(int i = 0; i < m->integerVariables().size(); ++i)
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
    if(c->infeasibility() >  0.0001) return false;

    else return c->objectiveValue() > base_case->objectiveValue();
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
    str.append(" CONT_ITER " + QString::number(maxIterContineous()) + "\n");
    str.append(" PERTURBATION " + QString::number(pertrurbationSize()) + "\n");
    str.append(" PARALLELRUNS " + QString::number(parallelRuns()) + "\n");
    str.append(" TERMINATION " + QString::number(termination()) + " " + QString::number(terminationStart()) + "\n");
    str.append("END OPTIMIZER\n\n");
    return str;
}





} // namespace ResOpt
