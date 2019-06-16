#ifndef SIMPLEWITHNEXTTOBRAIN_H
#define SIMPLEWITHNEXTTOBRAIN_H

#include "iobrain.h"

/**
 * @brief Simple brain that detects food direction and if its next to it
 */
class SimpleWithNextToBrain : public IOBrain
{
public:
    SimpleWithNextToBrain(Worm *worm) : IOBrain(worm,16) {}
    SimpleWithNextToBrain(const SimpleWithNextToBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~SimpleWithNextToBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};

#endif // SIMPLEWITHNEXTTOBRAIN_H
