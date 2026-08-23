#include <stdlib.h>
#include <assert.h>

#include "bitfield.h"

/* To be compiled on a platform with 16-bit ints */

struct Bitfield
{
    /** Actual array length is dynamic */
    unsigned int bits[1];
};

struct Bitfield* allocateBitfield(unsigned long bits)
{
    return calloc((bits+15) >> 4, 2);
}

void setBits(struct Bitfield* bitfield, unsigned long firstBit, unsigned long numBits)
{
    unsigned long wordIndex = firstBit >> 4;
    unsigned long lastWordIndex = (firstBit+numBits-1) >> 4;
    unsigned long endBit = firstBit+numBits;
    /* Mask off too-low bits in first word. */
    unsigned int bitsToSet = 0xFFFF << (firstBit & 0xF);
    while(wordIndex < lastWordIndex)
    {
        bitfield->bits[wordIndex] |= bitsToSet;
        /* Don't mask off too-low bits after the first word */
        bitsToSet = 0xFFFF;
        ++wordIndex;
    }
    if(endBit & 0xF)
    {
        /* Mask off too-high bits in last word (which may also be the first word). */
        bitsToSet &= (unsigned)0xFFFF >> (unsigned)(16 - (endBit & 0xF));
    }
    bitfield->bits[wordIndex] |= bitsToSet;
}

/** returns the zero-indexed position of the least-significant 1 bit in the nonzero word. */
static unsigned char lowestSetBit(unsigned int word)
{
    unsigned char bit = 0;

    if(!(word & 0x00FF))
    {
        bit += 8;
        word >>= 8;
    }
    if(!(word & 0x000F))
    {
        bit += 4;
        word >>= 4;
    }
    if(!(word & 0x0003))
    {
        bit += 2;
        word >>= 2;
    }
    if(!(word & 0x0001))
    {
        ++bit;
    }
    return bit;
}

unsigned long scanForZeroBit(struct Bitfield const* bitfield, unsigned long firstBit, unsigned long endBit)
{
    unsigned int wordIndex = firstBit >> 4;
    unsigned int lastWordIndex = (endBit-1) >> 4;
    unsigned int bitsToCheck = 0xFFFF << (firstBit & 0xF);
    unsigned int word;

    while(wordIndex <= lastWordIndex)
    {
        if(wordIndex == lastWordIndex && (endBit & 0xF))
        {
            bitsToCheck &= (unsigned)0xFFFF >> (unsigned)(16 - (endBit & 0xF));
        }
        word = bitfield->bits[wordIndex];
        if(~word & bitsToCheck)
        {
            return (unsigned long)wordIndex << 4 | lowestSetBit(~word & bitsToCheck);
        }
        bitsToCheck = 0xFFFF;
        wordIndex++;
    }
    return endBit;
}

unsigned long scanForOneBit(struct Bitfield const* bitfield, unsigned long firstBit, unsigned long endBit)
{
    unsigned int wordIndex = firstBit >> 4;
    unsigned int lastWordIndex = (endBit-1) >> 4;
    unsigned int bitsToCheck = 0xFFFF << (firstBit & 0xF);
    unsigned int word;

    while(wordIndex <= lastWordIndex)
    {
        if(wordIndex == lastWordIndex && (endBit & 0xF))
        {
            bitsToCheck &= (unsigned)0xFFFF >> (unsigned)(16 - (endBit & 0xF));
        }
        word = bitfield->bits[wordIndex];
        if(word & bitsToCheck)
        {
            return (unsigned long)wordIndex << 4 | lowestSetBit(word & bitsToCheck);
        }
        bitsToCheck = 0xFFFF;
        wordIndex++;
    }
    return endBit;
}
