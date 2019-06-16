#ifndef FOODANDWORMCOUNTBRAIN_H
#define FOODANDWORMCOUNTBRAIN_H

#include "iobrain.h"

class FoodAndWormCountBrain : public IOBrain
{
public:
    FoodAndWormCountBrain(Worm *worm) : IOBrain(worm, 36) {}
    FoodAndWormCountBrain(const FoodAndWormCountBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~FoodAndWormCountBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};


#endif // FOODANDWORMCOUNTBRAIN_H
