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


#ifndef ENDPIPE_H
#define ENDPIPE_H

#include "pipe.h"

namespace ResOpt
{


class EndPipe : public Pipe
{
private:
    double m_outletpressure;

public:
    EndPipe();

    // virtual functions

    virtual void calculateInletPressure();


    // set functions

    /**
     * @brief Sets the outlet pressure of the Pipe
     *
     * @param p
     */
    void setOutletPressure(double p) {m_outletpressure = p;}


    // get functions

    /**
     * @brief Returns the outlet pressure of the pipe
     *
     * @return double
     */
    double outletPressure() const {return m_outletpressure;}
};


} // namespace ResOpt

#endif // ENDPIPE_H
