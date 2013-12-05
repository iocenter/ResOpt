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


#include "masteroptimizer.h"
#include "masterrunner.h"
#include "case.h"
#include "runner.h"
#include "model.h"
#include "intvariable.h"
#include "binaryvariable.h"

#include <iostream>

using std::cout;
using std::endl;


namespace ResOpt
{


MasterOptimizer::MasterOptimizer(MasterRunner *mr) :
    p_master_runner(mr)
{
}


//-----------------------------------------------------------------------------------------------
// initializes the optimizer
//-----------------------------------------------------------------------------------------------
void MasterOptimizer::initialize()
{}


//-----------------------------------------------------------------------------------------------
// start the optimization
//-----------------------------------------------------------------------------------------------
void MasterOptimizer::start()
{

    cout << "#################################" << endl;
    cout << "###    Starting MasterOpt     ###" << endl;
    cout << "###                           ###" << endl;
    cout << "#################################" << endl;


}


//-----------------------------------------------------------------------------------------------
// generates a case with the starting point values for int and bin variables
//-----------------------------------------------------------------------------------------------
Case* MasterOptimizer::generateBaseCase()
{
    Case *c = new Case();

    Model *m = p_master_runner->runner(0)->model();

    // integer variables
    for(int i = 0; i < m->integerVariables().size(); ++i)
    {
        c->addIntegerVariableValue(m->integerVariableValue(i));
    }

    // binary variables
    for(int i = 0; i < m->binaryVariables().size(); ++i)
    {
        c->addBinaryVariableValue(m->binaryVariableValue(i));
    }

    return c;
}

} // namespace ResOpt
