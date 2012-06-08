#ifndef MODELREADER_H
#define MODELREADER_H

#include <QString>
#include <QStringList>
#include <QFile>

class Model;
class Well;
class ProductionWell;
class Reservoir;
class Pipe;
class MidPipe;
class Objective;
class Separator;

class ModelReader
{
private:

    QFile m_driver_file;

    QStringList processLine(const QString& line);
    bool isEmpty(const QStringList &list);

    /**
     * @brief Reads a RESERVOIR section of the driver file.
     * @details Generates a Reservoir object based on the information in the driver file.
     *  This object should be sent to a Model for storage.
     *
     * @return Reservoir Pointer to the generated Reservoir
     */
    Reservoir* readReservoir();


    /**
     * @brief  Reads a WELL section of the driver file
     *
     * @return Well
     */
    Well* readWell();


    /**
     * @brief Reads the SCHEDULE part of a well definition.
     *
     * @param w pointer to the well where the schedule should be added
     * @return true if everything went ok, false if errors were enocuntered
     */
    bool readWellSchedule(Well *w);


    /**
     * @brief Reads the CONNECTIONS part of a well definition
     *
     * @param w pointer to the well where the connections should be added
     * @return true if everything went ok, false if errors were enocuntered
     */
    bool readWellConnections(Well *w);


    /**
     * @brief Reads the OUTLETPIPES part of a well definition
     *
     * @param w
     * @return bool
     */
    bool readPipeConnections(ProductionWell *w);


    /**
     * @brief Reads the OUTLETPIPES part of a pipe definition
     *
     * @param p
     * @return bool
     */
    bool readPipeConnections(MidPipe *p);


    /**
     * @brief Reads the objective definition in the driver file
     *
     * @return Objective
     */
    Objective* readObjective();


    /**
     * @brief Reads a PIPE definition from the driver file
     *
     * @return Pipe
     */
    Pipe* readPipe();


    /**
     * @brief Reads a SEPARATOR definition from the driver file
     *
     * @return Separator
     */
    Separator* readSeparator();

public:

    ModelReader(const QString &driver);

    /**
     * @brief Reads the driver file, and sets up all the data structures
     *
     */
    Model* readDriverFile();


};

#endif // MODELREADER_H
