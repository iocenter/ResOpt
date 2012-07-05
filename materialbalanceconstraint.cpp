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


#include "materialbalanceconstraint.h"
#include "constraint.h"
#include "inputratevariable.h"
#include "realvariable.h"
#include "pipe.h"


namespace ResOpt
{

MaterialBalanceConstraint::MaterialBalanceConstraint()
    : p_irv(0)
{
    p_con_oil = shared_ptr<Constraint>(new Constraint(0,0,0));
    p_con_gas = shared_ptr<Constraint>(new Constraint(0,0,0));
    p_con_water = shared_ptr<Constraint>(new Constraint(0,0,0));
}

//-----------------------------------------------------------------------------------------------
// empties the stream
//-----------------------------------------------------------------------------------------------
void MaterialBalanceConstraint::emptyStream()
{
    m_stream.setGasRate(0.0);
    m_stream.setOilRate(0.0);
    m_stream.setWaterRate(0.0);
}

//-----------------------------------------------------------------------------------------------
// sets the input rate variable, and assignes names to the constraints
//-----------------------------------------------------------------------------------------------
void MaterialBalanceConstraint::setInputRateVariable(InputRateVariable *irv)
{
    p_irv = irv;

    // updating the constraint names
    Pipe *p = irv->pipe();

    if(p != 0)
    {
        p_con_oil->setName("Material balance constraint for oil entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
        p_con_gas->setName("Material balance constraint for gas entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
        p_con_water->setName("Material balance constraint for water entering Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p_irv->stream()->time()));
    }
}

//-----------------------------------------------------------------------------------------------
// updates the value of the constraints
//-----------------------------------------------------------------------------------------------
bool MaterialBalanceConstraint::updateConstraints()
{
    // checking that the input rate constraint has been set
    if(p_irv == 0) return false;

    // updating the values of the constraints
    p_con_oil->setValue(m_stream.oilRate() - p_irv->oilVariable()->value());
    p_con_gas->setValue(m_stream.gasRate() - p_irv->gasVariable()->value());
    p_con_water->setValue(m_stream.waterRate() - p_irv->waterVariable()->value());

    return true;

}


} // namespace ResOpt


