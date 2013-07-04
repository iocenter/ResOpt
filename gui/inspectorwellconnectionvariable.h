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


namespace ResOptGui
{
class InspectorVariable;

class InspectorWellConnectionVariable : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorWellConnectionVariable(int i, int i_max, int i_min, int j, int j_max, int j_min, int k1, int k2, int wi ,  QWidget *parent = 0);


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
