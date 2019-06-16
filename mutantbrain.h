#ifndef MUTANTBRAIN_H
#define MUTANTBRAIN_H

#include "iobrain.h"

/**
 * @brief Class with an IOBrain that mutates
 */
class MutantBrain : public IOBrain
{
public:
    MutantBrain(Worm *worm);
    MutantBrain(const MutantBrain &other, Worm *worm);
    virtual ~MutantBrain();

    virtual const std::string &name() const;
    virtual std::string fullName() const;

    virtual Brain *clone(Worm *worm);



    typedef int Sensor;
    enum SensorType
    {
        FoodNextTo = 1,
        WormNextTo,
        WallNextTo,
        FoodSeen,
        WormSeen,
        FoodRange,
        WormRange,
        WallRange,
        FoodCount,
        WormCount,
        FoodBeam,
        WormBeam,
        WallBeam,
        MaxSensors
    };

protected:
    virtual void fillInputs(QPoint pt, int dir);

    virtual void average(Brain *other);
    virtual void mix(Brain *other);
    virtual void randomise(int chance, int weight);
    virtual void write(QDataStream &out);
    virtual void read(QDataStream &in);


    void mutate();

    Sensor randSensor() const;
    inline SensorType sensorType(Sensor sensor) const {return SensorType(sensor >> 16);}
    inline int sensorRange(Sensor sensor) const {return (sensor & 0xFFFF);}
    inline Sensor makeSensor(SensorType type, int range) const {return (type << 16) | range;}
    inline bool sensorHasRange(SensorType type) const {return (type >= FoodRange && type < FoodBeam);}
    bool hasSensorOfType(SensorType type) const;
    uint findSensor(Sensor sensor) const;
    uint sensorInsertPos(Sensor sensor) const;
    void addSensor(Sensor sensor);
    void removeSensor(Sensor sensor);
    void removeSensorAt(uint removeIdx);

protected:
    uint m_numSensors;
    Sensor *m_sensors;
    const uint MaxNumberOfSensors = 20;
};


#endif // MUTANTBRAIN_H
