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


#include "bonmininterface.h"

#include <iostream>
#include <QVector>

#include "runner.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"



using std::cout;
using std::endl;


namespace ResOpt
{

BonminInterface::BonminInterface(Runner *r)
    : p_runner(r),
      m_pertubation(0.001)
{
    m_vars_binary = p_runner->model()->binaryVariables();
    m_vars_real = p_runner->model()->realVariables();
    m_cons = p_runner->model()->constraints();
}


//-----------------------------------------------------------------------------------------------
// Returns the variable types to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_variables_types(Index n, VariableType* var_types)
{
    assert(n == (m_vars_real.size() + m_vars_binary.size()));

    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        var_types[i] = CONTINUOUS;
        n_var++;
    }

    // the binary variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        var_types[i] = BINARY;
        n_var++;
    }



    return true;
}

//-----------------------------------------------------------------------------------------------
// Returns the variable liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_variables_linearity(Index n, Ipopt::TNLP::LinearityType* var_types)
{
    // the real (contineous) variables. this includes the well control variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        var_types[i] = Ipopt::TNLP::NON_LINEAR;
        n_var++;
    }

    // the integer variables. this includes the routing vars
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        var_types[i] = Ipopt::TNLP::NON_LINEAR;
        n_var++;
    }




    return true;
}


//-----------------------------------------------------------------------------------------------
// Retruns the constraint liniarity
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_constraints_linearity(Index m, Ipopt::TNLP::LinearityType* const_types)
{
    assert(m_cons.size() == m);

    for(int i = 0; i < m_cons.size(); i++)
    {
        const_types[i] = Ipopt::TNLP::NON_LINEAR;
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the dimensions of the problem to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                          Index& nnz_h_lag, TNLP::IndexStyleEnum& index_style)
{
    cout << "Giving bonmin the dimensions of the problem..." << endl;

    n = m_vars_binary.size() + m_vars_real.size();     // number of variables
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
bool BonminInterface::get_bounds_info(Index n, Number* x_l, Number* x_u,
                             Index m, Number* g_l, Number* g_u)
{

    int n_var = 0;  // variable index used by the optimizer (first real, then int)

    // setting the real variable bounds
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        x_l[n_var] = m_vars_real.at(i)->min();  // lower bound
        x_u[n_var] = m_vars_real.at(i)->max();	// upper bound

        n_var++;
    }

    //setting integer variable bounds
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        x_l[n_var] = m_vars_binary.at(i)->min();   // lower bound
        x_u[n_var] = m_vars_binary.at(i)->max();	// upper bound

        n_var++;
    }

    // setting the constraint bounds
    for(int i = 0; i < m_cons.size(); i++)
    {
        g_l[i] = m_cons.at(i)->min();   // lower bound
        g_u[i] = m_cons.at(i)->max();   // upper bound
    }


    return true;
}

//-----------------------------------------------------------------------------------------------
// Passes the starting points
//-----------------------------------------------------------------------------------------------
bool BonminInterface::get_starting_point(Index n, bool init_x, Number* x,
                                bool init_z, Number* z_L, Number* z_U,
                                Index m, bool init_lambda,
                                Number* lambda)
{
    cout << "Giving bonmin the starting point..." << endl;

    int n_var = 0;  // variable index used by the optimizer (first real, then binary)

    // setting the real variable starting points
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        x[n_var] = m_vars_real.at(i)->value();  // current value = starting point

        n_var++;
    }

    //setting binary variable starting points
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        x[n_var] = m_vars_binary.at(i)->value();  // current value = starting point

        n_var++;
    }



    return true;
}


