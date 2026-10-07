#pragma once
#include <cstddef>

/**
 * One job for the pool: test a chunk of odd divisors against one number.
 * If end is less than start, the chunk is empty and there is nothing to test.
 * That happens when the number has fewer divisors to test than there are threads.
 */
struct Task{
    /** The first divisor in the chunk (odd, inclusive). */
    std::size_t start   = 0;
    /** The last divisor in the chunk (inclusive). */
    std::size_t end     = 0;
    /** The number being tested for primality. */
    std::size_t n       = 0;
};