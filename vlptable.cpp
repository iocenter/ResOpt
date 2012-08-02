#include "vlptable.h"

namespace ResOpt
{


VlpTable::VlpTable()
{
}

//-----------------------------------------------------------------------------------------------
// Adds a row of data to the vlp table
//-----------------------------------------------------------------------------------------------
void VlpTable::addRow(double pbh, double glift, double oil, double gas, double wat)
{
    m_pbh.push_back(pbh);
    m_glift.push_back(glift);
    m_oil.push_back(oil);
    m_gas.push_back(gas);
    m_wat.push_back(wat);
}


} // namespace ResOpt
