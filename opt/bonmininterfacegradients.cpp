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


#include "bonmininterfacegradients.h"

#include <iostream>
#include <QVector>

#include "bonminoptimizer.h"
#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "case.h"
#include "casequeue.h"
#include "derivative.h"



using std::cout;
using std::endl;


namespace ResOpt
{

BonminInterfaceGradients::BonminInterfaceGradients(BonminOptimizer *o)
    : p_optimizer(o),
      p_case_last(0)

{
    m_vars_binary = p_optimizer->runner()->model()->binaryVariables();
    m_vars_real = p_optimizer->runner()->model()->realVariables();
    m_vars_integer = p_optimizer->runner()->model()->integerVariables();
    m_cons = p_optimizer->runner()->model()->constraints();
}

BonminInterfaceGradients::~BonminInterfaceGradients()
{
    if(p_case_last != 0) delete p_case_last;


}

//-----------------------------------------------------------------------------------------------
// Returns the variable types to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_variables_types(Index n, VariableType* var_types)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size() + m_vars_integer.size()));

    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        var_types[n_var] = CONTINUOUS;
        ++n_var;
    }

    // the binary variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        var_types[n_var] = BINARY;
        ++n_var;
    }

    // the integer variables. this includes separator install times
    for(int i = 0; i < m_vars_integer.size(); ++i)
    {
        var_types[n_var] = INTEGER;
        ++n_var;
    }




    return true;
}

//-----------------------------------------------------------------------------------------------
// Returns the variable liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size() + m_vars_integer.size()));

    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        var_types[n_var] = Ipopt::TNLP::NON_LINEAR;
        ++n_var;
    }

    // the binary variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        var_types[n_var] = Ipopt::TNLP::NON_LINEAR;
        ++n_var;
    }

    // the integer variables. this includes the separator install times
    for(int i = 0; i < m_vars_integer.size(); ++i)
    {
        var_types[n_var] = Ipopt::TNLP::NON_LINEAR;
        ++n_var;
    }





    return true;
}


//-----------------------------------------------------------------------------------------------
// Retruns the constraint liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_constraints_linearity(Index m, Ipopt::TNLP::LinearityType* const_types)
{
    assert(m_cons.size() == m);

    for(int i = 0; i < m_cons.size(); ++i)
    {
        const_types[i] = Ipopt::TNLP::NON_LINEAR;
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the dimensions of the problem to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                          Index& nnz_h_lag, TNLP::IndexStyleEnum& index_style)
{
    cout << "Giving bonmin the dimensions of the problem..." << endl;

    n = m_vars_binary.size() + m_vars_real.size() + m_vars_integer.size();     // number of variables
    m = m_cons.size();                              // number of constraints

    nnz_jac_g = m*n;        // dense Jacobian
    nnz_h_lag = n*(n+1)/2;  // dense, symmetric Hessian

    index_style = TNLP::C_STYLE;    // c style numbering, starting at 0

    cout << "Number of variables   = " << n << endl;
    cout << "Number of constraints = " << m << endl;


    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes variable and constraint bounds
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_bounds_info(Index n, Number* x_l, Number* x_u,
                             Index m, Number* g_l, Number* g_u)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size() + m_vars_integer.size()));
    assert(m == m_cons.size());

    int n_var = 0;  // variable index used by the optimizer (first real, then binary, then int)

    // setting the real variable bounds
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        x_l[n_var] = m_vars_real.at(i)->min();  // lower bound
        x_u[n_var] = m_vars_real.at(i)->max();	// upper bound

        ++n_var;
    }

    //setting binary variable bounds
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        x_l[n_var] = m_vars_binary.at(i)->min();   // lower bound
        x_u[n_var] = m_vars_binary.at(i)->max();	// upper bound

        ++n_var;
    }

    //setting integer variable bounds
    for(int i = 0; i < m_vars_integer.size(); ++i)
    {
        x_l[n_var] = m_vars_integer.at(i)->min();   // lower bound
        x_u[n_var] = m_vars_integer.at(i)->max();	// upper bound

        ++n_var;
    }

    // setting the constraint bounds
    for(int i = 0; i < m_cons.size(); ++i)
    {
        g_l[i] = m_cons.at(i)->min();   // lower bound
        g_u[i] = m_cons.at(i)->max();   // upper bound
    }


    return true;
}

