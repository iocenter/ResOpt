/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


#include "injectionwell.h"
#include "intvariable.h"
#include "cost.h"
#include "wellconnection.h"
#include "wellcontrol.h"

namespace ResOpt
{


InjectionWell::InjectionWell()
{
}

InjectionWell::InjectionWell(const InjectionWell &w)
    : Well(w)
{
}


InjectionWell::~InjectionWell()
{
}


//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString InjectionWell::description() const
{
    QString str("START WELL\n");
    str.append(" NAME " + name() + "\n");
    str.append(" TYPE I \n");
    str.append(" GROUP " + group() + "\n");
    str.append(" BHPLIMIT " + QString::number(bhpLimit()) + "\n");
    str.append(" BHPINJ ");
    if(bhpInj() == WellControl::QWAT) str.append("WATER\n");
    else if(bhpInj() == WellControl::QOIL) str.append("OIL\n");
    else if(bhpInj() == WellControl::QGAS) str.append("GAS\n");


    if(hasInstallTime())
    {
        str.append(" INSTALLTIME " + QString::number(installTime()->value()) + " " + QString::number(installTime()->max()) + " " + QString::number(installTime()->min()) + "\n");
    }

    if(hasCost())
    {
        str.append(" COST " + QString::number(cost()->constantCost()) + " " + QString::number(cost()->fractionCost()) + " " + QString::number(cost()->capacityCost()) + "\n");
    }

    str.append(" START CONNECTIONS\n");
    for(int i = 0; i < numberOfConnections(); ++i)
    {
        WellConnection *wc = connection(i);

        if(wc->cell() >= 0) str.append("  " + QString::number(wc->cell()) + " " + QString::number(wc->wellIndex()) + "\n");
        else str.append("  " + QString::number(wc->i()) + " " +
                        QString::number(wc->j()) + " " +
                        QString::number(wc->k1()) + " " +
                        QString::number(wc->k2()) + " " +
                        QString::number(wc->wellIndex()) + "\n");

    }

    str.append(" END CONNECTIONS\n\n");


    str.append(" START SCHEDULE\n");

    for(int i = 0; i < numberOfControls(); ++i) str.append(control(i)->description());

    str.append(" END SCHEDULE\n\n");

    str.append("END WELL\n\n");
    return str;

}


} // namespace ResOpt
