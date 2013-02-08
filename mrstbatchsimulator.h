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


#ifndef MRSTBATCHSIMULATOR_H
#define MRSTBATCHSIMULATOR_H

#include <QStringList>
#include "reservoirsimulator.h"

class QTextStream;


namespace ResOpt
{

class Well;
class WellControl;


/**
 * @brief Interface for MRST in batch model.
 * @details
 *
 */
class MrstBatchSimulator : public ReservoirSimulator
{
private:
    bool generateControlInputFile(Model *m);

    bool readWellOutput(Well *w, QString file_name);

    QStringList processLine(const QString &line);

public:
    MrstBatchSimulator();

    MrstBatchSimulator(const MrstBatchSimulator &m);

    virtual ~MrstBatchSimulator();

    // virtual functions

    virtual ReservoirSimulator* clone() const {return new MrstBatchSimulator(*this);}

    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator();
    virtual bool readOutput(Model *m);


};

} // namespace ResOpt

#endif // MRSTBATCHSIMULATOR_H

