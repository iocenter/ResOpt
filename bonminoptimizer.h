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


#ifndef BONMINOPTIMIZER_H
#define BONMINOPTIMIZER_H

#include "optimizer.h"


#include "CoinTime.hpp"
#include "CoinError.hpp"

#include "BonOsiTMINLPInterface.hpp"
#include "BonIpoptSolver.hpp"
#include "BonCbc.hpp"
#include "BonBonminSetup.hpp"

#include "BonOACutGenerator2.hpp"
#include "BonEcpCuts.hpp"
#include "BonOaNlpOptim.hpp"




using namespace Ipopt;
using namespace Bonmin;

namespace ResOpt
{

class Runner;
class BonminInterface;



/**
 * @brief The bonmin optimization algorithm.
 *
 */
class BonminOptimizer : public Optimizer
{
private:
 //   SmartPtr<BonminInterface> p_tminlp;
    BonminSetup m_bonmin;


public:
    BonminOptimizer(Runner *r);
    virtual ~BonminOptimizer();

    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt

#endif // BONMINOPTIMIZER_H
