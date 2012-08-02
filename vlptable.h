#ifndef VLPTABLE_H
#define VLPTABLE_H

#include <QList>
#include <QString>

namespace ResOpt
{

class VlpTable
{
private:
    QList<double> m_pbh;
    QList<double> m_glift;
    QList<double> m_oil;
    QList<double> m_gas;
    QList<double> m_wat;

    QString m_well_name;

public:
    VlpTable();

    // set functions
    void setWellName(const QString &n) {m_well_name = n;}

    // add functions
    void addRow(double pbh, double glift, double oil, double gas, double wat);

    // get functions
    const QString& wellName() const {return m_well_name;}

    int numberOfRows() const {return m_pbh.size();}

    double pbh(int i) const {return m_pbh.at(i);}
    double glift(int i) const {return m_glift.at(i);}
    double oil(int i) const {return m_oil.at(i);}
    double gas(int i) const {return m_gas.at(i);}
    double wat(int i) const {return m_wat.at(i);}


};


} // namespace ResOpt

#endif // VLPTABLE_H
