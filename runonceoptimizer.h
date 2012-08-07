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


#ifndef RUNONCEOPTIMIZER_H
#define RUNONCEOPTIMIZER_H

#include "optimizer.h"

namespace ResOpt
{

class Runner;
class Pipe;


/**
 * @brief Runs the project once with the starting point values for the variables.
 *
 */
class RunonceOptimizer : public Optimizer
{
private:

    Case* buildCase(Pipe *p);

public:
    RunonceOptimizer(Runner *r);
    virtual ~RunonceOptimizer();


    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt

#endif // RUNONCEOPTIMIZER_H
