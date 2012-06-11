#ifndef INJECTIONWELL_H
#define INJECTIONWELL_H

#include "well.h"

namespace ResOpt
{


class InjectionWell : public Well
{
public:
    InjectionWell();

    // virtual functions

    virtual void setAutomaticType() {setType(Well::INJ);}
};

} // namespace ResOpt

#endif // INJECTIONWELL_H
