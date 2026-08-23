#ifndef BITFIELD_H
#define BITFIELD_H

/** A dynamically sized array of bits. Range not checked in operations. */
struct Bitfield;

/** Allocate a bitfield with (at least) the given (positive) number of bits. 
 * bits must be less than 2^19.
 * All bit start out as 0. 
 * Free with free(). 
 */
struct Bitfield* allocateBitfield(unsigned long bits);

/** Set a non-empty run of bits to 1. Overflow not checked. */
void setBits(struct Bitfield* bitfield, unsigned long firstBit, unsigned long numBits);

/** Find the first zero bit in non-empty [firstBit, endBit). If none found, returns endBit. Overflow not checked. */
unsigned long scanForZeroBit(struct Bitfield const* bitfield, unsigned long firstBit, unsigned long endBit);

/** Find the first one bit in non-empty [firstBit, endBit). If none found, returns endBit. Overflow not checked. */
unsigned long scanForOneBit(struct Bitfield const* bitfield, unsigned long firstBit, unsigned long endBit);

#endif
