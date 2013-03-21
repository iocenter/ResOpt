#ifndef MODELITEMPRODWELL_H
#define MODELITEMPRODWELL_H

#include "modelitem.h"

namespace ResOpt
{
class ProductionWell;
}

using ResOpt::ProductionWell;



namespace ResOptGui
{

class ModelItemProdWell : public ModelItem
{
private:
    ProductionWell *p_prod_well;

public:

    ModelItemProdWell(ProductionWell *prod, const QString &file_name = ":new/images/prod_svg", QGraphicsItem *parent = 0);


    ProductionWell* productionWell() {return p_prod_well;}


};

} // namespace

#endif // MODELITEMPRODWELL_H
