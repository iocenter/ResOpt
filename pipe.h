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


namespace ResOpt
{

class ProductionWell;
class PressureDropCalculator;
class Stream;


/**
 * @brief A container class for pipe segments
 * @details A Pipe is fed by either Wells or other Pipes. Pressure drop calculations are performed based on the input streames
 *          going through the pipe and the outlet pressure of the pipe.
 *
 */
class Pipe
{
private:
    int m_number;                           // the identifier for the pipe
    QString m_file_name;                    // the file containing the pressure drop table
    double m_inletpressure;                 // the calculated inlet pressure of the pipe
    PressureDropCalculator *p_calculator;   // the pressure drop calculator


    QVector<Pipe*> m_feed_pipes;                    // pointers to pipes entering this pipe
    QVector<ProductionWell*> m_feed_wells;                    // pointers to wells entering this pipe directly
    QVector<ProductionWell*> m_connected_wells;               // pointers to all wells flowing through the pipe
    QVector<double> m_connected_well_fractions;     // the fractions of flow from the connected wells going through this pipe

    QVector<Stream*> m_streams;         // the streams going through the pipe



    /**
     * @brief Calculats the fraction of flow going through the pipe for all the connected wells
     *
     */
    void calculateConnectedWellFractions();

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
    ~Pipe();


    // virtual functions



    /**
     * @brief Calculates the inlet pressure of the pipe based on the outlet pressure and the incoming streams
     *
     */
    virtual void calculateInletPressure() = 0;


    // misc functions


    /**
     * @brief A recursive function that calculates the inlet pressures for all the feed pipes.
     * @details The inlet pressures are calculated recursively downwards in the branch. First for all the feed pipes connected to this
     *          pipe directly, then for all the feed pipes connected to the feed pipes of this pipe, and so on.
     *
     */
    void calculateBranchInletPressures();


    /**
     * @brief Collects all the wells connected to the pipe, directly and indirectly.
     *
     */
    QVector<ProductionWell*> findConnectedWells();


    /**
     * @brief Removes all the current feed Wells and Pipes
     *
     */
    void cleanFeedConnections();


    /**
     * @brief Deletes all the streams for the Pipe
     *
     */
    void cleanStreams();


    /**
     * @brief Sums up all the streams from the connected wells to form the total rates going through the pipe
     *
     */
    void aggregateStreams();



    /**
     * @brief Reads the input file containing the pressure drop tables for this pipe segment
     *
     */
    void readInputFile();


    // add functions

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

    void addStream(Stream *s) {m_streams.push_back(s);}

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


    /**
     * @brief Sets the inlet pressure of the pipe. This pressure should be calculated by calculateInletPressure().
     *
     * @param p
     */
    void setInletPressure(double p) {m_inletpressure = p;}


    // get functions

    /**
     * @brief Returns the identifying number of the pipe
     *
     * @return int
     */
    int number() {return m_number;}


    /**
     * @brief Returns the calculated inlet pressure of the Pipe
     *
     * @return double
     */
    double inletPressure() {return m_inletpressure;}


    /**
     * @brief Returns the pressure drop calculator for this Pipe
     *
     * @return PressureDropCalculator
     */
    PressureDropCalculator* calculator() {return p_calculator;}


    /**
     * @brief Returns the number of wells that flow through the pipe
     *
     * @return int
     */
    int numberOfConnectedWells() {return m_connected_wells.size();}

    int numberOfStreams() {return m_streams.size();}

    Stream* stream(int i) {return m_streams.at(i);}


    /**
     * @brief Returns the connected well i
     *
     * @param i
     * @return Well
     */
    ProductionWell* connectedWell(int i) {return m_connected_wells.at(i);}


};

} // namespace ResOpt


#endif // PIPE_H
