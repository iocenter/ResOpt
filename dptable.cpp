#include "dptable.h"

#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{

DpTable::DpTable()
{
}

//-----------------------------------------------------------------------------------------------
// Adds a row of data to the tables
//-----------------------------------------------------------------------------------------------
void DpTable::addRow(double dp, double gas, double oil, double water)
{
    m_dp.push_back(dp);
    m_gas.push_back(gas);
    m_oil.push_back(oil);
    m_wat.push_back(water);
}


//-----------------------------------------------------------------------------------------------
// finds the uniquie entries for the gas, oil, and water
//-----------------------------------------------------------------------------------------------
void DpTable::process()
{
    for(int i = 0; i < numberOfRows(); ++i)
    {
        // the gas
        if(!m_entries_gas.contains(m_gas.at(i))) m_entries_gas.push_back(m_gas.at(i));

        // the oil
        if(!m_entries_oil.contains(m_oil.at(i))) m_entries_oil.push_back(m_oil.at(i));

        // the water
        if(!m_entries_wat.contains(m_wat.at(i))) m_entries_wat.push_back(m_wat.at(i));

    }

    // sorting the entries
    qSort(m_entries_gas.begin(), m_entries_gas.end());
    qSort(m_entries_oil.begin(), m_entries_oil.end());
    qSort(m_entries_wat.begin(), m_entries_wat.end());

}


//-----------------------------------------------------------------------------------------------
// finds the index for the upper gas, oil, and water in the entries lists
//-----------------------------------------------------------------------------------------------
QList<int> DpTable::findUpperEntries(double gas, double oil, double water)
{

    int i_g = 0;
    int i_o = 0;
    int i_w = 0;

    QList<int> indexes;


    // finding the entry index for gas
    for(int i = 0; i < m_entries_gas.size(); ++i)
    {
        if(m_entries_gas.at(i) > gas)
        {
            i_g = i;
            break;
        }
    }

    // finding the entry index for oil
    for(int i = 0; i < m_entries_oil.size(); ++i)
    {
        if(m_entries_oil.at(i) > oil)
        {
            i_o = i;
            break;
        }
    }

    // finding the entry index for water
    for(int i = 0; i < m_entries_wat.size(); ++i)
    {
        if(m_entries_wat.at(i) > water)
        {
            i_w = i;
            break;
        }
    }

    // populating the list
    indexes << i_g << i_o << i_w;

    return indexes;

}

//-----------------------------------------------------------------------------------------------
// finds the index in the main table that corresponds to the entries in the entries table
//-----------------------------------------------------------------------------------------------
int DpTable::findTableIndex(int gas_entry, int oil_entry, int water_entry)
{
    int index = 0;

    double gas = m_entries_gas.at(gas_entry);
    double oil = m_entries_oil.at(oil_entry);
    double water = m_entries_wat.at(water_entry);


    for(int i = 0; i < m_gas.size(); ++i)
    {
        // first checknig the gas rate
        if(m_gas.at(i) == gas)
        {
            // then checking the oil rate
            if(m_oil.at(i) == oil)
            {

                // then checking the water rate
                if(m_wat.at(i) == water)
                {
                    index = i;
                    break;
                } // water
            } // oil
        } // gas
    } // for loop

    return index;
}


//-----------------------------------------------------------------------------------------------
// interpolates the table, returns the pressure drop
//-----------------------------------------------------------------------------------------------
double DpTable::interpolate(double gas, double oil, double water)
{


    // processing the table if not already done
    if(m_entries_gas.size() == 0) process();


    // checking that the desired point lies within the table
    if(gas < m_entries_gas.at(0) || gas > m_entries_gas.at(m_entries_gas.size() - 1))
    {
        cout << endl << "### Runtime Error ###" << endl
             << "The specified gas rate falls outside the range of the DP table..." << endl
             << "QG    : " << gas << endl
             << "QG_MAX: " << m_entries_gas.at(m_entries_gas.size() - 1) << endl
             << "QG_MIN: " << m_entries_gas.at(0) << endl;

        return 1000;



    }
    if(oil < m_entries_oil.at(0) || oil > m_entries_oil.at(m_entries_oil.size() - 1))
    {
        cout << endl << "### Runtime Error ###" << endl
             << "The specified oil rate falls outside the range of the DP table..." << endl
             << "QO    : " << oil << endl
             << "QO_MAX: " << m_entries_oil.at(m_entries_oil.size() - 1) << endl
             << "QO_MIN: " << m_entries_oil.at(0) << endl;

        return 1000;


    }
    if(water < m_entries_wat.at(0) || water > m_entries_wat.at(m_entries_wat.size() - 1))
    {
        cout << endl << "### Runtime Error ###" << endl
             << "The specified water rate falls outside the range of the DP table..." << endl
             << "QW    : " << water << endl
             << "QW_MAX: " << m_entries_wat.at(m_entries_wat.size() - 1) << endl
             << "QW_MIN: " << m_entries_wat.at(0) << endl;

        return 1000;


    }




    // getting the upper points
    QList<int> i_upper = findUpperEntries(gas, oil, water);

    // calculating the difference to the lower bounding points
    double xd = (gas - m_entries_gas.at(i_upper.at(0) - 1)) / (m_entries_gas.at(i_upper.at(0)) - m_entries_gas.at(i_upper.at(0) - 1));
    double yd = (oil - m_entries_oil.at(i_upper.at(1) - 1)) / (m_entries_oil.at(i_upper.at(1)) - m_entries_oil.at(i_upper.at(1) - 1));
    double zd = (water - m_entries_wat.at(i_upper.at(2) - 1)) / (m_entries_wat.at(i_upper.at(2)) - m_entries_wat.at(i_upper.at(2) - 1));


    // getting the indexes for the eight bounding points
    int i_000 = findTableIndex(i_upper.at(0) - 1, i_upper.at(1) - 1, i_upper.at(2) - 1);
    int i_010 = findTableIndex(i_upper.at(0) - 1, i_upper.at(1)    , i_upper.at(2) - 1);
    int i_001 = findTableIndex(i_upper.at(0) - 1, i_upper.at(1) - 1, i_upper.at(2)    );
    int i_011 = findTableIndex(i_upper.at(0) - 1, i_upper.at(1)    , i_upper.at(2)    );

    int i_100 = findTableIndex(i_upper.at(0)    , i_upper.at(1) - 1, i_upper.at(2) - 1);
    int i_110 = findTableIndex(i_upper.at(0)    , i_upper.at(1)    , i_upper.at(2) - 1);
    int i_101 = findTableIndex(i_upper.at(0)    , i_upper.at(1) - 1, i_upper.at(2)    );
    int i_111 = findTableIndex(i_upper.at(0)    , i_upper.at(1)    , i_upper.at(2)    );

    //interpolating along x (gas)
    double c_00 = m_dp.at(i_000) * (1 - xd) + m_dp.at(i_100) * xd;
    double c_10 = m_dp.at(i_010) * (1 - xd) + m_dp.at(i_110) * xd;
    double c_01 = m_dp.at(i_001) * (1 - xd) + m_dp.at(i_101) * xd;
    double c_11 = m_dp.at(i_011) * (1 - xd) + m_dp.at(i_111) * xd;

    // interpolating along y (oil)
    double c_0 = c_00 * (1 - yd) + c_10 * yd;
    double c_1 = c_01 * (1 - yd) + c_11 * yd;

    // interpolating along z (water)
    double c = c_0 * (1 - zd) + c_1 * zd;


    return c;
}


} // namespace ResOpt
