
#include <dos.h>
#include <assert.h>
#include <string.h>

#include "dosutil.h"
#include "util.h"

#pragma pack(push, 1)
typedef struct AbsDiskPacket
{
    unsigned long startSector;
    unsigned short numSectors;
    void __far* data;
} AbsDiskPacket;
#pragma pack(pop)

static int absReadInterrupt(unsigned char driveNumber, const AbsDiskPacket __far* packet);
#pragma aux absReadInterrupt = \
    "push ds" \
    "push bp" \
    "push es" \
    "pop ds" \
    "mov cx, 0ffffh" \
    "int 25h" \
    "sbb dx, dx" \
    "popf" \
    "pop bp" \
    "pop ds" \
    "and ax, dx" \
    parm [al] [es bx] \
    value [ax] \
    modify exact [ax bx cx dx si di];

static int absWriteInterrupt(unsigned char driveNumber, const AbsDiskPacket __far* packet);
#pragma aux absWriteInterrupt = \
    "push ds" \
    "push bp" \
    "push es" \
    "pop ds" \
    "mov cx, 0ffffh" \
    "int 26h" \
    "sbb dx, dx" \
    "popf" \
    "pop bp" \
    "pop ds" \
    "and ax, dx" \
    parm [al] [es bx] \
    value [ax] \
    modify exact [ax bx cx dx si di];

void absread(unsigned char driveNumber, unsigned int numSectors, unsigned long startSector, void* dataOut)
{
    AbsDiskPacket packet;
    int res;

    packet.startSector = startSector;
    packet.numSectors = (unsigned short)numSectors;
    packet.data = dataOut;
    res = absReadInterrupt(driveNumber, &packet);
    assert(res == 0);
}


void abswrite(unsigned char driveNumber, unsigned int numSectors, unsigned long startSector, void* dataIn)
{
    AbsDiskPacket packet;
    int res;

    packet.startSector = startSector;
    packet.numSectors = (unsigned short)numSectors;
    packet.data = dataIn;
    res = absWriteInterrupt(driveNumber, &packet);
    assert(res == 0);
}

void abswritesmall(unsigned char driveNumber, unsigned int numBytes, unsigned long sectorNum, unsigned int offset, void* dataIn)
{
    unsigned char sectorBytes[SECTOR_SIZE];

    assert(offset+numBytes <= SECTOR_SIZE);

    absread(driveNumber, 1, sectorNum, sectorBytes);
    memcpy(sectorBytes+offset, dataIn, numBytes);
    abswrite(driveNumber, 1, sectorNum, sectorBytes);
}

void getClusterInfo(unsigned char driveNumber, unsigned int* totalClustersOut, unsigned int* freeClustersOut)
{
    union REGPACK regs;
    regs.h.dl = driveNumber+1;
    regs.h.ah = 0x36;
    intr(0x21, &regs);
    assert(regs.w.ax != 0xFFFFu);
    *totalClustersOut = regs.w.dx;
    *freeClustersOut = regs.w.bx;
}
