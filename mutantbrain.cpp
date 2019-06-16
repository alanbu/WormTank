#include "mutantbrain.h"
#include "wormtank.h"
#include "worm.h"
#include "random.h"
#include "braincell.h"
#include <sstream>

// Register the simple brain creator
BrainCreator<MutantBrain> g_mutantBrainCreator(
        "Mutant",
        "Brain that has a random selection of sensors and can randomly add/remove them"
        " when generating a new generation."
        );

/**
 * @brief MutantBrain constructor
 *
 * Builds a brain that starts with 1 random sensor
 *
 * @param worm worm to contain the brain
 */
MutantBrain::MutantBrain(Worm *worm) :
    IOBrain(worm, 4),
    m_numSensors(0),
    m_sensors(nullptr)
{
    m_numSensors = m_numInputs/4;
    m_sensors = new Sensor[m_numSensors];
    m_sensors[0] = randSensor();
}

MutantBrain::MutantBrain(const MutantBrain &other, Worm *worm)
    : IOBrain(other, worm),
      m_sensors(nullptr)
{
    m_numSensors = other.m_numSensors;
    m_sensors = new Sensor[m_numSensors];
    for (uint j = 0; j < m_numSensors; j++)
        m_sensors[j] = other.m_sensors[j];
}

MutantBrain::~MutantBrain()
{
    delete [] m_sensors;
}


/**
 * @brief Return the name of this brain type
 * @return name of brain type
 */
const std::string &MutantBrain::name() const
{
    return g_mutantBrainCreator.name();
}

/**
 * @brief Return mutant brains full name
 *
 * The full name for a mutant brain contains a suffix with a coded list
 * of its sensors
 *
 * @return calculated full name
 */
std::string MutantBrain::fullName() const
{
    std::ostringstream name;
    name << g_mutantBrainCreator.name();
    name << "_";

    Sensor *sensor = m_sensors;
    Sensor *sensorEnd = m_sensors + m_numSensors;
    while (sensor != sensorEnd)
    {
        SensorType type = sensorType(*sensor);
        bool addRanges = false;
        switch(type)
        {
        case FoodNextTo: name << "Fn"; break;
        case WormNextTo: name << "Wn"; break;
        case WallNextTo: name << "En"; break;
        case FoodSeen:   name << "Fs"; break;
        case WormSeen:   name << "Ws"; break;
        case FoodRange:  name << "Fr"; addRanges = true; break;
        case WormRange:  name << "Wr"; addRanges = true; break;
        case WallRange:  name << "Er"; addRanges = true; break;
        case FoodCount:  name << "Fc"; addRanges = true; break;
        case WormCount:  name << "Fc"; addRanges = true; break;
        case MaxSensors: name << "X"; break; // Shouldn't happen
        }
        if (addRanges)
        {
            name << sensorRange(*sensor++);
            while (sensor != sensorEnd && sensorType(*sensor) == type)
            {
                name << "," << sensorRange(*sensor++);
            }
        } else
        {
            sensor++;
        }
    }

    return name.str();
}


/**
 * @brief Return a randomly generater sensor
 *
 * @return new sensor
 */
MutantBrain::Sensor MutantBrain::randSensor() const
{
    SensorType type = SensorType(randBetween(1, MaxSensors - 1));
    int range = sensorHasRange(type) ? randBetween(1, m_worm->tank()->maxDistance()) : 0;
    return makeSensor(type,range);
}

/**
 * @brief Check if brain contains sensor of specific type
 * @param type SensorType to check
 * @return true if a sensor of that type exists
 */
bool MutantBrain::hasSensorOfType(SensorType type) const
{
   uint j = 0;
   while (j < m_numSensors && type >= sensorType(m_sensors[j])) j++;
   if (j == m_numSensors) return false;
   return (type == sensorType(m_sensors[j]));
}


/**
 * @brief Find a sensor
 * @param sensor to find
 * @return position of sensors or m_numSensors if not found
 */
