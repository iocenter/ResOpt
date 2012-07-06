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

#ifndef PIPE_H
#define PIPE_H

#include <QString>
#include <QStringList>
#include <QVector>

#include "component.h"


namespace ResOpt
{

class ProductionWell;
class PressureDropCalculator;
class Stream;


/**
 * @brief Abstract base class for pipe segments
 * @details A Pipe is fed by either Wells or other Pipes. Pressure drop calculations are performed based on the input streames
 *          going through the pipe and the outlet pressure of the pipe.
 *
 */
class Pipe : public Component
{
private:
    int m_number;                           // the identifier for the pipe
    QString m_file_name;                    // the file containing the pressure drop table
    PressureDropCalculator *p_calculator;   // the pressure drop calculator


    QVector<Pipe*> m_feed_pipes;                    // pointers to pipes entering this pipe
    QVector<ProductionWell*> m_feed_wells;                    // pointers to wells entering this pipe directly
    QVector<double> m_schedule;         // a copy of the master schedule set in the model



    /**
     * @brief
     *
     * @param line
     * @return QStringList
     */
    QStringList processLine(const QString& line);

    /**
     * @brief
     *
     * @param list
     * @return bool
     */
    bool isEmpty(const QStringList &list);


public:
    Pipe();
    Pipe(const Pipe &p);


    virtual ~Pipe();


    // virtual functions

    virtual Pipe* clone() = 0;



    /**
     * @brief Calculates the inlet pressure of the pipe based on the outlet pressure and the incoming streams
     *
     */
    virtual void calculateInletPressure() = 0;


    // misc functions

    void initialize(const QVector<double> &schedule);


    /**
     * @brief A recursive function that calculates the inlet pressures for all the feed pipes.
     * @details The inlet pressures are calculated recursively downwards in the branch. First for all the feed pipes connected to this
     *          pipe directly, then for all the feed pipes connected to the feed pipes of this pipe, and so on.
     *
     */
    void calculateBranchInletPressures();




    /**
     * @brief Removes all the current feed Wells and Pipes
     *
     */
    void cleanFeedConnections();



    /**
     * @brief Reads the input file containing the pressure drop tables for this pipe segment
     *
     */
    void readInputFile();



    /**
     * @brief Resests the rates and pressures in all the streams to zero
     *
     */
    void emptyStreams();


    // add functions


    /**
     * @brief Adds s to Stream i
     *
     * @param i
     * @param s
     * @return bool
     */
    bool addToStream(int i, const Stream &s);

    /**
     * @brief Adds a Pipe as a feed stream to this Pipe.
     *
     * @param p
     */
    void addFeedPipe(Pipe *p) {m_feed_pipes.push_back(p);}

    /**
     * @brief Adds a Well as a direct feed to this Pipe
     * @details Only Wells that feed directly into this Pipe should be added here. If the Well feeds indirectly from another Pipe,
     *          the connection should be taken care of through addFeedPipe(Pipe*).
     *
     * @param w
     */
    void addFeedWell(ProductionWell *w) {m_feed_wells.push_back(w);}



    // set functions



    /**
     * @brief Sets the identifying number of the pipe
     *
     * @param n
     */
    void setNumber(int n) {m_number = n;}

    /**
     * @brief Sets the file name of the file containing pressure drop tables for the pipe.
     *
     * @param s
     */
    void setFileName(const QString &s) {m_file_name = s;}



    // get functions

    /**
     * @brief Returns the identifying number of the pipe
     *
     * @return int
     */
    int number() const {return m_number;}



    /**
     * @brief Returns the pressure drop calculator for this Pipe
     *
     * @return PressureDropCalculator
     */
    PressureDropCalculator* calculator() {return p_calculator;}








};

} // namespace ResOpt


#endif // PIPE_H
