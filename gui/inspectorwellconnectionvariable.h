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

// InspectorWellConnectionVariable

#ifndef INSPECTORWELLCONNECTIONVARIABLE_H
#define INSPECTORWELLCONNECTIONVARIABLE_H

#include <QWidget>


class QLineEdit;
class QLabel;


#include "inspectorvariable.h"
#include <QtWidgets/QLineEdit>

namespace ResOptGui
{


class InspectorWellConnectionVariable : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorWellConnectionVariable(int i, int i_max, int i_min, int j, int j_max, int j_min, int k1, int k2, int wi ,  QWidget *parent = 0);

    int i() {return p_var_i->value();}
    int iMax() {return p_var_i->max();}
    int iMin() {return p_var_i->min();}

    int j() {return p_var_j->value();}
    int jMax() {return p_var_j->max();}
    int jMin() {return p_var_j->min();}

    int k1() {return p_led_k1->text().toInt();}
    int k2() {return p_led_k2->text().toInt();}

    double wi() {return p_led_wi->text().replace(",",".").toDouble();}


signals:

public slots:

private:
    InspectorVariable *p_var_i;
    InspectorVariable *p_var_j;

    QLineEdit *p_led_k1;
    QLineEdit *p_led_k2;
    QLineEdit *p_led_wi;


};

} // namespace

#endif // INSPECTORWELLCONNECTIONVARIABLE_H
