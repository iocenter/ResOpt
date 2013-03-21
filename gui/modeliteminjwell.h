#ifndef MODELITEMINJWELL_H
#define MODELITEMINJWELL_H

#include "modelitem.h"

namespace ResOpt
{
class InjectionWell;
}

using ResOpt::InjectionWell;



namespace ResOptGui
{

class ModelItemInjWell : public ModelItem
{
private:
    InjectionWell *p_inj_well;

public:

    ModelItemInjWell(InjectionWell *inj, const QString &file_name = ":new/images/inj_svg", QGraphicsItem *parent = 0);


    InjectionWell* injectionWell() {return p_inj_well;}


};

} // namespace

#endif // MODELITEMINJWELL_H
