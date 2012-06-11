#ifndef PRESSUREDROPCALCULATOR_H
#define PRESSUREDROPCALCULATOR_H




namespace ResOpt
{

class Stream;

class PressureDropCalculator
{
public:
    PressureDropCalculator();

    virtual double pressureDrop(Stream *s, double p_outlet) = 0;
};

} // namespace ResOpt

#endif // PRESSUREDROPCALCULATOR_H
