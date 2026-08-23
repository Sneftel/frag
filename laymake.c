
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "laymake.h"
#include "structs.h"
#include "hdcache.h"
#include "dosutil.h"
#include "util.h"

struct FileInfo
{
    unsigned int firstCluster;
    unsigned int lastCluster;
    unsigned int numClusters;
};

struct LayoutMaker
{
    struct DriveInfo* origDriveInfo;
    struct FileInfo* fileInfos;
};

struct LayoutMaker* createLayoutMaker(struct DriveInfo* origDriveInfo)
{
    struct LayoutMaker* layoutMaker;
    int i;
    layoutMaker = malloc(sizeof(struct LayoutMaker));

    layoutMaker->origDriveInfo = origDriveInfo;
    layoutMaker->fileInfos = malloc(sizeof(struct FileInfo) * origDriveInfo->numAssignedDirectoryEntries);
    assert(layoutMaker->fileInfos);
    for(i=0; i<origDriveInfo->numDirectoryEntries; i++)
    {
        layoutMaker->fileInfos[i].firstCluster = 0xFFFF;
        layoutMaker->fileInfos[i].lastCluster = 0xFFFF;
        layoutMaker->fileInfos[i].numClusters = 0;
    }
    memset(layoutMaker->fileInfos, 0xFF, sizeof(struct FileInfo) * origDriveInfo->numAssignedDirectoryEntries);

    return layoutMaker;
}

#define FAT_ENTRIES_PER_SECTOR 256
#define FAT_ENTRIES_PER_SECTOR_BITS 8

static void writeFatValue(unsigned char driveNumber, unsigned int cluster, unsigned int value)
{
    unsigned int sectorNum;
    unsigned int offset;

    sectorNum = 1 + (cluster >> FAT_ENTRIES_PER_SECTOR_BITS); /* num FAT entries per sector */
    offset = (cluster & (FAT_ENTRIES_PER_SECTOR - 1)) * sizeof(unsigned int);

    cacheWrite(driveNumber, sizeof(unsigned int), sectorNum, offset, &value);
}

void assignClusterToEntry(struct LayoutMaker* layoutMaker, unsigned int clusterIndex, unsigned int entryIndex)
{
    struct FileInfo* fileInfo = layoutMaker->fileInfos+entryIndex;
    assert(clusterIndex < layoutMaker->origDriveInfo->numClusters);

    if(fileInfo->firstCluster == 0xFFFF)
    {
        assert(fileInfo->lastCluster == 0xFFFF);
        fileInfo->firstCluster = clusterIndex;
        fileInfo->lastCluster = clusterIndex;
    }
    else
    {
        writeFatValue(layoutMaker->origDriveInfo->driveNumber, fileInfo->lastCluster, clusterIndex);
        fileInfo->lastCluster = clusterIndex;
    }
    writeFatValue(layoutMaker->origDriveInfo->driveNumber, clusterIndex, 0xFFFF);
}

void applyLayout(struct LayoutMaker* layoutMaker)
{
    struct DirectoryEntry* directoryEntries;
    unsigned long rootDirectorySectorBase;
    int i, j;
    unsigned long clusterSize;
    char filename[9];

    cacheFlush();

    clusterSize = layoutMaker->origDriveInfo->numSectorsPerCluster * SECTOR_SIZE;

    /* update root directory entries */
    rootDirectorySectorBase = 1 + layoutMaker->origDriveInfo->numFatCopies * layoutMaker->origDriveInfo->numSectorsPerFat;
    directoryEntries = malloc(sizeof(struct DirectoryEntry) * layoutMaker->origDriveInfo->numDirectoryEntries);
    absread(layoutMaker->origDriveInfo->driveNumber, calcRootDirectorySectors(layoutMaker->origDriveInfo->numDirectoryEntries), rootDirectorySectorBase, directoryEntries);
    for(i=0; i<layoutMaker->origDriveInfo->numAssignedDirectoryEntries; i++)
    {
        memset(&directoryEntries[i], 0, sizeof(struct DirectoryEntry));
        /* Fills filename and extension*/
        sprintf(directoryEntries[i].filename, "%08d000", i);
        directoryEntries[i].firstCluster = layoutMaker->fileInfos[i].firstCluster;
        directoryEntries[i].fileSize = layoutMaker->fileInfos[i].numClusters * clusterSize;
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