//-----------------------------------------------------------------------------------------------
// Passes the starting points
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::get_starting_point(Index n, bool init_x, Number* x,
                                bool init_z, Number* z_L, Number* z_U,
                                Index m, bool init_lambda,
                                Number* lambda)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size() + m_vars_integer.size()));

    // only expect to initialize x
    assert(init_x);
    assert(!init_z);
    assert(!init_lambda);


    cout << "Giving bonmin the starting point..." << endl;

    int n_var = 0;  // variable index used by the optimizer (first real, then binary, then int)

    // setting the real variable starting points
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        x[n_var] = m_vars_real.at(i)->value();  // current value = starting point

        ++n_var;
    }

    //setting binary variable starting points
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        x[n_var] = m_vars_binary.at(i)->value();  // current value = starting point

        ++n_var;
    }

    //setting integer variable starting points
    for(int i = 0; i < m_vars_integer.size(); ++i)
    {
        x[n_var] = m_vars_integer.at(i)->value();  // current value = starting point

        ++n_var;
    }




    return true;
}


//-----------------------------------------------------------------------------------------------
// Calculates the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{

    // running model if needed
    update(n, x);


    // getting the value of the objective (negative since bonmin is doing minimization)
    obj_value = -p_case_last->objectiveValue();


    return true;
}

//-----------------------------------------------------------------------------------------------
// Calculates the derivatives of the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
    // running model if needed
    update(n, x);


    // ---- copying the gradients from the case to grad_f ----
    // checking that gradients for all variables are available
    Derivative *df = p_case_last->objectiveDerivative();

    if(df->numberOfPartials() == n)
    {
        for(int i = 0; i < n; ++i)
        {
            grad_f[i] = df->value(i);
        }

    }
    else
    {
        cout << endl << "### BONMIN Error! ###" << endl
             << "Number of df/dx in case does not match problem size..." << endl;

        exit(1);
    }


    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the constraint values to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
    // running model if needed
    update(n, x);

    // checking that the number of constraints in the cas corresponds to m
    assert(m == p_case_last->numberOfConstraints());

    // getting the value of the constraints
    for(int i = 0; i < p_case_last->numberOfConstraints(); ++i)
    {
        g[i] = p_case_last->constraintValue(i);

    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the Jacobian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::eval_jac_g(Index n, const Number* x, bool new_x,
                        Index m, Index nele_jac, Index* iRow, Index *jCol,
                        Number* values)
{
    // checking if the structure of the jacobian has been set
    if (values == NULL)
    {
        cout << "Giving bonmin the structure of the jacobian..." << endl;

        int entry = 0;

        for(int row = 0; row < m; ++row)
        {
            for(int col = 0; col < n; ++col)
            {
                iRow[entry] = row;
                jCol[entry] = col;

                ++entry;
            }
        }
    }
    else    // the structure is already set, getting the values
    {

        // running model if needed
        update(n, x);


        // copying gradients to Bonmin

        int entry = 0;
        for(int i = 0; i < p_case_last->numberOfConstraintDerivatives(); ++i)
        {
            Derivative *dc = p_case_last->constraintDerivative(i);
            for(int j = 0; j < dc->numberOfPartials(); ++j)
            {
                values[entry] = dc->value(j);

                ++entry;
            }
        }


    }

    return true;
}



//-----------------------------------------------------------------------------------------------
// Passes the Hessian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::eval_h(Index n, const Number* x, bool new_x,
                    Number obj_factor, Index m, const Number* lambda,
                    bool new_lambda, Index nele_hess, Index* iRow,
                    Index* jCol, Number* values)
{
    cout << "Evaluating the hessian for bonmin..." << endl;
    return true;
}


