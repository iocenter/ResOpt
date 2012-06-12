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


#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <QVector>
#include <QString>
#include <tr1/memory>


using std::tr1::shared_ptr;

namespace ResOpt
{

class Constraint;
class Pipe;



/**
 * @brief Container class for separators
 *
 */
class Separator
{
private:

    QString m_name;

    shared_ptr<Constraint> p_oil; /**< TODO */
    shared_ptr<Constraint> p_gas; /**< TODO */
    shared_ptr<Constraint> p_water; /**< TODO */
    shared_ptr<Constraint> p_liquid; /**< TODO */

    QVector<Pipe*> m_feed_pipes; /**< TODO */
    QVector<int> m_feed_pipe_numbers;


public:
/**
 * @brief
 *
 */
    Separator();

    // misc functions


    /**
     * @brief Updates the constraints based on the maximum rates going through the separator
     *
     */
    void updateConstraints();


    // add functions

    /**
     * @brief Adds a Pipe as a feed to the Separator
     *
     * @param p
     */
    void addFeedPipe(Pipe *p) {m_feed_pipes.push_back(p);}


    /**
     * @brief Adds the identifying number to a feed pipe.
     * @details The numbers must be resolved to pointers before running the model.
     *
     * @param i
     */
    void addFeedPipeNumber(int i) {m_feed_pipe_numbers.push_back(i);}

    // set functions


    /**
     * @brief Sets the separator name
     *
     * @param n
     */
    void setName(const QString &n) {m_name = n;}

    /**
     * @brief Sets the oil Constraint for the Separator
     *
     * @param c
     */
    void setOilConstraint(shared_ptr<Constraint> c) {p_oil = c;}

    /**
     * @brief Sets the gas Constraint for the Separator
     *
     * @param c
     */
    void setGasConstraint(shared_ptr<Constraint> c) {p_gas = c;}

    /**
     * @brief Sets the water Constraint for the Separator
     *
     * @param c
     */
    void setWaterConstraint(shared_ptr<Constraint> c) {p_water = c;}

    /**
     * @brief Sets the total liquid Constraint for the Separator
     *
     * @param c
     */
    void setLiquidConstraint(shared_ptr<Constraint> c) {p_liquid = c;}

    // get functions


    /**
     * @brief Returns the separator name
     *
     * @return const QString
     */
    const QString& name() const {return m_name;}

    /**
     * @brief Returns the oil Constraint for the Separator
     *
     * @return Constraint
     */
    shared_ptr<Constraint> oilConstraint() {return p_oil;}

    /**
     * @brief Returns the gas Constraint for the Separator
     *
     * @return Constraint
     */
    shared_ptr<Constraint> gasConstraint() {return p_gas;}

    /**
     * @brief Returns the water Constraint for the Separator
     *
     * @return Constraint
     */
    shared_ptr<Constraint> waterConstraint() {return p_water;}

    /**
     * @brief Returns the total liquid Constraint for the Separator
     *
     * @return Constraint
     */
    shared_ptr<Constraint> liquidConstraint() {return p_liquid;}


    /**
     * @brief The number of feed pipes specified in the driver file. Does not neccesary correspond to the number of connected pipes.
     *
     * @return int
     */
    int numberOfFeedPipeNumbers() {return m_feed_pipe_numbers.size();}


    /**
     * @brief Returns the number of feed pipe i (as input in the driver file)
     *
     * @param i
     * @return int
     */
    int feedPipeNumber(int i) {return m_feed_pipe_numbers.at(i);}

    int numberOfFeedPipes() {return m_feed_pipes.size();}
    Pipe* feedPipe(int i) {return m_feed_pipes.at(i);}
};

} // namespace ResOpt

#endif // SEPARATOR_H
