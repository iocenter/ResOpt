#ifndef IPOPTOPTIMIZER_H
#define IPOPTOPTIMIZER_H

#include "optimizer.h"
//#include "IpIpoptApplication.hpp"
//#include "IpSolveStatistics.hpp"

//using namespace Ipopt;

namespace ResOpt
{

class Runner;
class IpoptInterface;
class Case;

/**
 * @brief The Ipopt optimization algorithm.
 *
 */
class IpoptOptimizer : public Optimizer
{
private:

    //SmartPtr<IpoptInterface> p_tnlp;
    //SmartPtr<IpoptApplication> app;
    //IpoptSetup m_ipopt;
    Case* generateStartingPoint();


public:
    IpoptOptimizer(Runner *r);
    virtual ~IpoptOptimizer();

    virtual void initialize();

    virtual void start();

    virtual QString description() const;

};

} // namespace ResOpt


#endif // IPOPTOPTIMIZER_H
