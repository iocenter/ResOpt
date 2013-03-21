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

#ifndef WELLCONNECTION_H
#define WELLCONNECTION_H

namespace ResOpt
{


/**
 * @brief Represents a well perforation
 *
 */
class WellConnection
{
private:
    int m_cell; /**< TODO */
    int m_i;
    int m_j;
    int m_k1;
    int m_k2;
    int m_wi; /**< TODO */

public:
/**
 * @brief
 *
 */
    WellConnection();

    // set functions
    /**
     * @brief Sets the cell number
     *
     * @param i
     */
    void setCell(int i) {m_cell = i;}
    void setI(int i) {m_i = i;}
    void setJ(int j) {m_j = j;}
    void setK1(int k1) {m_k1 = k1;}
    void setK2(int k2) {m_k2 = k2;}

    /**
     * @brief Sets the connection well index
     *
     * @param wi
     */
    void setWellIndex(double wi) {m_wi = wi;}

    // get functions

    /**
     * @brief Returns the cell number
     *
     * @return int
     */
    int cell() const {return m_cell;}
    int i() const {return m_i;}
    int j() const {return m_j;}
    int k1() const {return m_k1;}
    int k2() const {return m_k2;}

    /**
     * @brief Returns the well index for the connection
     *
     * @return double
     */
    double wellIndex() const {return m_wi;}

};

} // namespace ResOpt

#endif // WELLCONNECTION_H
