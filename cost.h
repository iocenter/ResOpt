#ifndef COST_H
#define COST_H

namespace ResOpt
{


/**
 * @brief Class for costs associated with installing equipment etc.
 *
 */
class Cost
{
private:
    double m_time;
    double m_value;

public:
    Cost();

    // set functions
    void setTime(double t) {m_time = t;}
    void setValue(double v) {m_value = v;}

    // get functions
    double time() const {return m_time;}
    double value() const {return m_value;}
};


} // namespace ResOpt

#endif // COST_H
