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

#include "pipe.h"

#include <QFile>
#include <iostream>

#include "beggsbrillcalculator.h"
#include "stream.h"
#include "productionwell.h"

using std::cout;
using std::endl;

namespace ResOpt
{


Pipe::Pipe()
    : p_calculator(0)
{
}

//-----------------------------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------------------------
Pipe::Pipe(const Pipe &p)
{
    /*
    int m_number;                           // the identifier for the pipe
    QString m_file_name;                    // the file containing the pressure drop table
    double m_inletpressure;                 // the calculated inlet pressure of the pipe
    PressureDropCalculator *p_calculator;   // the pressure drop calculator


    QVector<Pipe*> m_feed_pipes;                    // pointers to pipes entering this pipe
    QVector<ProductionWell*> m_feed_wells;                    // pointers to wells entering this pipe directly
    QVector<ProductionWell*> m_connected_wells;               // pointers to all wells flowing through the pipe
    QVector<double> m_connected_well_fractions;     // the fractions of flow from the connected wells going through this pipe

    QVector<Stream*> m_streams;         // the streams going through the pipe


      */

    // copying basic types
    m_number = p.m_number;
    m_file_name = p.m_file_name;

    // the calculator
    if(p.p_calculator != 0) p_calculator = p.p_calculator->clone();
}

Pipe::~Pipe()
{
    if(p_calculator != 0) delete p_calculator;

    for(int i = 0; i < numberOfStreams(); i++)
    {
        delete m_streams.at(i);
    }
}


//-----------------------------------------------------------------------------------------------
// Calculates the fractions of flow from the connected wells
//-----------------------------------------------------------------------------------------------
void Pipe::calculateConnectedWellFractions()
{
    // clearing the vector
    m_connected_well_fractions.clear();

    // getting the fractions
    for(int i = 0; i < numberOfConnectedWells(); i++)
    {
        double frac = connectedWell(i)->flowFraction(this);
        m_connected_well_fractions.push_back(frac);

    }

}

//-----------------------------------------------------------------------------------------------
// Collects all connected wells
//-----------------------------------------------------------------------------------------------
QVector<ProductionWell*> Pipe::findConnectedWells()
{
    QVector<ProductionWell*> wells;

    // adding the feed wells
    for(int i = 0; i < m_feed_wells.size(); i++)
    {
        wells.push_back(m_feed_wells.at(i));
    }

    // looping through the feed pipes
    for(int j = 0; j < m_feed_pipes.size(); j++)
    {
        wells += m_feed_pipes.at(j)->findConnectedWells();
    }


    // setting the connected wells to this pipe
    m_connected_wells = wells;

    // updating the flow fractions from the wells
    calculateConnectedWellFractions();

    return wells;
}

//-----------------------------------------------------------------------------------------------
// Removes all feed connections
//-----------------------------------------------------------------------------------------------
void Pipe::cleanFeedConnections()
{
    m_feed_pipes.clear();
    m_feed_wells.clear();
}


//-----------------------------------------------------------------------------------------------
// Removes all streams
//-----------------------------------------------------------------------------------------------
void Pipe::cleanStreams()
{
    // deleting
    for(int i = 0; i < numberOfStreams(); i++) delete m_streams.at(i);

    m_streams.clear();
}

//-----------------------------------------------------------------------------------------------
// Calculates the rates going through the pipe from the connected wells
//-----------------------------------------------------------------------------------------------
void Pipe::aggregateStreams()
{
    // cleaning previous streams
    cleanStreams();

    if(numberOfConnectedWells() > 0)
    {
        int n_streams = connectedWell(0)->numberOfStreams();

        // looping thorough the time steps
        for(int i = 0; i < n_streams; i++)
        {
            Stream *s = new Stream();

            // looping through the connected wells
            for(int j = 0; j < numberOfConnectedWells(); j++)
            {
                *s += *connectedWell(j)->stream(i)*m_connected_well_fractions.at(j);
            }

            // adding the stream to the pipe
            addStream(s);
        }
    }
    else    // no wells connected to the pipe
    {
        // TODO: needs fixing, if no wells are connected a runtime error occurs
    }
}

//-----------------------------------------------------------------------------------------------
// Calculates the inlet pressures for all the pipes in the branch
//-----------------------------------------------------------------------------------------------
void Pipe::calculateBranchInletPressures()
{
    // first calculating the inlet pressure for the top node (this)
    calculateInletPressure();

    // looping through the feed pipes, calculating the inlet pressures for the sub-branch
    for(int i = 0; i < m_feed_pipes.size(); i++)
    {
        m_feed_pipes.at(i)->calculateBranchInletPressures();
    }
}

//-----------------------------------------------------------------------------------------------
// Reads the pressure drop table
//-----------------------------------------------------------------------------------------------
void Pipe::readInputFile()
{
    // opening the input file
    QFile input(m_file_name);

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Could not open PIPE input file: %s", m_file_name.toAscii().data());
        exit(1);
    }


