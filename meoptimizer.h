#ifndef MEOPTIMIZER_H
#define MEOPTIMIZER_H
#include "optimizer.h"

namespace ResOpt
{

class MeOptimizer : public Optimizer
{
public:
    MeOptimizer(Runner *r);
    virtual ~MeOptimizer();


    virtual void initialize();

    virtual void start();
};
} // end namespace
#endif // MEOPTIMIZER_H