uint MutantBrain::findSensor(Sensor sensor) const
{
    uint j = 0;
    while (j < m_numSensors && sensor > m_sensors[j]) j++;
    if (j < m_numSensors && sensor != m_sensors[j]) j = m_numSensors;
    return j;
}

/**
 * @brief Get position to insert a sensor
 * @param sensor to insert
 * @return position to insert
 */
uint MutantBrain::sensorInsertPos(Sensor sensor) const
{
    uint j = 0;
    while (j < m_numSensors && sensor > m_sensors[j]) j++;
    return j;
}

/**
 * @brief Clone a copy of this MutantBrain
 * @param worm worm to attach clone to
 * @return new Brain copy
 */
Brain *MutantBrain::clone(Worm *worm)
{
    return new MutantBrain(*this, worm);
}

void MutantBrain::write(QDataStream &out)
{
    out << m_numSensors;
    std::map<BrainCell *, int> linkMap;
    for (uint j = 0; j < 2; j++) m_outputs[j].writeLinks(out, linkMap);
    for (uint j = 0; j < m_numSensors; j++) out << m_sensors[j];
}

void MutantBrain::read(QDataStream &in)
{
    in >> m_numSensors;
    if (m_numInputs != m_numSensors * 4)
    {
        delete [] m_inputs;
        m_numInputs = m_numSensors * 4;
        m_inputs = new InputBrainCell[m_numInputs];
        delete [] m_sensors;
        m_sensors = new int[m_numSensors];
    }
    BrainCell *linkArray[m_numInputs];
    for (uint j = 0; j < m_numInputs; j++) linkArray[j] = &m_inputs[j];
    for (uint j = 0; j < 2; j++) m_outputs[j].readLinks(in, linkArray);
    for (uint j = 0; j < m_numSensors; j++) in >> m_sensors[j];
}


/**
 * @brief Fill the inputs with the food seen sensor data
 * @param pt worm head position
 * @param dir directin worm is facing
 */
void MutantBrain::fillInputs(QPoint pt, int dir)
{
    int ranges[m_numSensors];
    uint numRanges = 0;
    uint j = 0;
    SensorType lastSensorType = sensorType(m_sensors[0]);
    ranges[numRanges++] = sensorRange(m_sensors[0]);
    j++;

    InputBrainCell *inputs = m_inputs;

    while (j <= m_numSensors)
    {
       SensorType type = (j < m_numSensors) ? sensorType(m_sensors[j]) : MaxSensors;
       if (type == lastSensorType)
       {
           ranges[numRanges++] = sensorRange(m_sensors[j]);
       } else
       {
           switch(lastSensorType)
           {
           case FoodNextTo:
               m_worm->tank()->foodNextTo(pt, dir, inputs);
               break;
           case WormNextTo:
               m_worm->tank()->wormNextTo(pt, dir, inputs);
               break;
           case WallNextTo:
               m_worm->tank()->wallNextTo(pt, dir, inputs);
               break;
           case FoodSeen:
               m_worm->tank()->foodSeen(pt, dir, inputs );
               break;
           case WormSeen:
               m_worm->tank()->wormSeen(pt, dir, inputs );
               break;
           case FoodRange:
               m_worm->tank()->foodSeen(pt, dir, ranges, numRanges, inputs );
               break;
           case WormRange:
               m_worm->tank()->wormSeen(pt, dir, ranges, numRanges, inputs );
               break;
           case WallRange:
               m_worm->tank()->wallSeen(pt, dir, ranges, numRanges, inputs );
               break;
           case FoodCount:
               m_worm->tank()->foodCount(pt, dir, ranges, numRanges, inputs );
               break;
           case WormCount:
               m_worm->tank()->wormCount(pt, dir, ranges, numRanges, inputs );
               break;
           case MaxSensors:
               // Should never get here
               Q_ASSERT(false);
               break;
           }
           lastSensorType = type;
           inputs += 4;
           numRanges = 0;
       }
       j++;
    }
}

/**
 * @brief Make this brain an average of this and another
 *
 * @param other brain to average with
 */
