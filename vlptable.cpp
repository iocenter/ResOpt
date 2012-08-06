#include "vlptable.h"

#include "stream.h"

#include <QtAlgorithms>
#include <iostream>

using std::cout;
using std::endl;

namespace ResOpt
{


VlpTable::VlpTable()
{
}

//-----------------------------------------------------------------------------------------------
// Adds a row of data to the vlp table
//-----------------------------------------------------------------------------------------------
void VlpTable::addRow(double glift, double pbh, double gas, double oil, double wat)
{
    m_pbh.push_back(pbh);
    m_glift.push_back(glift);
    m_oil.push_back(oil);
    m_gas.push_back(gas);
    m_wat.push_back(wat);
}



//-----------------------------------------------------------------------------------------------
// interpolates the table
//-----------------------------------------------------------------------------------------------
Stream* VlpTable::interpolate(double pbh, double glift)
{
    // check if the table has been processed
    if(m_glift_entries.size() == 0) process();

    // check that the point lies within the table
    if(pbh < m_pbh_entries.at(0) || pbh > m_pbh_entries.at(m_pbh_entries.size() - 1))
    {
        cout << endl << "### Runtime Error ###" << endl
             << "The specified pressure falls outside the range of the VLP table..." << endl
             << "WELL : " << m_well_name.toAscii().data() <<  endl
             << "P    : " << pbh << endl
             << "P_MAX: " << m_pbh_entries.at(m_pbh_entries.size() - 1) << endl
             << "P_MIN: " << m_pbh_entries.at(0) << endl;

        exit(1);

    }

    if(glift < m_glift_entries.at(0) || glift > m_glift_entries.at(m_glift_entries.size() - 1))
    {
        cout << endl << "### Runtime Error ###" << endl
             << "The specified gas lift rate falls outside the range of the VLP table..." << endl
             << "WELL : " << m_well_name.toAscii().data() <<  endl
             << "Q    : " << glift << endl
             << "Q_MAX: " << m_glift_entries.at(m_glift_entries.size() - 1) << endl
             << "Q_MIN: " << m_glift_entries.at(0) << endl;

        exit(1);

    }



    // finding the upper point (Q22)
    QPair<int,int> upper_point = findUpperEntries(pbh, glift);

    // finding the index in the table for the four bounding points
    int i_Q22 = findTableIndex(upper_point.first, upper_point.second);
    int i_Q21 = findTableIndex(upper_point.first, upper_point.second - 1);
    int i_Q12 = findTableIndex(upper_point.first - 1, upper_point.second);
    int i_Q11 = findTableIndex(upper_point.first - 1, upper_point.second - 1);

    // finding the weights
    double t = (pbh - m_pbh.at(i_Q11)) / (m_pbh.at(i_Q21) - m_pbh.at(i_Q11));
    double u = (glift - m_glift.at(i_Q11)) / (m_glift.at(i_Q12) - m_glift.at(i_Q11));


    // finding the gas rate
    double qg = (1 - t) * (1 - u) * m_gas.at(i_Q11)
            + t * (1 - u) * m_gas.at(i_Q21)
            + t * u * m_gas.at(i_Q22)
            + (1 - t) * u * m_gas.at(i_Q12);

    // finding the oil rate
    double qo = (1 - t) * (1 - u) * m_oil.at(i_Q11)
            + t * (1 - u) * m_oil.at(i_Q21)
            + t * u * m_oil.at(i_Q22)
            + (1 - t) * u * m_oil.at(i_Q12);

    // finding the water rate
    double qw = (1 - t) * (1 - u) * m_wat.at(i_Q11)
            + t * (1 - u) * m_wat.at(i_Q21)
            + t * u * m_wat.at(i_Q22)
            + (1 - t) * u * m_wat.at(i_Q12);


    Stream *s = new Stream(0, qo, qg, qw, pbh);



    return s;
}

//-----------------------------------------------------------------------------------------------
// finds the index for the upper glift and lower pbh point
//-----------------------------------------------------------------------------------------------
void VlpTable::process()
{
    for(int i = 0; i < numberOfRows(); ++i)
    {
        // the glift
        if(!m_glift_entries.contains(m_glift.at(i))) m_glift_entries.push_back(m_glift.at(i));

        // the pbh
        if(!m_pbh_entries.contains(m_pbh.at(i))) m_pbh_entries.push_back(m_pbh.at(i));

    }

    // sorting the entries
    qSort(m_glift_entries.begin(), m_glift_entries.end());
    qSort(m_pbh_entries.begin(), m_pbh_entries.end());
}

//-----------------------------------------------------------------------------------------------
// finds the index for the upper glift and upper pbh point in the entries lists
//-----------------------------------------------------------------------------------------------
QPair<int,int> VlpTable::findUpperEntries(double pbh, double glift)
{

    int i_gl = 0;
    int i_pbh = 0;


    // finding the entry index for pbh
    for(int i = 0; i < m_pbh_entries.size(); ++i)
    {
        if(m_pbh_entries.at(i) > pbh)
        {
            i_pbh = i;
            break;
        }
    }

    for(int i = 0; i < m_glift_entries.size(); ++i)
    {
        if(m_glift_entries.at(i) > glift)
        {
            i_gl = i;
            break;
        }

    }

    return qMakePair(i_pbh, i_gl);

}

//-----------------------------------------------------------------------------------------------
// finds the index in the main table that corresponds to the entries in the entries table
//-----------------------------------------------------------------------------------------------
int VlpTable::findTableIndex(int pbh_entry, int glift_entry)
{
    int index = 0;

    double glift = m_glift_entries.at(glift_entry);
    double pbh = m_pbh_entries.at(pbh_entry);

    for(int i = 0; i < m_glift.size(); ++i)
    {
        // first checknig the gas lift rate
        if(m_glift.at(i) == glift)
        {
            // then checking the bottomhole pressure
            if(m_pbh.at(i) == pbh)
            {
                index = i;
                break;
            }
        }
    }

    return index;
}

} // namespace ResOpt
