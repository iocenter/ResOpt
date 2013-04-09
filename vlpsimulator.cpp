#include "vlpsimulator.h"

#include <QFile>
#include <iostream>
#include <QDir>

#include "vlptable.h"
#include "model.h"
#include "reservoir.h"
#include "productionwell.h"
#include "wellcontrol.h"
#include "realvariable.h"
#include "stream.h"

using std::cout;
using std::endl;

namespace ResOpt
{

VlpSimulator::VlpSimulator()
{
}

VlpSimulator::VlpSimulator(const VlpSimulator &v)
    : ReservoirSimulator(v)
{
    for(int i = 0; i < v.m_vlp_tables.size(); ++i) m_vlp_tables.push_back(new VlpTable(*v.m_vlp_tables.at(i)));
}

VlpSimulator::~VlpSimulator()
{
    for(int i = 0; i < m_vlp_tables.size(); ++i) delete m_vlp_tables.at(i);
}


//-----------------------------------------------------------------------------------------------
// Reads a single vlp table from a file
//-----------------------------------------------------------------------------------------------
VlpTable* VlpSimulator::readVlpTable(const QString &well_name, QFile &input)
{


    // starting to read the file

    cout << "Reading vlp table for WELL: " << well_name.toAscii().data() << "..." << endl;


    QStringList list;
    VlpTable *table = new VlpTable();

    table->setWellName(well_name);

    bool ok = true;

    list = processLine(input.readLine());   // reading a line from the output file

    while(!input.atEnd() && !list.at(0).startsWith("END"))
    {


        // format: glift pbh qq go gw

        if(list.size() == 5)    // the correct number of elements
        {
            // trying to read the numbers
            bool ok_line = true;
            QList<double> nums;

            for(int i = 0; i < 5; i++)
            {
                nums.push_back(list.at(i).toDouble(&ok_line));
                if(!ok_line) break;
            }

            if(ok_line) // all numbers read ok
            {
                // adding a new row to the vlp table
                table->addRow(nums.at(0), nums.at(1), nums.at(2), nums.at(3), nums.at(4));

            }
            else
            {
                cout << endl << "### Error detected in VLP table in reservoir input file! ###" << endl
                     << "File: " << input.fileName().toAscii().data() << endl
                     << "Could not convert line to numbers..." << endl
                     << "Last line: " << list.join(" ").toAscii().data() << endl;

                exit(1);


            }

        }
        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in VLP table in reservoir input file! ###" << endl
                 << "File: " << input.fileName().toAscii().data() << endl
                 << "Line does not have the correct number of entries..." << endl
                 << "Last line: " << list.join(" ").toAscii().data() << endl;

            exit(1);

        }


         list = processLine(input.readLine());   // reading a line from the output file




    }

    cout << "Added " << table->numberOfRows() << " rows to the table..." << endl;
    cout << "Done reading vlp table for WELL: " << well_name.toAscii().data() << "..." << endl;

    return table;

}

//-----------------------------------------------------------------------------------------------
// Reads all the vlp tables from the reservoir definition file
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::readInput(const QString &file)
{


    // reading the reservoir file, generating vlp tables

    QFile input(file);

    // checking if file opened ok...
    if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "Could not open reservoir definition file: " << file.toAscii().data() << endl;

        exit(1);
    }


    // starting to read the file

    QStringList list;

    bool ok = true;

    while(!input.atEnd() && ok)
    {
        // format: wellname filename

        list = processLine(input.readLine());   // reading a line from the input file

        if(list.at(0).startsWith("START") && list.at(1).startsWith("VLP"))
        {
            if(list.size() == 3) // correct format
            {
                m_vlp_tables.push_back(readVlpTable(list.at(2), input));
            }
            else
            {
                cout << endl << "### Error detected in reservoir input file! ###" << endl
                     << "Line does not have the correct number of entries..." << endl
                     << "Correct format: START VLPTABLE wellname" << endl
                     << "Last line: " << list.join(" ").toAscii().data() << endl;

                exit(1);


            }
        }


        else if(!isEmpty(list))
        {
            cout << endl << "### Error detected in reservoir input file! ###" << endl
                 << "Keyword not understood..." << endl
                 << "Last line: " << list.join(" ").toAscii().data() << endl;

            exit(1);

        }

    }

    return true;

}

//-----------------------------------------------------------------------------------------------
// Reads the vlp tables if they have not been read yet
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::generateInputFiles(Model *m)
{
    bool ok = true;

    if(numberOfVlpTables() == 0)
    {

        // reservoir input file name:
        QString file_name = folder() + "/" + m->reservoir()->file();

        if(!readInput(file_name)) ok = false;
    }

    return ok;

}

//-----------------------------------------------------------------------------------------------
// finds the vlp table for the well
//-----------------------------------------------------------------------------------------------
VlpTable* VlpSimulator::findVlpTable(const QString &well_name)
{
    for(int i = 0; i < numberOfVlpTables(); ++i)
    {
        if(vlpTable(i)->wellName().compare(well_name) == 0) return vlpTable(i);
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------
// calculates the rates
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::launchSimulator()
{

    // nothing to do here

    return true;
}

//-----------------------------------------------------------------------------------------------
// extracts the resluts
//-----------------------------------------------------------------------------------------------
bool VlpSimulator::readOutput(Model *m)
{
    // looping through the wells
    for(int i = 0; i < m->numberOfWells(); ++i)
    {
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(m->well(i));

        // checking if this is a production well. injection wells are ignored when vlp curves are used.
        if(prod_well != 0)
        {

            // finding the vlp table for the well
            VlpTable *table = findVlpTable(prod_well->name());

            if(table == 0)
            {
                cout << endl << "### Runtime Error ###" << endl
                     << "Could not find the VLP table for well: " << prod_well->name().toAscii().data() << endl
                     << "Does the well name match the name of the VLP table?" << endl << endl;

                exit(1);

            }
            else
            {
                // found the table ok, checking if the well has gas lift

                bool has_gas_lift = prod_well->hasGasLift();

                double pbh = 0;
                double glift = 0;


                // looping through the time steps
                for(int j = 0; j < prod_well->numberOfControls(); ++j)
                {

                    // checking if the well is installed
                    if(prod_well->isInstalled(j))
                    {

                        // updating for this time step
                        pbh = prod_well->control(j)->controlVar()->value();
                        if(has_gas_lift) glift = prod_well->gasLiftControl(j)->controlVar()->value();

                        // interpolating the vlp table
                        Stream *s = table->interpolate(pbh, glift);

                        s->setTime(prod_well->control(j)->endTime());

                        // setting the calculated rates to the well
                        prod_well->setStream(j, s);

                    }
                    else // the well is not installed, generating an empty stream
                    {
                        Stream *s = new Stream(prod_well->control(j)->endTime(), 0, 0, 0, 0);
                        prod_well->setStream(j, s);
                    }
                }
            } // table ok

        } // production well
    } // well

}


//-----------------------------------------------------------------------------------------------
// Splits a line read from the driver file into a list of arguments
//-----------------------------------------------------------------------------------------------
QStringList VlpSimulator::processLine(const QString& line)
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
bool VlpSimulator::isEmpty(const QStringList &list)
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

