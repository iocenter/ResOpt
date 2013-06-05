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


#include "lshoptimizer.h"
#include "lshipoptinterface.h"
#include "lshnomadevaluator.h"

#include <iostream>
#include <exception>

#include "runner.h"
#include "case.h"
#include "casequeue.h"

#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "pipe.h"
#include <QVector>

#include <tr1/memory>

#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"


using namespace Ipopt;




using std::tr1::shared_ptr;
using std::exception;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;

namespace ResOpt
{


LshOptimizer::LshOptimizer(Runner *r)
    : Optimizer(r),
      p_best_solution(0),
      m_last_sol_best(false),
      m_use_nomad(true)
{
}

LshOptimizer::~LshOptimizer()
{
    // deleting all solutions
    for(int i = 0; i < m_solutions.size(); ++i) delete m_solutions.at(i);
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void LshOptimizer::initialize()
{

    cout << "Initializing LSH Algorithm..." << endl;

    // setting up the best solution

    p_best_solution = new Case();

    p_best_solution->setObjectiveValue(-1e12);

    m_solutions.push_back(p_best_solution);
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void LshOptimizer::start()
{
    cout << "Starting LSH Algorithm..." << endl;

    Model *m = runner()->model();


    // ---- first solving for the starting values of bin and int vars ----

    cout << "---- Solving the contineous problem for the starting point ----" << endl;
    // setting up the current values for the bin and int
    p_current_values = new Case(m);

    // solving the contineous sub-problem
    solveContineous();


    // starting to do the local search on binary and integer variables
    int iter_count = 0;
    bool converged = false;

    while(iter_count < maxIterations() && !converged)
    {
        cout << "---- Starting iteration #" << iter_count +1 << " ----" << endl;

        // starting with the integer variables
        for(int i = 0; i < m->numberOfIntegerVariables(); ++i)
        {

            // calculating new variable value
            int var_val_old = p_current_values->integerVariableValue(i);
            int var_val;
            if(p_current_values->integerVariableValue(i) < m->integerVariables().at(i)->max()) var_val += 1;
            else var_val -= 1;

            // updating the variable value
            p_current_values->setIntegerVariableValue(i, var_val);

            // run the contineous sub-problem for new int var value
            solveContineous();


            // resetting the variable value
            p_current_values->setIntegerVariableValue(i, var_val_old);

            // checking if this solution is the best so far
            if(m_last_sol_best)
            {

                cout << "LSH found a better solution!" << endl;
                converged = true;
                break;
            }

        }

        // then doing the binary variables, if not converged

        ++iter_count;
    }


    // printing the best solution

    cout << "LSH Converged. The best solution is:" << endl;
    cout << p_best_solution->objectiveValue() << endl << endl;




    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Solves the contineous sub-problem
//-----------------------------------------------------------------------------------------------
void LshOptimizer::solveContineous()
{
    if(m_use_nomad) solveContineousNomad();
    else solveContineousIpopt();
}

//-----------------------------------------------------------------------------------------------
// Solves the contineous sub-problem using IPOPT
//-----------------------------------------------------------------------------------------------
void LshOptimizer::solveContineousIpopt()
{


    // initializing the problem

    // setting up the TNLP
    SmartPtr<LshIpoptInterface> p_tnlp = new LshIpoptInterface(this);

    // Create an instance of the IpoptApplication
    // We are using the factory, since this allows us to compile this
    // example with an Ipopt Windows DLL
    SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

    // Change some options
    // Note: The following choices are only examples, they might not be
    //       suitable for your optimization problem.
    app->Options()->SetNumericValue("tol", 1e-1);
    app->Options()->SetNumericValue("dual_inf_tol", 3e9);
    app->Options()->SetNumericValue("compl_inf_tol", 10);

    //app->Options()->SetStringValue("mu_strategy", "adaptive");
    //app->Options()->SetStringValue("output_file", runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.out");

    // Solver options
    app->Options()->SetIntegerValue("max_iter", 10);

    // Derivative checks
    //app->Options()->SetStringValue("check_derivatives_for_naninf", "yes"); // no (default) or yes (may produce a lot of output)
    //app->Options()->SetStringValue("derivative_test", "first-order"); // none (default)

    // Quasi-Newton approximation of Hessian of Lagrangian
    app->Options()->SetStringValue("hessian_approximation", "limited-memory"); // exact (default, no approx) or limited-memory (quasi-Newton)
    //app->Options()->SetStringValue("limited_memory_update_type", "bfgs"); // BFGS (default) or SR1 (not working well)

    // Initialize the IpoptApplication and process the options
    app->Initialize();


    //Set up done, now let's run Ipopt
    ApplicationReturnStatus status;
    status = app->OptimizeTNLP(GetRawPtr(p_tnlp));

    if (status == Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;


    }
    else
    {
        std::cout << std::endl << std::endl << "*** The problem FAILED!" << std::endl;
    }

    // As the SmartPtrs go out of scope, the reference count
    // will be decremented and the objects will automatically
    // be deleted.
    //return (int) status;

}

//-----------------------------------------------------------------------------------------------
// Solves the contineous sub-problem using NOMAD
//-----------------------------------------------------------------------------------------------
void LshOptimizer::solveContineousNomad()
{
    // initializing
    NOMAD::Display *p_disp = new NOMAD::Display( std::cout );
    NOMAD::Parameters *p_param = 0;
    LshNomadEvaluator *p_evaluator = 0;


    p_disp->precision ( NOMAD::DISPLAY_PRECISION_STD );


    try
    {

        // NOMAD initializations:
        NOMAD::begin(0, 0); // hope this works, was: begin(argc, argv)



        // parameters creation:
        p_param = generateNomadParameters(p_disp);

        // custom evaluator creation:
        p_evaluator = new LshNomadEvaluator(*p_param, this);


        // algorithm creation and execution:
        NOMAD::Mads mads ( *p_param , p_evaluator );
        mads.run();

        // getting the best feasible solution, sending it to the runner if exists
        const NOMAD::Eval_Point *best_feas = mads.get_best_feasible();
        if(best_feas != NULL)
        {
            const NOMAD::Point outputs = best_feas->get_bb_outputs();

            // the variable values
            Case *c = p_evaluator->generateCase(*best_feas);

            // the objective
            c->setObjectiveValue(-outputs[0].value());

            // the constraints
            //for(int i = 1; i < outputs.size(); ++i) c->addConstraintValue(outputs[i].value());

            // setting as current solution
            setCurrentSolution(c);




        }

    }
    catch(exception &e)
    {
        cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
    }

    NOMAD::Slave::stop_slaves (*p_disp);
    NOMAD::end();

    // deleting pointers
    delete p_disp;
    p_disp = 0;

    if(p_evaluator != 0)
    {
        delete p_evaluator;
        p_evaluator = 0;
    }
    if(p_param != 0)
    {
        delete p_param;
        p_param = 0;
    }

}

//-----------------------------------------------------------------------------------------------
// Generates the parameters for NOMAD
//-----------------------------------------------------------------------------------------------
NOMAD::Parameters* LshOptimizer::generateNomadParameters(NOMAD::Display *disp)
{
    NOMAD::Parameters *p_param = new NOMAD::Parameters(*disp);

    // finding the number of variables
    int n_var = runner()->model()->realVariables().size();

    p_param->set_DIMENSION (n_var);     // number of variables

    NOMAD::Point sp(n_var);             // starting point
    NOMAD::Point ub(n_var);             // upper bounds
    NOMAD::Point lb(n_var);             // lower bounds


    // setting all the values for each variable
    for(int i = 0; i < runner()->model()->realVariables().size(); ++i)
    {
        shared_ptr<RealVariable> v = runner()->model()->realVariables().at(i);

        // setting the type
        p_param->set_BB_INPUT_TYPE(i, NOMAD::CONTINUOUS);

        // setting the starting point
        sp[i] = v->value();

        // setting the upper bound
        ub[i] = v->max();

        //setting the lower bound
        lb[i] = v->min();


    }




    // setting the starting point, upper and lower bounds to the parameters
    p_param->set_X0(sp);
    p_param->set_UPPER_BOUND(ub);
    p_param->set_LOWER_BOUND(lb);


    // setting the objective and constraint info
    int n_con = runner()->model()->constraints().size();

    vector<NOMAD::bb_output_type> bbot (n_con + 1); // definition of output types
    bbot[0] = NOMAD::OBJ;       // the first is the objective
    for(int i = 1; i < n_con + 1; ++i) bbot[i] = NOMAD::PB;     // the rest are constraints

    p_param->set_BB_OUTPUT_TYPE ( bbot );

    // not sure what this does...
    p_param->set_DISPLAY_STATS ( "bbe ( sol ) obj" );

    // setting the maximum number of iterations
    p_param->set_MAX_BB_EVAL(100);






    // parameters validation:
    p_param->check();

    cout << "NOMAD Parameters:" << endl;
    cout << *p_param << endl;


    return p_param;



}

//-----------------------------------------------------------------------------------------------
// Called with the best solution of the contineous sub-problem
//-----------------------------------------------------------------------------------------------
void LshOptimizer::sendCasesToOptimizer(CaseQueue *cases)
{
    // adding the int and binary variable values to the cases
    for(int i = 0; i < cases->size(); ++i)
    {
        // binary variables
        for(int j = 0; j < p_current_values->numberOfBinaryVariables(); ++j)
        {
            cases->at(i)->addBinaryVariableValue(p_current_values->binaryVariableValue(j));
        }

        // integer variables
        for(int j = 0; j < p_current_values->numberOfIntegerVariables(); ++j)
        {
            cout << "adding int variable to case i = " << i << endl;
            cases->at(i)->addIntegerVariableValue(p_current_values->integerVariableValue(j));


        }

        cout << "case i = " << i <<  " has " << cases->at(i)->numberOfIntegerVariables() << " int variables" << endl;
    }

    // sending the cases to the optimizer
    runCases(cases);
}


//-----------------------------------------------------------------------------------------------
// Set by IPOPT through finalize_solution
//-----------------------------------------------------------------------------------------------
void LshOptimizer::setCurrentSolution(Case *c)
{
    // adding binary variable values
    for(int i = 0; i < p_current_values->numberOfBinaryVariables(); ++i)
    {
        c->addBinaryVariableValue(p_current_values->binaryVariableValue(i));
    }

    // adding integer variable values
    for(int i = 0; i < p_current_values->numberOfIntegerVariables(); ++i)
    {
        c->addIntegerVariableValue(p_current_values->integerVariableValue(i));
    }

    cout << "Getting a new best solution from IPOPT" << endl;
    c->printToCout();


    // setting it to be the current solution
    p_current_solution = c;

    // adding the current solution to the vector of solutions
    m_solutions.push_back(c);

    // checking if this is the best solution so far
    if(p_best_solution->objectiveValue() < c->objectiveValue())
    {
        p_best_solution = c;
        m_last_sol_best = true;
    }
    else m_last_sol_best = false;

}


} // namespace ResOpt
