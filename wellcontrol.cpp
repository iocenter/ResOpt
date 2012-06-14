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

#include "wellcontrol.h"
#include "realvariable.h"

namespace ResOpt
{


WellControl::WellControl()
{
}


WellControl::WellControl(const WellControl &c)
{
    /*
    shared_ptr<RealVariable> p_control_var;        // variable containing current set point
    WellControl::contol_type m_type;    // type of well control
    double m_end_time;                      // stop time
    */

    // copying basic types
    m_type = c.m_type;
    m_end_time = c.m_end_time;

    // copying the variable

    setControlVar(shared_ptr<RealVariable>(new RealVariable(*c.p_control_var)));
}


WellControl::~WellControl()
{


}


} // namespace ResOpt
