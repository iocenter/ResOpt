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
#include "pipereader.h"

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
    : Component(p)
{
    // copying basic types
    m_number = p.m_number;
    m_file_name = p.m_file_name;
    m_schedule = p.m_schedule;

    // the calculator
    if(p.p_calculator != 0) p_calculator = p.p_calculator->clone();
    else p_calculator = 0;


}

Pipe::~Pipe()
{
    if(p_calculator != 0) delete p_calculator;

}


//-----------------------------------------------------------------------------------------------
// initializes the Pipe, sets the streams and schedule
//-----------------------------------------------------------------------------------------------
void Pipe::initialize(const QVector<double> &schedule)
{
    m_schedule = schedule;

    clearStreams();
    for(int i = 0; i < m_schedule.size(); ++i) addStream(new Stream(schedule.at(i), 0, 0, 0, 0));

}


//-----------------------------------------------------------------------------------------------
// sets the rates and pressure of all the streams to zero
//-----------------------------------------------------------------------------------------------
void Pipe::emptyStreams()
{
    for(int i = 0; i < numberOfStreams(); ++i)
    {
        stream(i)->setGasRate(0);
        stream(i)->setOilRate(0);
        stream(i)->setWaterRate(0);
        stream(i)->setPressure(0);

    }
}




//-----------------------------------------------------------------------------------------------
// adds the rates in s to the current stream in the pipe
//-----------------------------------------------------------------------------------------------
bool Pipe::addToStream(int i, const Stream &s)
{
    // first checking that the stream vector is set up correctly
    if(i >= numberOfStreams() || i < 0)
    {
        cout << endl << "### Runtime Error ###" << endl
             << "Trying to add to a stream that does not exist... " << endl
             << "PIPE:   " << number() <<  endl
             << "STREAM: " << i << endl << endl;

        exit(1);

    }

    // adding the stream
    *stream(i) += s;


    return true;
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

    // creating a pipe reader
    PipeReader reader;

    // reading the file, getting back the calculator
    p_calculator = reader.readFile(m_file_name);



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
