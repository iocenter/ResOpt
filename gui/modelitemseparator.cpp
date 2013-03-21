#include "modelitemseparator.h"

#include "separator.h"

namespace ResOptGui
{

ModelItemSeparator::ModelItemSeparator(Separator *sep, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_separator(sep)

{

    setScale(0.6);

    setToolTip("Separator: " + QString::number(p_separator->number()));

}

} // namespace
