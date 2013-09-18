#ifndef NOMADEVALUATOR_H
#define NOMADEVALUATOR_H

#include "nomad.hpp"

namespace ResOpt
{
class NomadOptimizer;
class Case;

class NomadEvaluator : public NOMAD::Evaluator
{
private:
    NomadOptimizer *p_optimizer;



public:
    NomadEvaluator(const NOMAD::Parameters &p, NomadOptimizer *o);

    bool eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const;

    Case* generateCase(const NOMAD::Eval_Point &x) const;
};

} // namespace ResOpt

#endif // NOMADEVALUATOR_H
