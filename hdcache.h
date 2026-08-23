#ifndef HDCACHE_H
#define HDCACHE_H

/* Write to a one-sector write-back disk cache. */
void cacheWrite(unsigned char driveNumber, unsigned int numBytes, unsigned long sectorId, unsigned int offset, void const* dataIn);

/* Flush the cached sector, if any, to disk. */
void cacheFlush(void);

#endif
