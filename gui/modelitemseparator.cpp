/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "modelitemseparator.h"
#include "inspectorseparator.h"
#include "modelscene.h"

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


//-----------------------------------------------------------------------------------------------
// Open the inspector window if item is double clicked
//-----------------------------------------------------------------------------------------------
void ModelItemSeparator::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    InspectorSeparator *inspector = new InspectorSeparator(p_separator);

    ModelScene *m_scene = dynamic_cast<ModelScene*>(scene());
    connect(inspector, SIGNAL(sendMsg(QString)), m_scene, SIGNAL(sendMsg(QString)));

}

} // namespace
