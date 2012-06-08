#ifndef OPTIMIZER_H
#define OPTIMIZER_H

class Runner;

class Optimizer
{
private:
    Runner *p_runner;

    bool m_initialized;

public:
    Optimizer(Runner *r);

    virtual void initialize() = 0;

    virtual void start() = 0;

    Runner* runner() {return p_runner;}

    void setInitialized(bool i) {m_initialized = i;}

    bool isInitialized() {return m_initialized;}
};

#endif // OPTIMIZER_H
