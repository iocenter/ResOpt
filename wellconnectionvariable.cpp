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


#include "wellconnectionvariable.h"


#include "intvariable.h"


namespace ResOpt
{


WellConnectionVariable::WellConnectionVariable()
{
    p_well_con = new WellConnection();
}

WellConnectionVariable::WellConnectionVariable(const WellConnectionVariable &wcv)
{
    setIVariable(shared_ptr<IntVariable>(new IntVariable(*wcv.p_var_i)));
    setJVariable(shared_ptr<IntVariable>(new IntVariable(*wcv.p_var_j)));

    p_well_con = new WellConnection(*wcv.p_well_con);

}

WellConnectionVariable::~WellConnectionVariable()
{
    delete p_well_con;
}



//-----------------------------------------------------------------------------------------------
// returning the well connection, after updating according to variable values
//-----------------------------------------------------------------------------------------------
WellConnection* WellConnectionVariable::wellConnection()
{
    // updating i
    p_well_con->setI(p_var_i->value());

    // updating j
    p_well_con->setJ(p_var_j->value());

    return p_well_con;
}

} // namespace
