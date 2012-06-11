#ifndef OPTIMIZER_H
#define OPTIMIZER_H


namespace ResOpt
{

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

} // namespace ResOpt


#endif // OPTIMIZER_H
