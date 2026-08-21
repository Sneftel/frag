
#include <stdlib.h>
#include <assert.h>

#include "laymake.h"
#include "structs.h"
#include "dosutil.h"
#include "util.h"

struct FileInfo
{
    unsigned int remainingClusters;
    unsigned int firstCluster;
    unsigned int lastCluster;
};

struct LayoutMaker
{
    struct DriveInfo* origDriveInfo;
    struct FileInfo* fileInfos;
    unsigned int freeRemainingClusters;
    unsigned int nextCluster;
};

struct LayoutMaker* createLayoutMaker(struct DriveInfo* origDriveInfo)
{
    struct LayoutMaker* layoutMaker;
    int i;
    layoutMaker = malloc(sizeof(struct LayoutMaker));

    layoutMaker->origDriveInfo = origDriveInfo;
    layoutMaker->fileInfos = malloc(sizeof(struct FileInfo) * origDriveInfo->numAssignedDirectoryEntries);
    assert(layoutMaker->fileInfos);
    for(i=0; i<origDriveInfo->numAssignedDirectoryEntries; i++)
    {
        layoutMaker->fileInfos[i].remainingClusters = origDriveInfo->assignedDirectoryEntrySizesInClusters[i];
        layoutMaker->fileInfos[i].firstCluster = 0xFFFF;
        layoutMaker->fileInfos[i].lastCluster = 0xFFFF;
    }
    layoutMaker->freeRemainingClusters = origDriveInfo->numFreeClusters;
    layoutMaker->nextCluster = 2;

    return layoutMaker;
}

unsigned int getRemainingClustersForEntry(struct LayoutMaker* layoutMaker, unsigned int entryIndex)
{
    return entryIndex == LM_FREE_ENTRY ? layoutMaker->freeRemainingClusters : layoutMaker->fileInfos[entryIndex].remainingClusters;
}

#define FAT_ENTRIES_PER_SECTOR 256
#define FAT_ENTRIES_PER_SECTOR_BITS 8


static void writeFatValue(unsigned char driveNumber, unsigned int cluster, unsigned int value)
{
    unsigned int sectorNum;
    unsigned int offset;

    sectorNum = 1 + (cluster >> FAT_ENTRIES_PER_SECTOR_BITS); /* num FAT entries per sector */
    offset = (cluster & (FAT_ENTRIES_PER_SECTOR - 1)) * sizeof(unsigned int);

    abswritesmall(driveNumber, sizeof(unsigned int), sectorNum, offset, &value);
}

void assignNextClusterToEntry(struct LayoutMaker* layoutMaker, unsigned int entryIndex)
{
    unsigned int cluster;

    assert(layoutMaker->nextCluster < layoutMaker->origDriveInfo->numClusters);

    cluster = layoutMaker->nextCluster++;

    if(entryIndex == LM_FREE_ENTRY)
    {
        writeFatValue(layoutMaker->origDriveInfo->driveNumber, cluster, 0);
        --layoutMaker->freeRemainingClusters;
    }
    else
    {
        struct FileInfo* fileInfo = layoutMaker->fileInfos+entryIndex;
        assert(fileInfo->remainingClusters > 0);
        if(fileInfo->firstCluster == 0xFFFF)
        {
            assert(fileInfo->lastCluster == 0xFFFF);
            fileInfo->firstCluster = cluster;
            fileInfo->lastCluster = cluster;
        }
        else
        {
            writeFatValue(layoutMaker->origDriveInfo->driveNumber, fileInfo->lastCluster, cluster);
            fileInfo->lastCluster = cluster;
        }
        writeFatValue(layoutMaker->origDriveInfo->driveNumber, cluster, 0xFFFF);
        --fileInfo->remainingClusters;
    }
}

void applyLayout(struct LayoutMaker* layoutMaker)
{
    struct DirectoryEntry* directoryEntries;
    unsigned long rootDirectorySectorBase;
    int i, j;

    /* update root directory entries */
    rootDirectorySectorBase = 1 + layoutMaker->origDriveInfo->numFatCopies * layoutMaker->origDriveInfo->numSectorsPerFat;
    directoryEntries = malloc(sizeof(struct DirectoryEntry) * layoutMaker->origDriveInfo->numDirectoryEntries);
    absread(layoutMaker->origDriveInfo->driveNumber, calcRootDirectorySectors(layoutMaker->origDriveInfo->numDirectoryEntries), rootDirectorySectorBase, directoryEntries);
    for(i=0; i<layoutMaker->origDriveInfo->numAssignedDirectoryEntries; i++)
    {
        directoryEntries[i].firstCluster = layoutMaker->fileInfos[i].firstCluster;
    }
    abswrite(layoutMaker->origDriveInfo->driveNumber, calcRootDirectorySectors(layoutMaker->origDriveInfo->numDirectoryEntries), rootDirectorySectorBase, directoryEntries);

    /* copy first FAT into other FAT copies, a sector at a time */
    for(i=0; i<layoutMaker->origDriveInfo->numSectorsPerFat; i++)
    {
        unsigned char sector[SECTOR_SIZE];
        absread(layoutMaker->origDriveInfo->driveNumber, 1, i+1, sector);
        for(j=1; j<layoutMaker->origDriveInfo->numFatCopies; j++)
        {
            abswrite(layoutMaker->origDriveInfo->driveNumber, 1, 1+layoutMaker->origDriveInfo->numSectorsPerFat*i + j, sector);
        }
    }
}
