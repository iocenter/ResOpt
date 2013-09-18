#ifndef NOMADOPTIMIZER_H
#define NOMADOPTIMIZER_H

#include "optimizer.h"
#include "nomad.hpp"

namespace ResOpt
{

class NomadEvaluator;


class NomadOptimizer : public Optimizer
{
private:
    NomadEvaluator *p_evaluator;
    NOMAD::Parameters *p_param;
    NOMAD::Display *p_disp;

    void generateParameters();

public:
    NomadOptimizer(Runner *r);
    virtual ~NomadOptimizer();

    virtual void initialize();

    virtual void start();

    virtual QString description() const;

};


} // namespace ResOpt

#endif // NOMADOPTIMIZER_H
