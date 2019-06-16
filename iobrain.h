#ifndef IOBRAIN_H
#define IOBRAIN_H

#include "brain.h"
#include "braincell.h"

/**
 * @brief Simple base class for a brain with multiple inputs
 * attached to two outputs
 */
class IOBrain : public Brain
{
public:
    IOBrain(Worm *worm, uint numInputs);
    IOBrain(const IOBrain &other, Worm *worm);
    virtual ~IOBrain();

    virtual void write(QDataStream &out);
    virtual void read(QDataStream &in);

    virtual void planMove(QPoint &pt, int &dir);

    virtual void randomise(int chance, int weight);
    virtual void average(Brain *other);
    virtual void mix(Brain *other);


protected:
    /**
     * @brief Fill the inputs with the data from the worms sensors
     * @param pt The head position of the worm
     * @param dir the direction the worm is facing
     */
    virtual void fillInputs(QPoint pt, int dir) = 0;

protected:
    uint m_numInputs;
    InputBrainCell *m_inputs;
    LinkedBrainCell m_outputs[2];
};

#endif // IOBRAIN_H
