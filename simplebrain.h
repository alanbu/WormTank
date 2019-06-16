#ifndef SIMPLEBRAIN_H
#define SIMPLEBRAIN_H

#include "iobrain.h"

class SimpleBrain : public IOBrain
{
public:
    SimpleBrain(Worm *worm) : IOBrain(worm,4) {}
    SimpleBrain(const SimpleBrain &other, Worm *worm) : IOBrain(other, worm) {}
    virtual ~SimpleBrain(){}

    virtual const std::string &name() const;
    virtual Brain *clone(Worm *worm);

protected:
    virtual void fillInputs(QPoint pt, int dir);
};

#endif // SIMPLEBRAIN_H
