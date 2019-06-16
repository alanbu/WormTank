#ifndef BEAMBRAIN_H
#define BEAMBRAIN_H

#include "iobrain.h"

/**
 * @brief Sends out beams in three directions to detect food, worms or the walls
 */
class BeamBrain : public IOBrain
{
public:
    BeamBrain(Worm *worm) : IOBrain(worm,12) {}
    BeamBrain(const BeamBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~BeamBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};

#endif // BEAMBRAIN_H
