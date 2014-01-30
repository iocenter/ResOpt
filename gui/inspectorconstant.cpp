/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2014 Aleksander O. Juell <aleksander.juell@ntnu.no>
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


#include "inspectorconstant.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QDoubleValidator>

namespace ResOptGui
{


InspectorConstant::InspectorConstant(const QString &name, double value, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        layout->addWidget(new QLabel("value", this), row, 1, Qt::AlignLeft);

        row++;
    }

    // setting up the components

    p_name = new QLabel(name, this);
    p_name->setFixedWidth(150);
    layout->addWidget(p_name, row, 0, Qt::AlignLeft);

    p_value = new QLineEdit(QString::number(value), this);
    //p_value->setFixedWidth(200);
    p_value->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_value, row, 1, Qt::AlignLeft);

    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);


}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
double InspectorConstant::value()
{
    return p_value->text().replace(",",".").toDouble();
}


} // namespace
