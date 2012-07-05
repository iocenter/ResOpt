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


#include "inputratevariable.h"

#include "stream.h"
#include "realvariable.h"

namespace ResOpt
{

InputRateVariable::InputRateVariable()
    : p_stream(0)
{
}


//-----------------------------------------------------------------------------------------------
// updates the stream with the current values in the variables
//-----------------------------------------------------------------------------------------------
bool InputRateVariable::updateStream()
{
    // checking that the stream and varaibles are defined
    if(p_stream == 0 || p_var_oil == 0 || p_var_gas == 0 || p_var_water == 0) return false;

    // everything is set up ok, updating the stream
    p_stream->setOilRate(p_var_oil->value());
    p_stream->setGasRate(p_var_gas->value());
    p_stream->setWaterRate(p_var_water->value());

    return true;
}


} // namespace ResOpt
