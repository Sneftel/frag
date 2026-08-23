#ifndef DOSUTIL_H
#define DOSUTIL_H

/* Reads a number of sectors from a partition. */
void absread(unsigned char driveNumber, unsigned int numSectors, unsigned long startSector, void* dataOut);

/* Writes a number of sectors to a partition. */
void abswrite(unsigned char driveNumber, unsigned int numSectors, unsigned long startSector, void* dataIn);

/* Writes some bytes to anywhere on a partition. */
void abswritesmall(unsigned char driveNumber, unsigned int numBytes, unsigned long sector, unsigned int offset, void* dataIn);

/* Get the total number of clusters (including reserved clusters) for a partition. */
unsigned int getNumClusters(unsigned char driveNumber);

void diskReset();

#endif

