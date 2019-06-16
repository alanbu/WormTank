#ifndef FOODRANGEBRAIN_H
#define FOODRANGEBRAIN_H

#include "iobrain.h"

class FoodRangeBrain : public IOBrain
{
public:
    FoodRangeBrain(Worm *worm) : IOBrain(worm, 24) {}
    FoodRangeBrain(const FoodRangeBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~FoodRangeBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};


#endif // FOODRANGEBRAIN_H
