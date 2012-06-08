#ifndef RUNONCEOPTIMIZER_H
#define RUNONCEOPTIMIZER_H

#include "optimizer.h"

class Runner;

class RunonceOptimizer : public Optimizer
{
public:
    RunonceOptimizer(Runner *r);


    virtual void initialize();

    virtual void start();

};

#endif // RUNONCEOPTIMIZER_H
