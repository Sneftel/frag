#include "drvinfo.h"
#include "dosutil.h"
#include "structs.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>

static int getNumAssignedDirectoryEntries(struct BootSector* bootSector, struct DirectoryEntry* directoryEntries)
{
    int i;

    for(i = 0; i < bootSector->rootEntries; i++)
    {
        if(directoryEntries[i].filename[0] == 0)
        {
            break;
        }
    }

    return i;
}

static int countFileClusters(unsigned int* fat, struct DirectoryEntry* directoryEntry)
{
    int numClusters = 0;
    unsigned int cluster;

    if(directoryEntry->filename[0] != 0xE5)
    {
        cluster = directoryEntry->firstCluster;

        while(cluster < 0xFFF8)
        {
            assert(cluster >= 0x3 && cluster != 0xFFF7);
            numClusters++;
            cluster = fat[cluster];
        }
    }

    return numClusters;
}

static int isDirectory(struct DirectoryEntry* directoryEntry)
{
    return directoryEntry->attributes & (1<<4);
}

struct DriveInfo getDriveInfo(unsigned char driveNumber)
{
    struct DriveInfo driveInfo;
    long fileInfoSectorBase;
    struct DirectoryEntry* directoryEntries;
    struct BootSector bootSector;
    int i;

    driveInfo.driveNumber = driveNumber;

    getClusterInfo(driveNumber, &driveInfo.numClusters, &driveInfo.numFreeClusters);

    absread(driveNumber, 1, 0, &bootSector);

    driveInfo.fat = malloc(SECTOR_SIZE * bootSector.sectorsPerFat);
    assert(driveInfo.fat);
    absread(driveNumber, bootSector.sectorsPerFat, 1, driveInfo.fat);

    driveInfo.numDirectoryEntries = bootSector.rootEntries;
    fileInfoSectorBase = 1 + bootSector.fatCopies * bootSector.sectorsPerFat;
    directoryEntries = malloc(sizeof(struct DirectoryEntry) * bootSector.rootEntries);
    assert(directoryEntries);
    absread(driveNumber, calcRootDirectorySectors(bootSector.rootEntries), fileInfoSectorBase, directoryEntries);

    driveInfo.numAssignedDirectoryEntries = getNumAssignedDirectoryEntries(&bootSector, directoryEntries);
    driveInfo.assignedDirectoryEntrySizesInClusters = malloc(sizeof(unsigned int) * driveInfo.numAssignedDirectoryEntries);
    assert(driveInfo.assignedDirectoryEntrySizesInClusters);
    for(i=0; i<driveInfo.numAssignedDirectoryEntries; i++)
    {
        assert(!isDirectory(directoryEntries+i));
        driveInfo.assignedDirectoryEntrySizesInClusters[i] = countFileClusters(driveInfo.fat, directoryEntries+i);
    }
    free(directoryEntries);

    driveInfo.numFatCopies = bootSector.fatCopies;
    driveInfo.numSectorsPerFat = bootSector.sectorsPerFat;

    return driveInfo;
}
