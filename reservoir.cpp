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

#include "reservoir.h"

namespace ResOpt
{


Reservoir::Reservoir()
    : m_use_mrst_script(false),
      m_keep_mat_file(false),
      m_gas_phase(false),
      m_oil_phase(false),
      m_wat_phase(false),
      m_den_gas(0.0),
      m_den_oil(0.0),
      m_den_wat(0.0)
{
}

//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString Reservoir::description() const
{
    QString str("START RESERVOIR\n");
    str.append(" NAME " + name() + "\n");
    str.append(" FILE " + file() + "\n");
    str.append(" MRST " + mrstPath() + "\n");
    str.append(" MATLAB " + matlabPath() + "\n");
    str.append(" SCRIPT " + mrstScript() + "\n");

    str.append(" TIME " + QString::number(endTime()) + "\n");

    str.append(" PHASES " + QString::number(m_gas_phase) + " " +
               QString::number(m_oil_phase) + " " +
               QString::number(m_wat_phase) + "\n");

    str.append(" DENS " + QString::number(m_den_gas) + " " +
               QString::number(m_den_oil) + " " +
               QString::number(m_den_wat) + "\n");

    str.append("END RESERVOIR\n\n");

    return str;

}

/*
!----------------- Reservoir Definition ----------------------------
START RESERVOIR
 NAME RES1
 FILE SIMPLE10x5x10.DATA
 TIME 3650
 PHASES 0 1 1             ! gas oil water
 DENS 0.06054 49.1 64.79  ! gas oil water
END RESERVOIR

 */


} // namespace ResOpt
