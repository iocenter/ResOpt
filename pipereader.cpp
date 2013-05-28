#include "pipereader.h"

#include <iostream>
#include "pressuredropcalculator.h"
#include "beggsbrillcalculator.h"
#include "dptablecalculator.h"
#include "dptable.h"

using std::cout;
using std::endl;

namespace ResOpt
{

PipeReader::PipeReader()
{
}


//-----------------------------------------------------------------------------------------------
// Reads the input file, generates a pressure drop calculator
//-----------------------------------------------------------------------------------------------
PressureDropCalculator* PipeReader::readFile(const QString &file_name)
{
    PressureDropCalculator *calc = 0;


    // opening the input file
    QFile input(file_name);

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Could not open PIPE input file: %s", file_name.toLatin1().constData());
        exit(1);
    }


    // starting to read the file, finding what type of calculator to produce

    QStringList list = processLine(input.readLine());

    while(!input.atEnd() && !list.at(0).startsWith("EOF"))
    {

        if(list.at(0).startsWith("CORRELATION"))
        {
            if(list.at(1).startsWith("BB73")) calc = readBeggsBrill(input);         // this is a beggs and brill correlation
            else if(list.at(1).startsWith("TABLE")) calc = readDpTable(input);      // this is a table correlation
        }
        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in PIPE definition file! ###" << endl
                 << "File: " << file_name.toLatin1().constData() << endl
                 << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                 << "Not understood in current context." << endl << endl;

            exit(1);
        }


        list = processLine(input.readLine());
    }






    // everything ok, closing input file
    input.close();


    return calc;
}


//-----------------------------------------------------------------------------------------------
// Reads the input file, generates a beggs and brill calculator
//-----------------------------------------------------------------------------------------------
BeggsBrillCalculator* PipeReader::readBeggsBrill(QFile &file)
{
    BeggsBrillCalculator* bb = new BeggsBrillCalculator();

    double l_diameter = 0.0;
    double l_length = 0.0;
    double l_angle = 0.0;
    double l_temperature = 0.0;
    double l_sg_gas = 0.0;
    double l_den_oil = 0.0;
    double l_den_wat = 0.0;
    double l_vis_oil = 0.0;
    double l_vis_wat = 0.0;


    bool ok = true;

    QStringList list = processLine(file.readLine());

    while(!file.atEnd() && !list.at(0).startsWith("EOF"))
    {


        if(list.at(0).startsWith("DIAMETER")) l_diameter = list.at(1).toDouble(&ok);       // getting the diameter
        else if(list.at(0).startsWith("LENGTH")) l_length = list.at(1).toDouble(&ok);           // getting the length
        else if(list.at(0).startsWith("ANGLE")) l_angle = list.at(1).toDouble(&ok);             // getting the angle
        else if(list.at(0).startsWith("TEMPERATURE")) l_temperature = list.at(1).toDouble(&ok); // getting the temp
        else if(list.at(0).startsWith("GASGRAVITY")) l_sg_gas = list.at(1).toDouble(&ok);       // getting the gas sg
        else if(list.at(0).startsWith("OILDENSITY")) l_den_oil = list.at(1).toDouble(&ok);      // getting the oil den
        else if(list.at(0).startsWith("WATERDENSITY")) l_den_wat = list.at(1).toDouble(&ok);    // getting the water den
        else if(list.at(0).startsWith("OILVISCOSITY")) l_vis_oil = list.at(1).toDouble(&ok);    // getting the oil visc
        else if(list.at(0).startsWith("WATERVISCOSITY")) l_vis_wat = list.at(1).toDouble(&ok);  // getting the water visc
        else
        {
            if(!isEmpty(list))
            {
                cout << endl << "### Error detected in PIPE definition file! ###" << endl
                     << "File: " << file.fileName().toLatin1().constData() << endl
                     << "Keyword: " << list.join(" ").toLatin1().constData() << endl
                     << "Not understood in current context." << endl << endl;

                exit(1);
            }
        }

        if(!ok) break;


        list = processLine(file.readLine());

    }

    // checking remaining input

    if(!ok)
    {
        cout << endl << "### Error detected in PIPE definition file! ###" << endl
             << "File: " << file.fileName().toLatin1().constData() << endl
             << "Definition is incomplete..." << endl
             << "Last line: " << list.join(" ").toLatin1().constData() << endl;

        exit(1);


    }


    // setting the parameters
    bb->setAngle(l_angle);
    bb->setDiameter(l_diameter);
    bb->setGasSpecificGravity(l_sg_gas);
    bb->setLength(l_length);
    bb->setOilDensity(l_den_oil);
    bb->setOilViscosity(l_vis_oil);
    bb->setTemperature(l_temperature);
    bb->setWaterDensity(l_den_wat);
    bb->setWaterViscosity(l_vis_wat);


    return bb;

}

//-----------------------------------------------------------------------------------------------
// Reads the input file, generates a dp table calculator
//-----------------------------------------------------------------------------------------------
DpTableCalculator* PipeReader::readDpTable(QFile &file)
{
    DpTableCalculator *dpc = new DpTableCalculator();

    DpTable *table = new DpTable();

    // starting to read the file
    QStringList list = processLine(file.readLine());   // reading a line from the output file

    while(!file.atEnd() && !list.at(0).startsWith("EOF"))
    {


        // format: gas oil water dp

        if(list.size() == 4)    // the correct number of elements
        {
            // trying to read the numbers
            bool ok_line = true;
            QList<double> nums;

            for(int i = 0; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok_line));
                if(!ok_line) break;
            }

            if(ok_line) // all numbers read ok
            {
                // adding a new row to the vlp table
                table->addRow(nums.at(3), nums.at(0), nums.at(1), nums.at(2));

            }
            else
            {
                cout << endl << "### Error detected in DP table! ###" << endl
                     << "File: " << file.fileName().toLatin1().constData() << endl
                     << "Could not convert line to numbers..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                exit(1);


            }

        }
        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in DP table! ###" << endl
                 << "File: " << file.fileName().toLatin1().constData() << endl
                 << "Line does not have the correct number of entries..." << endl
                 << "Last line: " << list.join(" ").toLatin1().constData() << endl;

            exit(1);

        }


         list = processLine(file.readLine());   // reading a line from the output file




    }

    cout << "Added " << table->numberOfRows() << " rows to the table..." << endl;


    dpc->setDpTable(table);



    return dpc;
}

//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList PipeReader::processLine(const QString& line)
{
    QString temp = line.split("!").at(0);   // removing everything after !
    temp = temp.trimmed();                  // removing trailing and leadnig whitespaces

    QStringList list = temp.split(QRegExp("\\s+"));

    while(list.size() < 2)
    {
        list.push_back(" ");
    }

    return list;
}

//-----------------------------------------------------------------------------------------------
// Checks if the line is empty or not (white spaces)
//-----------------------------------------------------------------------------------------------
bool PipeReader::isEmpty(const QStringList &list)
{
    bool ok = true;

    for(int i = 0; i < list.size(); i++)
    {
        QString temp = list.at(i);

        temp.remove(QRegExp("\\s+"));

        if(temp.size() != 0)
        {
            ok = false;
            break;
        }

    }

    return ok;
}


} // namespace ResOpt
