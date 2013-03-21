#include "modeliteminjwell.h"

#include "injectionwell.h"


namespace ResOptGui
{

ModelItemInjWell::ModelItemInjWell(InjectionWell *inj, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_inj_well(inj)

{

    setScale(0.8);

    setToolTip("Injection well: " + p_inj_well->name());

}

} // namespace