void MutantBrain::average(Brain *other)
{
    MutantBrain *otherBrain = dynamic_cast<MutantBrain *>(other);
    if (otherBrain == nullptr) return; // Can only work with another MutantBrain

	Sensor *sensor = m_sensors, *sensorEnd = m_sensors + m_numSensors;
    Sensor *otherSensor = otherBrain->m_sensors, *otherSensorEnd = otherBrain->m_sensors + otherBrain->m_numSensors;
	
    std::vector<std::pair<Sensor, Sensor>> averageList;
	
	while (sensor != sensorEnd && otherSensor != otherSensorEnd)
	{
		SensorType type = SensorType(*sensor);
		if (*sensor == *otherSensor)
		{
            averageList.push_back(std::make_pair(*sensor, *otherSensor));
			sensor++;
			otherSensor++;
		}
		else if (type == sensorType(*otherSensor))
		{
			if (sensorHasRange(type))
			{
				int prevRange = 0;
				do
				{
					int myRange = sensorRange(*sensor);
					int otherRange = sensorRange(*otherSensor);
					Sensor*nextSensor = sensor + 1;
                    int nextRange = 99999999;
					if (nextSensor != sensorEnd && SensorType(*nextSensor) == SensorType(*sensor))
					{
                        nextRange = sensorRange(*nextSensor);
					}
					int newRange = (myRange + otherRange) / 2;
					if (newRange < prevRange)
					{
						otherSensor++;
					}
					else if (newRange > nextRange)
					{
						sensor++;
						prevRange = myRange;
					}
					else
					{
						*sensor = makeSensor(SensorType(*sensor), newRange);
						prevRange = newRange;
						averageList.push_back(std::make_pair(*sensor, *otherSensor));
						sensor++;
						otherSensor++;
					}
				} while (sensor != sensorEnd
					&& otherSensor != otherSensorEnd
					&& SensorType(*sensor) == type
					&& SensorType(*otherSensor) == type
					);
			}
			else
			{
				sensor++;
                otherSensor++;
			}
		}
		else
		{
			if (type < sensorType(*otherSensor)) sensor++;
			else otherSensor++;
		}
	}

	for (auto averageFrom : averageList)
	{
        uint myIdx = findSensor(averageFrom.first);
        uint otherIdx = otherBrain->findSensor(averageFrom.second);

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 4; k++)
			{
                m_outputs[j].averageWeight(&m_inputs[myIdx * 4 + k],
                    otherBrain->m_outputs[j].getWeight(&(otherBrain->m_inputs[otherIdx * 4 + k]))
					);
			}
		}
	}
}

/**
 * @brief Mix this brain with another
 *
 * @param other brain to mix with
 */
void MutantBrain::mix(Brain *other)
{
    MutantBrain *otherBrain = dynamic_cast<MutantBrain *>(other);
    if (otherBrain == nullptr) return; // Can only work with another MutantBrain

	// Mix the sensors
    uint minSensors = std::min(m_numSensors, otherBrain->m_numSensors);
	std::vector<Sensor> removeList;
	std::vector<Sensor> addList;
	for (uint j = 0; j < minSensors; j++)
	{
		if (std::rand() & 1)
		{
			removeList.push_back(m_sensors[j]);
            addList.push_back(otherBrain->m_sensors[j]);
		}
	}

	for (auto sensor : removeList)
	{
		removeSensor(sensor);
	}
	for (auto sensor : addList)
	{
		addSensor(sensor);
        int otherIdx = otherBrain->findSensor(sensor);
		int myIdx = findSensor(sensor);
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 4; k++)
			{
                m_outputs[j].setWeight(&m_inputs[myIdx * 4 + k],
                    otherBrain->m_outputs[j].getWeight(&(otherBrain->m_inputs[otherIdx * 4 + k]))
					);
			}
		}
	}
}

/**
 * @brief randomise a brain
 *
 * Alongside randomizing weight, the mutant brain also has a
 * 10% chance of adding or removing a sensor
 *
 * @param chance chance a weight it randomized
 * @param weight amount weight is changed (+/- weight)
 */
