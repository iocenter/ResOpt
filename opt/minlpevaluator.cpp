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


#include "minlpevaluator.h"

#include "optimizer.h"
#include "runner.h"
#include "reservoirsimulator.h"
#include "minlpipoptinterface.h"
#include "case.h"


#include <iostream>

using std::cout;
using std::endl;

#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"

using namespace Ipopt;


namespace ResOpt
{


MINLPEvaluator::MINLPEvaluator(Optimizer *o) :
    p_optimizer(o),
    m_iterations(0),
    m_iter_cont(0)
{
}

MINLPEvaluator::~MINLPEvaluator()
{
    for(int i = 0; i < m_results.size(); ++i) delete m_results.at(i);
}

//-----------------------------------------------------------------------------------------------
// solves the contineous sub-problem using IPOPT
//-----------------------------------------------------------------------------------------------
Case* MINLPEvaluator::solveContineousProblem(Case *discrete_vars)
{
    // checking if the case has been solved already
    Case *result = findResult(discrete_vars);
    if(result != 0) return result;


    // else need to solve the case

    // ----- Initializing IPOPT ------- //

    m_iter_cont = 0;

    // setting up the TNLP
    SmartPtr<MINLPIpoptInterface> p_tnlp = new MINLPIpoptInterface(p_optimizer, this, discrete_vars);

    // Create an instance of the IpoptApplication
    SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

    // Change some options
    app->Options()->SetStringValue("output_file", p_optimizer->runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.out");
    app->Options()->SetIntegerValue("max_iter", 50);
    app->Options()->SetIntegerValue("max_soc", 1);
    app->Options()->SetNumericValue("tol", 0.01);
    app->Options()->SetNumericValue("dual_inf_tol", 5);
    app->Options()->SetNumericValue("constr_viol_tol", 0.001);
    app->Options()->SetNumericValue("compl_inf_tol", 0.02);
    app->Options()->SetStringValue("accept_every_trial_step", "yes");


    //app->Options()->SetStringValue("derivative_test", "first-order");
    //app->Options()->SetStringValue("derivative_test_print_all", "yes");
    //app->Options()->SetNumericValue("derivative_test_perturbation", 0.001);


    app->Options()->SetStringValue("hessian_approximation", "limited-memory"); // exact (default, no approx) or limited-memory (quasi-Newton)

    // Initialize the IpoptApplication and process the options
    ApplicationReturnStatus status = app->Initialize();


    if (status != Solve_Succeeded)
    {
        cout << endl << endl << "*** Error during IPOPT initialization!" << endl;
        //cout << "Status = " << status.s
        exit(1);
    }


    // ----- Starting IPOPT ------- //
    status = app->OptimizeTNLP(GetRawPtr(p_tnlp));

    if (status == Solve_Succeeded)
    {
        cout << endl << endl << "*** The contineous IPOPT sub-problem problem solved!" << endl;
    }
    else
    {
        cout << endl << endl << "*** The contineous IPOPT sub-problem problem FAILED!" << endl;
    }

    // checking if this is the best solution so far
    QVector<double> objs_current = p_tnlp->objectives();
    QVector<double> infeas_current = p_tnlp->infeasibilities();

    if(m_best_objs.size() == 0)
    {
        m_best_objs = objs_current;
        m_best_infeas = infeas_current;
    }

    else if(m_best_objs.last() > objs_current.last() && m_best_infeas.last() >= (infeas_current.last()-0.01))
    {
     //   cout << "---- current sub-problem has the highest obj ----" << endl;
        m_best_objs = objs_current;
        m_best_infeas = infeas_current;
    }

    // extracting the final solution from IPOPT
    result = p_tnlp->bestCase();

    Case *result_new = new Case(*result);
    p_optimizer->runCase(result_new);
    m_results.push_back(result_new);

    // increasing the number of iterations
    ++m_iterations;

    return result_new;


}

//-----------------------------------------------------------------------------------------------
// checks if the current contineous optimization should continue
//-----------------------------------------------------------------------------------------------
bool MINLPEvaluator::shouldContinue(int i, double obj, double infeas)
{
    //cout << "checking objective for iteration #" << i << " against obj = " << obj << endl;
    //cout << "number of best objs = " << m_best_objs.size() << endl;

    // checking if the max number of iterations for the sub-problem is reached
    if(++m_iter_cont > p_optimizer->maxIterContineous()) return false;


    if(i < p_optimizer->terminationStart())
    {
       // cout << "------------------------------------" << endl;
       // cout << "dont check yet" << endl;
       // cout << "------------------------------------" << endl;
        return true;
    }

    if(m_best_objs.size() == 0) return true;

    // if the current ipopt has gotten further than the current best
    if(i >= m_best_objs.size())
    {

        double obj_best;
        if(m_best_objs.last() > 0) obj_best = m_best_objs.last() / (p_optimizer->termination() + 0.0001);
        else obj_best = m_best_objs.last() * p_optimizer->termination();

        //cout << "------------------------------------" << endl;
        //cout << "current is further" << endl;
        //cout << "obj    = " << obj << " cur = " << obj_best << endl;
        //cout << "infeas = " << infeas << " cur = " << m_best_infeas.last() << endl;
        //cout << "------------------------------------" << endl;


        bool ok_obj = obj_best > obj;
        bool ok_infeas = m_best_infeas.last() >= infeas;

        if(!ok_obj && !ok_infeas) return false;
        else return true;

    }

    else
    {


        double obj_best;
        if(m_best_objs.at(i) > 0) obj_best = m_best_objs.at(i) / (p_optimizer->termination() + 0.0001);
        else obj_best = m_best_objs.at(i) * p_optimizer->termination();

        //cout << "------------------------------------" << endl;
        //cout << "on same path" << endl;
        //cout << "obj    = " << obj << " cur = " << obj_best << endl;
        //cout << "infeas = " << infeas << " cur = " << m_best_infeas.at(i) << endl;
        //cout << "------------------------------------" << endl;


        bool ok_obj = obj_best > obj;
        bool ok_infeas = m_best_infeas.at(i) >= infeas;

        if(!ok_obj && !ok_infeas) return false;
        else return true;


    }
}

//-----------------------------------------------------------------------------------------------
// Finds a result case with int and bin var values in c
//-----------------------------------------------------------------------------------------------
Case* MINLPEvaluator::findResult(Case *c)
{
    for(int i = 0; i < m_results.size(); ++i)
    {
        Case *r = m_results.at(i);

        // checking the results case for match
        if(r->numberOfBinaryVariables() != c->numberOfBinaryVariables()) return 0;
        if(r->numberOfIntegerVariables() != c->numberOfIntegerVariables()) return 0;

        bool ok = true;
        for(int j = 0; j < c->numberOfBinaryVariables(); ++j)
        {
            if(c->binaryVariableValue(j) != r->binaryVariableValue(j))
            {
                ok = false;
                break;
            }

        }
        if(ok)
        {
            for(int j = 0; j < c->numberOfIntegerVariables(); ++j)
            {
                if(c->integerVariableValue(j) != r->integerVariableValue(j))
                {
                    ok = false;
                    break;
                }
            }
        }

        if(ok) return r;

    }

    return 0;


}

} // namespace
