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


#ifndef ADJOINT_H
#define ADJOINT_H

#include "well.h"

namespace ResOpt
{

class Stream;
class RealVariable;


class Adjoint
{
private:
    double m_dp_dx;
    double m_dqo_dx;
    double m_dqg_dx;
    double m_dqw_dx;

    int m_time;
    Well *p_well;

public:
    Adjoint();
    Adjoint(Well *w, int time);

    // misc functions
    bool perturbStream(double eps_x);

    // set functions
    void setDpDx(double d) {m_dp_dx = d;}
    void setDqoDx(double d) {m_dqo_dx = d;}
    void setDqgDx(double d) {m_dqg_dx = d;}
    void setDqwDx(double d) {m_dqw_dx = d;}

    //void setStream(Stream *s) {p_stream = s;}
    void setWell(Well *w) {p_well = w;}


    // get functions
    double dpDx() const {return m_dp_dx;}
    double dqoDx() const {return m_dqo_dx;}
    double dqgDx() const {return m_dqg_dx;}
    double dqwDx() const {return m_dqw_dx;}

    Stream* stream() {return p_well->stream(m_time);}
    int time() {return m_time;}
    Well* well() {return p_well;}


};

} // namespace

#endif // ADJOINT_H
