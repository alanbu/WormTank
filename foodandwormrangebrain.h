#ifndef FOODANDWORMRANGEBRAIN_H
#define FOODANDWORMRANGEBRAIN_H

#include "iobrain.h"

class FoodAndWormRangeBrain : public IOBrain
{
public:
    FoodAndWormRangeBrain(Worm *worm) : IOBrain(worm, 36) {}
    FoodAndWormRangeBrain(const FoodAndWormRangeBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~FoodAndWormRangeBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};


#endif // FOODANDWORMRANGEBRAIN_H
