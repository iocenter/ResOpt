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


#include "inspectorwellconnectionvariable.h"

#include "inspectorvariable.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QDoubleValidator>
#include <QIntValidator>



namespace ResOptGui
{


InspectorWellConnectionVariable::InspectorWellConnectionVariable(int i, int i_max, int i_min, int j, int j_max, int j_min, int k1, int k2, int wi, QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    // setting up the i variable
    p_var_i = new InspectorVariable("i-direction: ", i, i_max, i_min, this, true);
    layout->addWidget(p_var_i, 0, 0, 1, 6);

    // setting up the j variable
    p_var_j = new InspectorVariable("j-direction: ", j, j_max, j_min, this, false);
    layout->addWidget(p_var_j, 1, 0, 1, 6);

    // setting up a spacer line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line, 2, 0, 1, 6);

    // setting up k1
    QLabel *p_lbl_k1 = new QLabel("k1: ", this);
    p_lbl_k1->setFixedWidth(50);
    p_lbl_k1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    p_led_k1 = new QLineEdit(QString::number(k1), this);
    p_led_k1->setFixedWidth(50);
    layout->addWidget(p_lbl_k1, 3, 0);
    layout->addWidget(p_led_k1, 3, 1);

    // setting up k2
    QLabel *p_lbl_k2 = new QLabel("k2: ", this);
    p_lbl_k2->setFixedWidth(50);
    p_lbl_k2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    p_led_k2 = new QLineEdit(QString::number(k2), this);
    p_led_k2->setFixedWidth(50);
    layout->addWidget(p_lbl_k2, 3, 2, Qt::AlignRight);
    layout->addWidget(p_led_k2, 3, 3);

    // setting up well index
    QLabel *p_lbl_wi = new QLabel("well index: ", this);
    p_lbl_wi->setFixedWidth(100);
    p_lbl_wi->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    p_led_wi = new QLineEdit(QString::number(wi), this);
    p_led_wi->setFixedWidth(100);
    layout->addWidget(p_lbl_wi, 3, 4, Qt::AlignRight);
    layout->addWidget(p_led_wi, 3, 5);






    setDisabled(true);


}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------




} // namespace
