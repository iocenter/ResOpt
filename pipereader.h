#ifndef PIPEREADER_H
#define PIPEREADER_H

#include <QStringList>
#include <QFile>


namespace ResOpt
{

class PressureDropCalculator;
class BeggsBrillCalculator;
class DpTableCalculator;

class PipeReader
{
private:
    QStringList processLine(const QString& line);
    bool isEmpty(const QStringList &list);

    BeggsBrillCalculator* readBeggsBrill(QFile &file);
    DpTableCalculator* readDpTable(QFile &file);


public:
    PipeReader();

    PressureDropCalculator* readFile(const QString &file_name);


};


} // namespace ResOpt
#endif // PIPEREADER_H
