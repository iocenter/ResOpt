#ifndef COUPLEDMODEL_H
#define COUPLEDMODEL_H

#include "model.h"

namespace ResOpt
{
class ProductionWell;
class MidPipe;

class CoupledModel : public Model
{
private:

    /**
    * @brief Adds the streams flowing from this well to the upstream connected pipes
    *
    * @param w
    */
    void addStreamsUpstream(ProductionWell *w);

    /**
    * @brief Adds the streams flowing from this pipe to the upstream connected pipes (or separators)
    *
    * @param p
     */
    void addStreamsUpstream(MidPipe *p);


public:
    CoupledModel();
    CoupledModel(const CoupledModel &m);

    // virtual functions
    virtual void updateStreams();
    virtual Model* clone() const {return new CoupledModel(*this);}
};


} // namespace ResOpt

#endif // COUPLEDMODEL_H
