#ifndef FOODCOUNTBRAIN_H
#define FOODCOUNTBRAIN_H

#include "iobrain.h"

class FoodCountBrain : public IOBrain
{
public:
    FoodCountBrain(Worm *worm) : IOBrain(worm, 24) {}
    FoodCountBrain(const FoodCountBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~FoodCountBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};


#endif // FOODCOUNTBRAIN_H
