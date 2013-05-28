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


#ifndef INSPECTORCAPACITY_H
#define INSPECTORCAPACITY_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QVector>

namespace ResOpt
{
class Capacity;
}

using ResOpt::Capacity;

namespace ResOptGui
{


class InspectorConstraint;

class InspectorCapacity : public QWidget
{
    Q_OBJECT
private:
    Capacity *p_capacity;

   // QVector<InspectorConstraint*> m_constraints;

    QLabel m_lbl_oil;
    QCheckBox m_chk_oil;
    QLineEdit m_led_oil;

    QLabel m_lbl_gas;
    QCheckBox m_chk_gas;
    QLineEdit m_led_gas;

    QLabel m_lbl_water;
    QCheckBox m_chk_water;
    QLineEdit m_led_water;

    QLabel m_lbl_liquid;
    QCheckBox m_chk_liquid;
    QLineEdit m_led_liquid;

    QPushButton m_btn_close;
    QPushButton m_btn_ok;

    bool m_changed;

    void construct();


public:
    explicit InspectorCapacity(Capacity *cap, QWidget *parent = 0);

signals:
    void sendMsg(QString);

public slots:
    void saveAndClose();
    void setChanged();


};

} // namespace

#endif // INSPECTORCAPACITY_H
