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


#include "adjoint.h"
#include "stream.h"


namespace ResOpt
{

Adjoint::Adjoint() :
    m_dp_dx(0),
    m_dqo_dx(0),
    m_dqg_dx(0),
    m_dqw_dx(0),
    p_stream(0),
    p_well(0)
{
}

Adjoint::Adjoint(Well *w, Stream *s) :
    m_dp_dx(0),
    m_dqo_dx(0),
    m_dqg_dx(0),
    m_dqw_dx(0),
    p_stream(s),
    p_well(w)
{
}


//-----------------------------------------------------------------------------------------------
// perturbs the values in the stream, given change in x and d/dx
//-----------------------------------------------------------------------------------------------
bool Adjoint::perturbStream(double eps_x)
{
    if(p_stream != 0)
    {
        double q_o = p_stream->oilRate(true) + m_dqo_dx*eps_x;
        double q_g = p_stream->gasRate(true) + m_dqg_dx*eps_x;
        double q_w = p_stream->waterRate(true) + m_dqw_dx*eps_x;
        double p = p_stream->pressure(true) + m_dp_dx*eps_x;

        p_stream->setOilRate(q_o);
        p_stream->setGasRate(q_g);
        p_stream->setWaterRate(q_w);
        p_stream->setPressure(p);

        return true;
    }

    else return false;
}



} // namespace
