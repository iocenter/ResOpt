#ifndef ENDPIPE_H
#define ENDPIPE_H

#include "pipe.h"

namespace ResOpt
{


class EndPipe : public Pipe
{
private:
    double m_outletpressure;

public:
    EndPipe();

    // virtual functions

    virtual void calculateInletPressure();


    // set functions

    /**
     * @brief Sets the outlet pressure of the Pipe
     *
     * @param p
     */
    void setOutletPressure(double p) {m_outletpressure = p;}


    // get functions

    /**
     * @brief Returns the outlet pressure of the pipe
     *
     * @return double
     */
    double outletPressure() {return m_outletpressure;}
};


} // namespace ResOpt

#endif // ENDPIPE_H
