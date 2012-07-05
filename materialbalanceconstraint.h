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


#ifndef MATERIALBALANCECONSTRAINT_H
#define MATERIALBALANCECONSTRAINT_H

#include <tr1/memory>
#include "stream.h"

using std::tr1::shared_ptr;

namespace ResOpt
{

class InputRateVariable;
class Constraint;


/**
 * @brief The constraints asociated with an InputRateVariable in the DecoupledModel
 *
 */
class MaterialBalanceConstraint
{
private:
    Stream m_stream;   // this stream containts the rates that the input rate variable must be equal to for mass balance feasibility

    InputRateVariable *p_irv;
    shared_ptr<Constraint> p_con_oil;
    shared_ptr<Constraint> p_con_gas;
    shared_ptr<Constraint> p_con_water;


public:
    MaterialBalanceConstraint();

    // misc functions

    /**
     * @brief Updates the value of the oil, gas, and water constraint according to the values in the stream and the input rate variable.
     *
     * @return bool
     */
    bool updateConstraints();

    void emptyStream();

    // set functions
    void setStream(const Stream &s) {m_stream = s;}
    void setInputRateVariable(InputRateVariable *irv);


    // get functions
    shared_ptr<Constraint> oilConstraint() {return p_con_oil;}
    shared_ptr<Constraint> gasConstraint() {return p_con_gas;}
    shared_ptr<Constraint> waterConstraint() {return p_con_water;}

    InputRateVariable* inputRateVariable() {return p_irv;}
    const Stream& stream() const {return m_stream;}
};


} // namespace ResOpt

#endif // MATERIALBALANCECONSTRAINT_H
