#ifndef SIMPLEWITHNEXTTOFOOD_H
#define SIMPLEWITHNEXTTOFOOD_H

#include "iobrain.h"

/**
 * @brief Simple brain that detects food direction and if its next to it
 */
class SimpleWithNextToFoodBrain : public IOBrain
{
public:
    SimpleWithNextToFoodBrain(Worm *worm) : IOBrain(worm,8) {}
    SimpleWithNextToFoodBrain(const SimpleWithNextToFoodBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~SimpleWithNextToFoodBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};

#endif // SIMPLEWITHNEXTTOFOOD_H
