#ifndef INJECTIONWELL_H
#define INJECTIONWELL_H

#include "well.h"

class InjectionWell : public Well
{
public:
    InjectionWell();

    // virtual functions

    virtual void setAutomaticType() {setType(Well::INJ);}
};

#endif // INJECTIONWELL_H
