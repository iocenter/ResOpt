#include "modelitemprodwell.h"

#include "productionwell.h"

namespace ResOptGui
{

ModelItemProdWell::ModelItemProdWell(ProductionWell *prod, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_prod_well(prod)

{

    setScale(0.8);

    setToolTip("Production well: " + p_prod_well->name());

}

} // namespace
