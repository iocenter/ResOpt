#ifndef VLPTABLE_H
#define VLPTABLE_H

#include <QList>
#include <QString>
#include <QPair>

namespace ResOpt
{

class Stream;

class VlpTable
{
private:
    QList<double> m_pbh;
    QList<double> m_glift;
    QList<double> m_oil;
    QList<double> m_gas;
    QList<double> m_wat;

    QList<double> m_pbh_entries;        // list of the unique values for the pbh
    QList<double> m_glift_entries;      // list of the uniquie values for the glift

    QString m_well_name;


    /**
     * @brief returns the index for the upper glift and upper pbh point
     *
     * @param pbh
     * @param qlift
     * @return int
     */
    QPair<int,int> findUpperEntries(double pbh, double glift);

    int findTableIndex(int pbh_entry, int glift_entry);

public:
    VlpTable();

    // misc functions


    /**
     * @brief This function interpolates the table, and returns rates for the interpolated point.
     *
     * @param pbh
     * @param glift
     * @return Stream
     */
    Stream* interpolate(double pbh, double glift);


    /**
     * @brief Generates lists of the unique entries for glift and pbh, to be used by the interpolation algorithm
     *
     */
    void process();

    // set functions
    void setWellName(const QString &n) {m_well_name = n;}

    // add functions
    void addRow(double glift, double pbh, double gas, double oil, double wat);

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
