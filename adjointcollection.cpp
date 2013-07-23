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

#include "adjointcollection.h"
#include "adjoint.h"
#include "well.h"
#include "stream.h"

#include <iostream>

using std::cout;
using std::endl;


namespace ResOpt
{


AdjointCollection::AdjointCollection()
{
}

AdjointCollection::AdjointCollection(const AdjointCollection &ac)
{
    p_var = ac.p_var;

    for(int i = 0; i < ac.m_adjoints.size(); ++i) m_adjoints.push_back(new Adjoint(*ac.m_adjoints.at(i)));
}

//-----------------------------------------------------------------------------------------------
// returns the adjoint for well w and stream s
//-----------------------------------------------------------------------------------------------
Adjoint* AdjointCollection::adjoint(Well *w, int time)
{
    for(int i = 0; i < m_adjoints.size(); ++i)
    {
        if((w->id() == m_adjoints.at(i)->well()->id()) && (time == m_adjoints.at(i)->time()))
        {
            return m_adjoints.at(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------
// perturbs the values in the streams, given change in x and d/dx
//-----------------------------------------------------------------------------------------------
bool AdjointCollection::perturbStreams(double eps_x)
{

    for(int i = 0; i < m_adjoints.size(); ++i)
    {
        // perturbing each adjoint stream, returning false if unsuccessfull.
        if(!m_adjoints.at(i)->perturbStream(eps_x)) return false;
    }

    return true;
}

} // namespace
