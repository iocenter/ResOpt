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


#include "runonceoptimizer.h"

#include <iostream>
#include "runner.h"



using std::cout;
using std::endl;

namespace ResOpt
{


RunonceOptimizer::RunonceOptimizer(Runner *r)
    : Optimizer(r)
{
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::initialize()
{
    // nothing to do here

    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::start()
{
    cout << "Starting RUN_ONCE..." << endl;

    // launching the runner for one iteration
    runner()->evaluate();

    // letting the runner know that the optimization has finished
    emit finished();
}


} // namespace ResOpt
