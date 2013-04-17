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


#include "modelitemendpipe.h"
#include "inspectorendpipe.h"
#include "modelscene.h"

#include "endpipe.h"

namespace ResOptGui
{

ModelItemEndPipe::ModelItemEndPipe(EndPipe *pipe, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_endpipe(pipe)

{

    setScale(0.8);

    setToolTip("Pipe: " + QString::number(p_endpipe->number()));

}

//-----------------------------------------------------------------------------------------------
// Open the inspector window if item is double clicked
//-----------------------------------------------------------------------------------------------
void ModelItemEndPipe::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    InspectorEndPipe *inspector = new InspectorEndPipe(p_endpipe);

    ModelScene *m_scene = dynamic_cast<ModelScene*>(scene());
    connect(inspector, SIGNAL(sendMsg(QString)), m_scene, SIGNAL(sendMsg(QString)));

}


} // namespace
