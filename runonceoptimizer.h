#ifndef RUNONCEOPTIMIZER_H
#define RUNONCEOPTIMIZER_H

#include "optimizer.h"

namespace ResOpt
{

class Runner;


class RunonceOptimizer : public Optimizer
{
public:
    RunonceOptimizer(Runner *r);


    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt

#endif // RUNONCEOPTIMIZER_H
