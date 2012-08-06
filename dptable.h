#ifndef DPTABLE_H
#define DPTABLE_H

#include <QList>

namespace ResOpt
{

class DpTable
{
private:

    QList<double> m_gas;
    QList<double> m_oil;
    QList<double> m_wat;

    QList<double> m_dp;

    QList<double> m_entries_gas;
    QList<double> m_entries_oil;
    QList<double> m_entries_wat;

    void process();

    QList<int> findUpperEntries(double gas, double oil, double water);
    int findTableIndex(int gas_entry, int oil_entry, int water_entry);


public:
    DpTable();

    // misc functions

    double interpolate(double gas, double oil, double water);

    // add functions
    void addRow(double dp, double gas, double oil, double water);

    // get functions
    int numberOfRows() const {return m_gas.size();}

};


} // namespace ResOpt
#endif // DPTABLE_H
