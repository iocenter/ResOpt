#include "dptablecalculator.h"
#include "dptable.h"
#include "stream.h"

namespace ResOpt
{

DpTableCalculator::DpTableCalculator()
    : p_dp_table(0)
{
}

DpTableCalculator::DpTableCalculator(const DpTableCalculator &c)
{
    // copying the dp table
    p_dp_table = new DpTable(*c.p_dp_table);
}

DpTableCalculator::~DpTableCalculator()
{
    if(p_dp_table != 0) delete p_dp_table;
}


//-----------------------------------------------------------------------------------------------
// calculates the pressure drop
//-----------------------------------------------------------------------------------------------
double DpTableCalculator::pressureDrop(Stream *s, double p_outlet)
{
    return p_dp_table->interpolate(s->gasRate(), s->oilRate(), s->waterRate());
}

} // namespace ResOpt
