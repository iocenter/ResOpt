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
#include "derivative.h"

namespace ResOpt
{

Case::Case()
    : m_objective_value(0),
      p_objective_derivative(0)
{
}

Case::Case(Model *m)
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



}

Case::Case(const Case &c, bool cpy_output)
    : m_objective_value(0)
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

        for(int i = 0; i < m_constraint_derivatives.size(); ++i)
        {
            m_constraint_derivatives.push_back(new Derivative(*c.m_constraint_derivatives.at(i)));
        }

        p_objective_derivative = new Derivative(*c.p_objective_derivative);


    }


}

Case::~Case()
{
    if(p_objective_derivative != 0) delete p_objective_derivative;

    for(int i = 0; i < m_constraint_derivatives.size(); ++i) delete m_constraint_derivatives.at(i);
}

} // namespace ResOpt
