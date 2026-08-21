#include <assert.h>
#include <dos.h>
#include <stdlib.h>

#include "rawwrite.h"

#define SECTOR_SIZE 512

static char sectorBuf[SECTOR_SIZE];

static int drive = -1;
static unsigned long curSector = ULONG_MAX;
static unsigned int writePos = 0;

void rw_selectDrive(unsigned char dosDriveNumber)
{
    drive = dosDriveNumber;
}

void rw_flush()
{
    int res;
    if(writePos == 0) return;
    res = abswrite(drive, 1, curSector, sectorBuf);
    assert(res == 0);
    writePos = 0;
}

/* size must be less than 64k sectors */
void rw_write(char* data, unsigned long size)
{
    unsigned long offset = 0;
    unsigned long bytesToWrite;
    unsigned long sectorsToWrite;
    int res;

    assert(curSector != ULONG_MAX);

    if(size == 0) return;

    /* Fill the rest of the current sector's buffer, if it's partially full */
    if(writePos > 0)
    {
        bytesToWrite = min(size, SECTOR_SIZE-writePos);
        memcpy(sectorBuf+writePos, data, bytesToWrite);
        offset = bytesToWrite;
    }

    if(writePos == SECTOR_SIZE)
    {
        rw_flush();
        curSector++;
    }

    assert(offset <= size);
    if(offset == size) return;

    /* Directly write full sectors */
    sectorsToWrite = (size-offset) / SECTOR_SIZE;
    assert(sectorsToWrite < UINT_MAX);
    if(sectorsToWrite > 0)
    {
        res = abswrite(drive, sectorsToWrite, curSector, data+offset);
        assert(res == 0);
        curSector += sectorsToWrite;
        offset += sectorsToWrite * SECTOR_SIZE;
    }

    assert(size-offset < SECTOR_SIZE);
    assert(writePos == 0);

    assert(offset <= size);
    if(offset == size) return;

    /* Put remainder into buffer */
    assert(writePos == 0);
    memcpy(sectorBuf, data+offset, size-offset);
    writePos = size-offset;
}
