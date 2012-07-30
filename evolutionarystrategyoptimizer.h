#ifndef EVOLUTIONARYSTRATEGYOPTIMIZER_H
#define EVOLUTIONARYSTRATEGYOPTIMIZER_H

#include "optimizer.h"
#include <QVector>

namespace ResOpt
{

class Runner;
class Case;

class EvolutionaryStrategyOptimizer : public Optimizer
{
private:
    int n_parents;
    int n_children;
    double mut_range;
    double destab_range;
    double mut_precision;
    bool consider_past;

    QVector<Case*> m_parents;
    QVector<Case*> m_children;

    void mutate(Case *c, bool destabilize = false);
    void getChildren();
    Case* randomChild();


public:
    EvolutionaryStrategyOptimizer(Runner *r);

    virtual void initialize();

    virtual void start();

};

} // namespace ResOpt

#endif // EVOLUTIONARYSTRATEGYOPTIMIZER_H
