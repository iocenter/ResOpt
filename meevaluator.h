#ifndef MEEVALUATOR_H
#define MEEVALUATOR_H

#include "nomad.hpp"

namespace ResOpt
{
class MeOptimizer;
class Case;

class MeEvaluator : public NOMAD::Evaluator
{
private:
    MeOptimizer *p_optimizer;

    int numberOfProductionWells();


public:
    MeEvaluator(const NOMAD::Parameters &p, MeOptimizer *o);

    bool eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const;

    Case* generateCase(const NOMAD::Eval_Point &x) const;
};

} // namespace ResOpt

#endif // MEEVALUATOR_H
