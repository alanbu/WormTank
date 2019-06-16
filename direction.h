#ifndef DIRECTION_H
#define DIRECTION_H

/**
 * @brief get direction from 0,0 to the given position
 * @param x
 * @param y
 * @return direction (0 north, 1 east, 2 south, 3 west)
 */
inline int direction(int x, int y)
{
    if (y >= 0)
    {
        if (x > y) return 1;
        else if (x < -y) return 3;
        else return 0;
    }
    else if (x < y) return 3;
    else if (x > -y) return 1;
    else return 2;
}

/**
 * @brief move a point in the given direction
 * @param pt point to move
 * @param dir direction
 */
inline void movePoint(QPoint &pt, int dir)
{
    switch (dir)
    {
    case 0: pt.ry()++; break;
    case 1: pt.rx()++; break;
    case 2: pt.ry()--; break;
    case 3: pt.rx()--; break;
    }
}

#endif // DIRECTION_H

