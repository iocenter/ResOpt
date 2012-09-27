#ifndef MEOPTIMIZER_H
#define MEOPTIMIZER_H
#include "optimizer.h"

namespace ResOpt
{

class MeOptimizer : public Optimizer
{

private:
    int m_installationtime;

public:
    MeOptimizer(Runner *r);
    virtual ~MeOptimizer();


    virtual void initialize();

    virtual void start();


    int installTime() {return m_installationtime;}
    void setInstallTime(int i) {m_installationtime = i;}
};
} // end namespace
#endif // MEOPTIMIZER_H
