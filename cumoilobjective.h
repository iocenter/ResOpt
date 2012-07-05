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

#ifndef CUMOILOBJECTIVE_H
#define CUMOILOBJECTIVE_H

#include "objective.h"


namespace ResOpt
{

/**
 * @brief Class for objectives that maximize oil cumulative production
 *
 */
class CumoilObjective : public Objective
{
public:
/**
 * @brief
 *
 */
    CumoilObjective();

    // virtuals

    virtual Objective* clone() {return new CumoilObjective(*this);}

    /**
     * @brief Calculates the cumulative oil produced from the input streams
     *
     * @param s
     */
    virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c);

};

} // namespace ResOpt

#endif // CUMOILOBJECTIVE_H
