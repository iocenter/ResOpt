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


#include "case.h"
#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "intvariable.h"
#include "constraint.h"
#include "objective.h"
#include "derivative.h"

#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{

Case::Case()
    : m_objective_value(0),
      p_objective_derivative(0)
{
}

Case::Case(Model *m, bool cpy_output)
    : m_objective_value(0),
      p_objective_derivative(0)
{
    // adding real variables
    for(int i = 0; i < m->realVariables().size(); ++i)
    {
        addRealVariableValue(m->realVariables().at(i)->value());
    }

    // adding binary variables
    for(int i = 0; i < m->binaryVariables().size(); ++i)
    {
        addBinaryVariableValue(m->binaryVariables().at(i)->value());
    }

    // adding integer variables
    for(int i = 0; i < m->integerVariables().size(); ++i)
    {
        addIntegerVariableValue(m->integerVariables().at(i)->value());
    }

    if(cpy_output)
    {
        // adding constraint values
        for(int i = 0; i < m->constraints().size(); ++i)
        {
            addConstraintValue(m->constraints().at(i)->value());
        }

        // adding objective value
        setObjectiveValue(m->objective()->value());
    }



}

Case::Case(const Case &c, bool cpy_output)
    : m_objective_value(0),
      p_objective_derivative(0)
{
    for(int i = 0; i < c.numberOfRealVariables(); ++i)
    {
        m_real_var_values.push_back(c.m_real_var_values.at(i));
    }

    for(int i = 0; i < c.numberOfBinaryVariables(); ++i)
    {
        m_binary_var_values.push_back(c.m_binary_var_values.at(i));
    }

    for(int i = 0; i < c.numberOfIntegerVariables(); ++i)
    {
        m_integer_var_values.push_back(c.m_integer_var_values.at(i));
    }



    if(cpy_output)  // copy the constraints and objective too
    {
        for(int i = 0; i < c.numberOfConstraints(); ++i)
        {
            m_constraint_values.push_back(c.m_constraint_values.at(i));
        }

        m_objective_value = c.m_objective_value;

        for(int i = 0; i < c.m_constraint_derivatives.size(); ++i)
        {
            m_constraint_derivatives.push_back(new Derivative(*c.m_constraint_derivatives.at(i)));
        }

        if(c.p_objective_derivative != 0) p_objective_derivative = new Derivative(*c.p_objective_derivative);


    }


}

Case::~Case()
{

    if(p_objective_derivative != 0) delete p_objective_derivative;

    for(int i = 0; i < m_constraint_derivatives.size(); ++i) delete m_constraint_derivatives.at(i);


}

//-----------------------------------------------------------------------------------------------
// Assignment operator
//-----------------------------------------------------------------------------------------------
Case& Case::operator =(const Case &rhs)
{
    if(this != &rhs)
    {
        m_real_var_values = rhs.m_real_var_values;
        m_binary_var_values = rhs.m_binary_var_values;
        m_integer_var_values = rhs.m_integer_var_values;

        m_constraint_values = rhs.m_constraint_values;
        m_objective_value = rhs.m_objective_value;

        for(int i = 0; i < m_constraint_derivatives.size(); ++i) delete m_constraint_derivatives.at(i);
        m_constraint_derivatives.resize(0);


        for(int i = 0; i < rhs.m_constraint_derivatives.size(); ++i)
        {
            m_constraint_derivatives.push_back(new Derivative(*rhs.m_constraint_derivatives.at(i)));
        }

        if(p_objective_derivative != 0)
        {
            delete p_objective_derivative;
            p_objective_derivative = 0;
        }

        if(rhs.p_objective_derivative != 0) p_objective_derivative =  new Derivative(*rhs.p_objective_derivative);

    }

    return *this;
}

//-----------------------------------------------------------------------------------------------
// prints to the standard output
//-----------------------------------------------------------------------------------------------
void Case::printToCout()
{
    cout << "---------- CASE DATA -----------" << endl;
    cout << "OBJ =" << objectiveValue() << endl;

    for(int i = 0; i < numberOfRealVariables(); ++i)
    {
        cout << "VAR_C_" << i +1 << " = " << realVariableValue(i) << endl;
    }

    for(int i = 0; i < numberOfBinaryVariables(); ++i)
    {
        cout << "VAR_B_" << i +1 << " = " << binaryVariableValue(i) << endl;
    }

    for(int i = 0; i < numberOfIntegerVariables(); ++i)
    {
        cout << "VAR_I_" << i +1 << " = " << integerVariableValue(i) << endl;
    }

    for(int i = 0; i < numberOfConstraints(); ++i)
    {
        cout << "CON_" << i +1 << " = " << constraintValue(i) << endl;
    }

    cout << endl;

    cout << "OBJ DERIVATIVES:" << endl;
    for(int i = 0; i < objectiveDerivative()->numberOfPartials(); ++i)
    {
        cout << i << " = " << objectiveDerivative()->value(i) << endl;
    }
}


} // namespace ResOpt
