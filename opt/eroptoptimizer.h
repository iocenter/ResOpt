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


#ifndef EROPTOPTIMIZER_H
#define EROPTOPTIMIZER_H

#include "optimizer.h"

#include <QList>

namespace ResOpt
{

class Runner;
class MINLPEvaluator;

/**
 * @brief Runs the project once with the starting point values for the variables.
 *
 */
class EroptOptimizer : public Optimizer
{
private:
    enum direction{UP, DOWN};

    MINLPEvaluator *p_evaluator;
    QList<direction> m_directions;


    Case* generateBaseCase();
    void perturbVariable(Case *c, int i_var, direction move_direction, int step = 1);
    void setInitialDirections();

    bool isBetter(Case *c, Case *base_case);
    direction switchDirection(direction d);

    Case* solve(Case *base_case, int start_var, bool *converged);


public:
    EroptOptimizer(Runner *r);
    virtual ~EroptOptimizer();


    virtual void initialize();

    virtual void start();

    virtual QString description() const;

};

} // namespace ResOpt

#endif // EROPTOPTIMIZER_H