void MutantBrain::randomise(int chance, int weight)
{
    if (randPercent() <= 10) mutate();

    for (int j = 0; j < 2; j++)
    {
        m_outputs[j].randomise(chance, weight);
    }
}

/**
 * @brief Mutates a brain by adding or removing a sensor
 */
void MutantBrain::mutate()
{
    bool add = (std::rand() & 1) == 1;
    if (add)
    {
        if (m_numSensors == MaxNumberOfSensors) add = false;
    } else
    {
        if (m_numSensors == 1) add = true;
    }

    if (add) addSensor(randSensor());
    else removeSensorAt(randBelow(m_numSensors));
}

/**
 * @brief Add a new sensor
 *
 * Does nothing if the generated sensor is already in the brain
 */
void MutantBrain::addSensor(Sensor newSensor)
{
    uint addIdx = sensorInsertPos(newSensor);
    // Only add new sensors
    if (addIdx < m_numSensors && m_sensors[addIdx] == newSensor) return;

    InputBrainCell *newInputs = new InputBrainCell[m_numInputs + 4];
    Sensor *newSensors = new Sensor[m_numSensors+1];

    uint j;
    for (j = 0; j < addIdx; j++)
    {
        newSensors[j] = m_sensors[j];
    }
    for (j = addIdx; j < m_numSensors; j++)
    {
        newSensors[j+1] = m_sensors[j];
    }
    newSensors[addIdx] = newSensor;

    // Copy inputs
    for (j = 0; j < addIdx * 4; j++)
    {
        newInputs[j] = m_inputs[j];
    }
    for (j = addIdx * 4; j < m_numInputs; j++)
    {
        newInputs[j+4] = m_inputs[j];
    }

    // Fix up output links
    for (j = 0; j < 2; j++)
    {
        if (addIdx > 0)
        {
            m_outputs[j].renameLinks(newInputs, m_inputs, m_inputs + addIdx * 4 - 1);
        }
        if (addIdx < m_numInputs)
        {
            m_outputs[j].renameLinks(newInputs + (addIdx+1) * 4 - 1,
                                     m_inputs + addIdx * 4,
                                     m_inputs + m_numInputs * 4 - 1);
        }
    }

    // Add new links
    for (j = 0; j < 2; j++)
    {
        for (int k = 0; k < 4; k++)
        {
            m_outputs[j].addLink(&newInputs[addIdx * 4 + k], randBetween(-5,5));
        }
    }

    delete [] m_inputs;
    delete [] m_sensors;
    m_inputs = newInputs;
    m_sensors = newSensors;
    m_numInputs += 4;
    m_numSensors++;
}

/**
 * @brief remove the given sensor
 *
 * Removes only if it is in the last of sensors otherwise does nothing
 *
 * @param sensor the sensor to remove
 */
void MutantBrain::removeSensor(Sensor sensor)
{
	uint removeIdx = sensorInsertPos(sensor);
	if (removeIdx < m_numSensors && m_sensors[removeIdx] == sensor)
	{
		removeSensorAt(removeIdx);
	}
}

/**
 * @brief Remove a sensor given its index
 * @param removeIdx index of sensor to remove
 */
void MutantBrain::removeSensorAt(uint removeIdx)
{
    // Remove links to input cells and rename to move down
    for (int j = 0; j < 2; j++)
    {
        InputBrainCell *removeStart = m_inputs + 4 * removeIdx;
        InputBrainCell *removeEnd = removeStart + 4;
        m_outputs[j].removeLinks(removeStart, removeEnd);
        if (removeIdx < m_numSensors - 1)
        {
            m_outputs[j].renameLinks(removeStart, removeEnd, m_inputs + m_numSensors * 4 - 1);
        }
    }

    m_numSensors--;
    // Shift the input values left to fill the gap created
    m_numInputs-=4;
    for (uint j = removeIdx * 4; j < m_numInputs; j++)
    {
        m_inputs[j].setValue(m_inputs[j+4].value());
    }
}