    // starting to read the file

    cout << "Reading pressure drop definition for PIPE " << number() << "..." << endl;


    double l_diameter = 0.0;
    double l_length = 0.0;
    double l_angle = 0.0;
    double l_temperature = 0.0;
    double l_sg_gas = 0.0;
    double l_den_oil = 0.0;
    double l_den_wat = 0.0;
    double l_vis_oil = 0.0;
    double l_vis_wat = 0.0;

    PressureDropCalculator *dp = 0;


    bool ok = true;

    QStringList list;


    list = processLine(input.readLine());

    while(!input.atEnd() && !list.at(0).startsWith("EOF"))
    {


        if(list.at(0).startsWith("CORRELATION"))              // getting the correlation type
        {
            if(list.at(1).startsWith("BB73")) dp = new BeggsBrillCalculator();                // Beggs & Brill
            else                                                                    // not recognized
            {
                cout << endl << "### Error detected in PIPE definition file ###" << endl
                     << "File: " << m_file_name.toAscii().data() << endl
                     << "CORRELATION type: " << list.at(1).toAscii().data() << endl
                     << "Is not recognized..." << endl << endl;

                exit(1);


            }
        }
        else if(list.at(0).startsWith("DIAMETER")) l_diameter = list.at(1).toDouble(&ok);       // getting the diameter
        else if(list.at(0).startsWith("LENGTH")) l_length = list.at(1).toDouble(&ok);           // getting the length
        else if(list.at(0).startsWith("ANGLE")) l_angle = list.at(1).toDouble(&ok);             // getting the angle
        else if(list.at(0).startsWith("TEMPERATURE")) l_temperature = list.at(1).toDouble(&ok); // getting the temp
        else if(list.at(0).startsWith("GASGRAVITY")) l_sg_gas = list.at(1).toDouble(&ok);       // getting the gas sg
        else if(list.at(0).startsWith("OILDENSITY")) l_den_oil = list.at(1).toDouble(&ok);      // getting the oil den
        else if(list.at(0).startsWith("WATERDENSITY")) l_den_wat = list.at(1).toDouble(&ok);    // getting the water den
        else if(list.at(0).startsWith("OILVISCOSITY")) l_vis_oil = list.at(1).toDouble(&ok);    // getting the oil visc
        else if(list.at(0).startsWith("WATERVISCOSITY")) l_vis_wat = list.at(1).toDouble(&ok);  // getting the water visc
        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in PIPE definition file! ###" << endl
                     << "File: " << m_file_name.toAscii().data() << endl
                     << "Keyword: " << list.join(" ").toAscii().data() << endl
                     << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(input.readLine());

    }

    // checking remaining input

    if(dp == 0 || !ok)
    {
        cout << endl << "### Error detected in PIPE definition file! ###" << endl
             << "File: " << m_file_name.toAscii().data() << endl
             << "Definition is incomplete..." << endl
             << "Last line: " << list.join(" ").toAscii().data() << endl;

        exit(1);


    }

    // checking if the correlation is BB73
    BeggsBrillCalculator *bb = dynamic_cast<BeggsBrillCalculator*>(dp);
    if(dp != 0)
    {
        // setting the parameters
        bb->setAngle(l_angle);
        bb->setDiameter(l_diameter);
        bb->setGasSpecificGravity(l_sg_gas);
        bb->setLength(l_length);
        bb->setOilDensity(l_den_oil);
        bb->setOilViscosity(l_vis_oil);
        bb->setTemperature(l_temperature);
        bb->setWaterDensity(l_den_wat);
        bb->setWaterViscosity(l_vis_wat);


        // assigning to the PIPE calculator
        p_calculator = bb;
    }



    // everything ok, closing input file
    input.close();

    cout << "Done reading for PIPE " << number() << ": All OK" << endl;




}


//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList Pipe::processLine(const QString& line)
{
    QString temp = line.split("!").at(0);   // removing everything after !
    temp = temp.trimmed();                  // removing trailing and leadnig whitespaces

    QStringList list = temp.split(QRegExp("\\s+"));

    while(list.size() < 2)
    {
        list.push_back(" ");
    }

    return list;
}

//-----------------------------------------------------------------------------------------------
// Checks if the line is empty or not (white spaces)
//-----------------------------------------------------------------------------------------------
bool Pipe::isEmpty(const QStringList &list)
{
    bool ok = true;

    for(int i = 0; i < list.size(); i++)
    {
        QString temp = list.at(i);

        temp.remove(QRegExp("\\s+"));

        if(temp.size() != 0)
        {
            ok = false;
            break;
        }

    }

    return ok;
}


} // namespace ResOpt
