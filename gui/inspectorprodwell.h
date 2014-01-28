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


#ifndef INSPECTORPRODWELL_H
#define INSPECTORPRODWELL_H
#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QtWidgets/QGroupBox>

namespace ResOpt
{
class ProductionWell;
}

using ResOpt::ProductionWell;

namespace ResOptGui
{

class InspectorVariable;
class InspectorWellControl;
class InspectorConstraint;
class InspectorWellConnectionVariable;
class InspectorGasLift;

class InspectorProdWell : public QWidget
{
    Q_OBJECT
private:
    ProductionWell *p_well;

    QVector<InspectorWellControl*> m_controls;
    QVector<InspectorGasLift*> m_gaslift;
    QVector<InspectorConstraint*> m_bhp_constraints;
    QVector<InspectorWellConnectionVariable*> m_varcons;

    QGroupBox *box_control;
    QGroupBox *box_gaslift;
    QGroupBox *box_varcon;
    QGroupBox *box_bhp_con;

    QPushButton *p_btn_control;
    QPushButton *p_btn_gaslift;
    QPushButton *p_btn_varcon;
    QPushButton *p_btn_bhp_con;

    QPushButton m_btn_close;
    QPushButton m_btn_ok;
    QPushButton m_btn_plot;

    void construct();


public:
    explicit InspectorProdWell(ProductionWell *well, QWidget *parent = 0);

signals:
    void sendMsg(QString);

public slots:
    void saveAndClose();
    void openPlot();
    void hideControls(bool b);
    void hideGasLift(bool b);
    void hideConnectionVariables(bool b);
    void hideBhpConstraints(bool b);


};

} // namespace


#endif // INSPECTORPRODWELL_H
