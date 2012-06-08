#ifndef PRESSUREDROPCALCULATOR_H
#define PRESSUREDROPCALCULATOR_H

class Stream;

class PressureDropCalculator
{
public:
    PressureDropCalculator();

    virtual double pressureDrop(Stream *s, double p_outlet) = 0;
};

#endif // PRESSUREDROPCALCULATOR_H
