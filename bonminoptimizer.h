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


class BonminOptimizer : public Optimizer
{
private:

    SmartPtr<BonminInterface> p_tminlp;
    BonminSetup m_bonmin;


public:
    BonminOptimizer(Runner *r);

    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt

#endif // BONMINOPTIMIZER_H
