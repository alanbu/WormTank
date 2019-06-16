#ifndef RANDOM_H
#define RANDOM_H

#include <cstdlib>

/**
* @brief Produce a (sort of) random number in a range
* @param start beginning of range
* @param end end of range
* @return number in the range
*/
inline int randBetween(int start, int end)
{
	return (std::rand() % (end + 1 - start)) + start;
}

/**
 * @brief return a random number from 0 to value - 1
 * @param value top limit (exclusive)
 * @return number from 0 to value -1
 */
inline uint randBelow(uint value)
{
    return uint(std::rand() % value);
}

/**
 * @brief Return a random number between 1 and 100
 */
inline int randPercent()
{
	return (std::rand() % 100) + 1;
}

#endif