//-----------------------------------------------------------------------------------------------
// Calculates the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
    cout << "Evaluating the function for bonmin..." << endl;

    // checking if this is a new set of variable values
    if(newVariableValues(n, x))
    {
        // updating the variable values
        int n_var = 0;

        // real variables
        for(int i = 0; i < m_vars_real.size(); i++)
        {
            m_vars_real.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // binary variables
        for(int i = 0; i < m_vars_binary.size(); i++)
        {
            m_vars_binary.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // evaluating the model
        p_runner->evaluate();

    }

    // getting the value of the objective (negative since bonmin is doing minimization)
    obj_value = -p_runner->model()->objective()->value();


    return true;
}

//-----------------------------------------------------------------------------------------------
// Calculates the derivatives of the objective
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
    cout << "Evaluating the gradient for bonmin..." << endl;

    // first checking if gradients are allready calculated
    if(!gradientsAreUpdated(n,x))
    {
        calculateGradients(n,x);
    }

    // copying the calculated gradients to BonMin
    for(int i = 0; i < n; i++)
    {
        grad_f[i] = m_grad_f.at(i);
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the constraint values to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
    cout << "Evaluating the constraints for bonmin..." << endl;

    // checking if this is a new set of variable values
    if(newVariableValues(n, x))
    {
        // updating the variable values
        int n_var = 0;

        // real variables
        for(int i = 0; i < m_vars_real.size(); i++)
        {
            m_vars_real.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // binary variables
        for(int i = 0; i < m_vars_binary.size(); i++)
        {
            m_vars_binary.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // evaluating the model
        p_runner->evaluate();

    }

    // getting the value of the constraints
    for(int i = 0; i < m_cons.size(); i++)
    {
        g[i] = m_cons.at(i)->value();
    }

    return true;
}


//-----------------------------------------------------------------------------------------------
// Passes the Jacobian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_jac_g(Index n, const Number* x, bool new_x,
                        Index m, Index nele_jac, Index* iRow, Index *jCol,
                        Number* values)
{
    // checking if the structure of the jacobian has been set
    if (values == NULL)
    {
        cout << "Giving bonmin the structure of the jacobian..." << endl;

        int entry = 0;
        for ( int col = 0; col < n; col++ )
        {
            for ( int row = 0; row < m; row++)
            {
                iRow[entry] = row;
                jCol[entry] = col;

                entry++;
            }
        }
    }
    else    // the structure is already set, getting the values
    {
        cout << "Evaluating the jacobian for bonmin..." << endl;

        // checking if gradients are calculated
        if(!gradientsAreUpdated(n,x))
        {
            calculateGradients(n,x);
        }

        // copying gradients to BonMin
        for(int i = 0; i < nele_jac; i++)
        {
            values[i] = m_jac_g.at(i);
        }


    }

    return true;
}



//-----------------------------------------------------------------------------------------------
// Passes the Hessian to Bonmin
//-----------------------------------------------------------------------------------------------
bool BonminInterface::eval_h(Index n, const Number* x, bool new_x,
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
void BonminInterface::finalize_solution(TMINLP::SolverReturn status,
                               Index n, const Number* x, Number obj_value)
{
}

//-----------------------------------------------------------------------------------------------
// Checks if the x values are the same as the current model values for the variables
//-----------------------------------------------------------------------------------------------
bool BonminInterface::newVariableValues(Index n, const Number *x)
{
    bool x_new = false;

    // checking if the model is up to date
    if(!p_runner->isUpToDate()) return true;

    int n_var = 0;

    // checking the real variables
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        if(x[n_var] != m_vars_real.at(i)->value())
        {
            x_new = true;
            break;
        }
        n_var++;

    }

    // checking the binary variables, if nothing new was detected with the real vars
    if(!x_new)
    {
        for(int i = 0; i < m_vars_binary.size(); i++)
        {
            if(x[n_var] != m_vars_binary.at(i)->value())
            {
                x_new = true;
                break;
            }
            n_var++;

        }


    }

    return x_new;
}

//-----------------------------------------------------------------------------------------------
// Calculates the perturbated value of a real variable
//-----------------------------------------------------------------------------------------------
double BonminInterface::getPerturbationReal(shared_ptr<RealVariable> var)
{
    double span = var->max() - var->min();
    double l_slack = var->value() - var->min();
    double u_slack = var->max() - var->value();
    double x_perturbed;

    if ( u_slack >= l_slack )
    {
        x_perturbed = var->value() + m_pertubation*span;

    }
    else
    {
        x_perturbed = var->value() - m_pertubation*span;
    }

    return x_perturbed;

}

//-----------------------------------------------------------------------------------------------
// Calculates the perturbated value of a binary variable
//-----------------------------------------------------------------------------------------------
int BonminInterface::getPerturbationBinary(shared_ptr<BinaryVariable> var)
{
    double span = var->max() - var->min();
    double l_slack = var->value() - var->min();
    double u_slack = var->max() - var->value();
    double x_perturbed;

    if ( u_slack >= l_slack )
    {
        x_perturbed = var->value() + m_pertubation*span;

    }
    else
    {
        x_perturbed = var->value() - m_pertubation*span;
    }

    return x_perturbed;

}

//-----------------------------------------------------------------------------------------------
// Calculates the gradients
//-----------------------------------------------------------------------------------------------
void BonminInterface::calculateGradients(Index n, const Number *x)
{
    // checking if the gradient vectors have the correct size
    int n_grad = m_vars_binary.size() + m_vars_real.size();
    if(m_grad_f.size() != n_grad) m_grad_f = QVector<double>(n_grad);

    int n_jac = (m_vars_binary.size() + m_vars_real.size())*m_cons.size();
    if(m_jac_g.size() != n_jac) m_jac_g = QVector<double>(n_jac);

    // checking if x vectors have correct size
    if(m_x_real.size() != m_vars_real.size()) m_x_real = QVector<double>(m_vars_real.size());
    if(m_x_binary.size() != m_vars_binary.size()) m_x_binary = QVector<int>(m_vars_binary.size());


    // checking if these are new variable values
    if(newVariableValues(n, x))
    {
        // updating the variable values
        int n_var = 0;

        // real variables
        for(int i = 0; i < m_vars_real.size(); i++)
        {
            m_vars_real.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // binary variables
        for(int i = 0; i < m_vars_binary.size(); i++)
        {
            m_vars_binary.at(i)->setValue(x[n_var]);
            n_var++;
        }

        // evaluating the model
        p_runner->evaluate();

    }

    // getting the value of the objective for the base
    double f_base = -p_runner->model()->objective()->value();

    // getting the values for the constraints for the base
    QVector<double> c_base;
    for(int i = 0; i < m_cons.size(); i++) c_base.push_back(m_cons.at(i)->value());


    // starting to do pertubations on real variables
    int n_var = 0;
    for(int i = 0; i < m_vars_real.size(); i++)
    {
        shared_ptr<RealVariable> v = m_vars_real.at(i);

        // storing the base value of the variable
        double x_base = v->value();

        // updating the variable value
        v->setValue(getPerturbationReal(v));

        // evaluating the model
        p_runner->evaluate();

        // extracting the new objective value
        double f_perturbed = -p_runner->model()->objective()->value();

        // calculating the gradient of the objective
        m_grad_f.replace(n_var, (f_base - f_perturbed) / (x_base - v->value()));

        // extracting the new constraint values
        QVector<double> c_perturbed;
        for(int j = 0; j < m_cons.size(); j++) c_perturbed.push_back(m_cons.at(j)->value());

        // calculating the gradients of the constraints
        int entry = n_var*m_cons.size();
        for(int j = 0; j < m_cons.size(); j++)
        {
            double dcdx = (c_base.at(j) - c_perturbed.at(j)) / (x_base - v->value());
            m_jac_g.replace(entry, dcdx);
            entry++;


        }

        // restoring the base value of the variable
        v->setValue(x_base);

        // updating x vectors for this point
        m_x_real.replace(i, x_base);

        n_var++;

    }

    // starting to do perturbations on binary variables
    for(int i = 0; i < m_vars_binary.size(); i++)
    {
        shared_ptr<BinaryVariable> v = m_vars_binary.at(i);

        // storing the base value of the variable
        int x_base = v->value();

        // updating the variable value
        v->setValue(getPerturbationBinary(v));

        // evaluating the model
        p_runner->evaluate();

        // extracting the new objective value
        double f_perturbed = p_runner->model()->objective()->value();

        // calculating the gradient
        m_grad_f.replace(n_var, (f_base - f_perturbed) / (x_base - v->value()));

        // extracting the new constraint values
        QVector<double> c_perturbed;
        for(int j = 0; j < m_cons.size(); j++) c_perturbed.push_back(m_cons.at(j)->value());

        // calculating the gradients of the constraints
        int entry = n_var*m_cons.size();
        for(int j = 0; j < m_cons.size(); j++)
        {
            double dcdx = (c_base.at(j) - c_perturbed.at(j)) / (x_base - v->value());
            m_jac_g.replace(entry, dcdx);
            entry++;


        }

        // restoring the base value of the variable
        v->setValue(x_base);

        // updating x vectors for this point
        m_x_binary.replace(i, x_base);


        n_var++;

    }


}

//-----------------------------------------------------------------------------------------------
// Checks if the gradients are up to date
//-----------------------------------------------------------------------------------------------
bool BonminInterface::gradientsAreUpdated(Index n, const Number *x)
{
    if(m_x_real.size() == m_vars_real.size() && m_x_binary.size() == m_vars_binary.size()) //checking if the x vectors are initialized
    {
        // checking if the variable values are the same as the x vectors
        int n_var = 0;
        bool new_x = false;

        // real variables
        for(int i = 0; i < m_vars_real.size(); i++)
        {
            if(x[n_var] != m_vars_real.at(i)->value())
            {
                new_x = true;
                break;
            }
            n_var++;
        }
        if(new_x) return new_x;

        // binary variables
        for(int i = 0; i < m_vars_binary.size(); i++)
        {
            if(x[n_var] != m_vars_binary.at(i)->value())
            {
                new_x = true;
                break;
            }


            n_var++;
        }

        return new_x;

    }

    else return false;
}

} // namespace ResOpt
