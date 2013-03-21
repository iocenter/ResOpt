#ifndef DPTABLECALCULATOR_H
#define DPTABLECALCULATOR_H

#include "pressuredropcalculator.h"

namespace ResOpt
{

class DpTable;
class Stream;

class DpTableCalculator : public PressureDropCalculator
{
private:
    DpTable *p_dp_table;

public:
    DpTableCalculator();
    DpTableCalculator(const DpTableCalculator &c);

    virtual ~DpTableCalculator();

    // virtual functions

    virtual PressureDropCalculator* clone() const {return new DpTableCalculator(*this);}

    /**
     * @brief Calculates the pressure drop in the pipe for a given stream and outlet pressure
     *
     * @param s
     * @param p_outlet
     * @return double
     */
    virtual double pressureDrop(Stream *s, double p_outlet, Stream::units unit);

    // set functions
    void setDpTable(DpTable *table) {p_dp_table = table;}



};

} // namespace ResOpt

#endif // DPTABLECALCULATOR_H
