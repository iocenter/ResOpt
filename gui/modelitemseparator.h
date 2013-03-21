#ifndef MODELITEMSEPARATOR_H
#define MODELITEMSEPARATOR_H

#include "modelitem.h"

namespace ResOpt
{
class Separator;
}

using ResOpt::Separator;

namespace ResOptGui
{

class ModelItemSeparator : public ModelItem
{
private:
    Separator *p_separator;

public:

    ModelItemSeparator(Separator *sep, const QString &file_name = ":new/images/sep_svg", QGraphicsItem *parent = 0);


    Separator* separator() {return p_separator;}


};

} // namespace

#endif // MODELITEMSEPARATOR_H
