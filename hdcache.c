#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "hdcache.h"
#include "util.h"
#include "dosutil.h"

static unsigned char cachedDriveNumber = 0xFF;
static unsigned long cachedSectorId = UINT32_MAX;
static unsigned char cachedSector[SECTOR_SIZE];

void cacheWrite(unsigned char driveNumber, unsigned int numBytes, unsigned long sectorId, unsigned int offset, void const* dataIn)
{
    assert(driveNumber != 0xFF);
    assert(sectorId != UINT32_MAX);
    assert(offset+numBytes <= SECTOR_SIZE);

    if(sectorId != cachedSectorId || driveNumber != cachedDriveNumber)
    {
        cacheFlush();
        absread(driveNumber, 1, sectorId, cachedSector);
        cachedDriveNumber = driveNumber;
        cachedSectorId = sectorId;
    }
    memcpy(cachedSector+offset, dataIn, numBytes);
}

void cacheFlush(void)
{
    if(cachedSectorId != UINT32_MAX)
    {
        abswrite(cachedDriveNumber, 1, cachedSectorId, cachedSector);
        cachedSectorId = UINT32_MAX;
        cachedDriveNumber = 0xFF;
    }
}