//-----------------------------------------------------------------------------------------------
// Called by Ipopt when finished
//-----------------------------------------------------------------------------------------------
void BonminInterfaceGradients::finalize_solution(TMINLP::SolverReturn status,
                               Index n, const Number* x, Number obj_value)
{


    cout << "IPOPT returned a status indicating ";

    if ( status == Ipopt::SUCCESS ) cout << "SUCCESS";
    else if ( status == Ipopt::MAXITER_EXCEEDED ) cout << "MAXITER_EXCEEDED";
    else if ( status == Ipopt::CPUTIME_EXCEEDED ) cout << "CPUTIME_EXCEEDED";
    else if ( status == Ipopt::STOP_AT_TINY_STEP ) cout << "STOP_AT_TINY_STEP";
    else if ( status == Ipopt::STOP_AT_ACCEPTABLE_POINT ) cout << "STOP_AT_ACCEPTABLE_POINT";
    else if ( status == Ipopt::LOCAL_INFEASIBILITY ) cout << "LOCAL_INFEASIBILITY";
    else if ( status == Ipopt::USER_REQUESTED_STOP ) cout << "USER_REQUESTED_STOP";
    else if ( status == Ipopt::FEASIBLE_POINT_FOUND ) cout << "FEASIBLE_POINT_FOUND";
    else if ( status == Ipopt::DIVERGING_ITERATES ) cout << "DIVERGING_ITERATES";
    else if ( status == Ipopt::RESTORATION_FAILURE ) cout << "RESTORATION_FAILURE";
    else if ( status == Ipopt::ERROR_IN_STEP_COMPUTATION ) cout << "ERROR_IN_STEP_COMPUTATION";
    else if ( status == Ipopt::INVALID_NUMBER_DETECTED ) cout << "INVALID_NUMBER_DETECTED";
    else if ( status == Ipopt::TOO_FEW_DEGREES_OF_FREEDOM ) cout << "TOO_FEW_DEGREES_OF_FREEDOM";
    else if ( status == Ipopt::INVALID_OPTION ) cout << "INVALID_OPTION";
    else if ( status == Ipopt::OUT_OF_MEMORY ) cout << "OUT_OF_MEMORY";
    else if ( status == Ipopt::INTERNAL_ERROR ) cout << "INTERNAL_ERROR";
    else cout << "UNKNOWN_ERROR";

    cout << endl;

    cout << "Writing the final solution to the summary file..." << endl;
    Case *c = generateCase(n,x);
    c->setObjectiveValue(-obj_value);

    p_optimizer->sendBestCaseToRunner(c);

}


//-----------------------------------------------------------------------------------------------
// Checks if the x values are the same as the current model values for the variables
//-----------------------------------------------------------------------------------------------
bool BonminInterfaceGradients::newVariableValues(Index n, const Number *x)
{
    bool x_new = false;

    // checking if the last case has been set up
    if(p_case_last == 0) return true;

    // checking that the number of variables in the case correspons to n
    int n_var_case = p_case_last->numberOfRealVariables() + p_case_last->numberOfBinaryVariables() + p_case_last->numberOfIntegerVariables();
    if(n != n_var_case) return false;

    // checking if the real variables are the same
    int n_var = 0;

    for(int i = 0; i < p_case_last->numberOfRealVariables(); ++i)
    {
        if(x[n_var] != p_case_last->realVariableValue(i))
        {
            x_new = true;
            break;
        }

        ++n_var;
    }

    // checking the binary variables, if nothing new was detected with the real vars
    if(!x_new)
    {
        for(int i = 0; i < p_case_last->numberOfBinaryVariables(); ++i)
        {
            if(x[n_var] != p_case_last->binaryVariableValue(i))
            {
                x_new = true;
                break;
            }

            ++n_var;
        }
    }

    // checking the integer variables, if nothing new was detected with the real or binary vars
    if(!x_new)
    {
        for(int i = 0; i < p_case_last->numberOfIntegerVariables(); ++i)
        {
            if(x[n_var] != p_case_last->integerVariableValue(i))
            {
                x_new = true;
                break;
            }

            ++n_var;
        }
    }



    return x_new;
}






//-----------------------------------------------------------------------------------------------
// Runs new case with the variable values if needed
//-----------------------------------------------------------------------------------------------
void BonminInterfaceGradients::update(Index n, const Number *x)
{
    // checking if this is a new set of variable values
    if(newVariableValues(n, x))
    {
        // deleting the old case
        if(p_case_last != 0) delete p_case_last;
        p_case_last = 0;

        // creating a new case
        Case *case_new = generateCase(n, x);

        // sending the case to the runner
        p_optimizer->runCase(case_new);

        // setting the case as the last case
        p_case_last = case_new;

    }


}

//-----------------------------------------------------------------------------------------------
// Generates a Case based on the values in x
//-----------------------------------------------------------------------------------------------
Case* BonminInterfaceGradients::generateCase(Index n, const Number *x)
{
    // checking that the problem has the right dimensions
    assert(n == (m_vars_real.size() + m_vars_binary.size() + m_vars_integer.size()));

    // creating a new case
    Case *case_new = new Case();

    // updating the variable values
    int n_var = 0;

    // real variables
    for(int i = 0; i < m_vars_real.size(); ++i)
    {
        case_new->addRealVariableValue(x[n_var]);
        ++n_var;
    }

    // binary variables
    for(int i = 0; i < m_vars_binary.size(); ++i)
    {
        case_new->addBinaryVariableValue(x[n_var]);
        ++n_var;
    }

    // integer variables
    for(int i = 0; i < m_vars_integer.size(); ++i)
    {
        case_new->addIntegerVariableValue(x[n_var]);
        ++n_var;
    }


    return case_new;


}

} // namespace ResOpt
