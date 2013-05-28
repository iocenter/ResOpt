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


#include "inspectorwellcontrol.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QComboBox>
#include <QDoubleValidator>



namespace ResOptGui
{


InspectorWellControl::InspectorWellControl(double time, double value, double max, double min, WellControl::contol_type type, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        QLabel *time_header = new QLabel("time", this);
        time_header->setMinimumWidth(80);
        layout->addWidget(time_header, row, 0, Qt::AlignCenter);
        layout->addWidget(new QLabel("value", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 3, Qt::AlignCenter);
        layout->addWidget(new QLabel("type", this), row, 4, Qt::AlignCenter);

        row++;
    }

    // setting up the components

    p_time = new QLabel(QString::number(time), this);
    p_time->setMinimumWidth(80);
    layout->addWidget(p_time, row, 0, Qt::AlignCenter);

    p_value = new QLineEdit(QString::number(value), this);
    p_value->setFixedWidth(100);
    p_value->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_value, row, 1);

    p_max = new QLineEdit(QString::number(max), this);
    p_max->setFixedWidth(100);
    p_max->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_max, row, 2);

    p_min = new QLineEdit(QString::number(min), this);
    p_min->setFixedWidth(100);
    p_min->setValidator(new QDoubleValidator(this));
    layout->addWidget(p_min, row, 3);

    p_type = new QComboBox(this);
    p_type->addItem("BHP", WellControl::BHP);
    p_type->addItem("OIL", WellControl::QOIL);
    p_type->addItem("GAS", WellControl::QGAS);
    p_type->addItem("WAT", WellControl::QWAT);

    p_type->setCurrentIndex(p_type->findData(type));


    layout->addWidget(p_type, row, 4);


}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
double InspectorWellControl::value()
{
    return p_value->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current max
//-----------------------------------------------------------------------------------------------
double InspectorWellControl::max()
{
    return p_max->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current min
//-----------------------------------------------------------------------------------------------
double InspectorWellControl::min()
{
    return p_min->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current type
//-----------------------------------------------------------------------------------------------
WellControl::contol_type InspectorWellControl::type()
{
    if(p_type->itemData(p_type->currentIndex()) == WellControl::BHP) return WellControl::BHP;
    else if(p_type->itemData(p_type->currentIndex()) == WellControl::QOIL) return WellControl::QOIL;
    else if(p_type->itemData(p_type->currentIndex()) == WellControl::QWAT) return WellControl::QWAT;
    else return WellControl::QGAS;
}


} // namespace
