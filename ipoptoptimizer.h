#ifndef IPOPTOPTIMIZER_H
#define IPOPTOPTIMIZER_H

#include "optimizer.h"
#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"

using namespace Ipopt;

namespace ResOpt
{

class Runner;
class IpoptInterface;

/**
 * @brief The Ipopt optimization algorithm.
 *
 */
class IpoptOptimizer : public Optimizer
{
private:

    SmartPtr<IpoptInterface> p_tnlp;
    SmartPtr<IpoptApplication> app;
    //IpoptSetup m_ipopt;

public:
    IpoptOptimizer(Runner *r);
    virtual ~IpoptOptimizer();

    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt


#endif // IPOPTOPTIMIZER_H
