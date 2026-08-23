
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "laymake.h"
#include "structs.h"
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
    unsigned int huge* fat;
};

struct LayoutMaker* createLayoutMaker(struct DriveInfo* origDriveInfo)
{
    struct LayoutMaker* layoutMaker;
    unsigned int clearedFatSector[256];
    int i;
    layoutMaker = malloc(sizeof(struct LayoutMaker));

    layoutMaker->origDriveInfo = origDriveInfo;
    layoutMaker->fileInfos = malloc(sizeof(struct FileInfo) * origDriveInfo->numDirectoryEntries);
    assert(layoutMaker->fileInfos);

    memset(layoutMaker->fileInfos, 0, sizeof(struct FileInfo) * origDriveInfo->numDirectoryEntries);

    layoutMaker->fat = halloc(origDriveInfo->numClusters, sizeof(unsigned int));
    layoutMaker->fat[0] = 0xFFF8u;
    layoutMaker->fat[1] = 0xFFFFu;
    for(i=2; i<origDriveInfo->numClusters; i++)
    {
        layoutMaker->fat[i] = 0;
    }

    for(i=0; i<origDriveInfo->numSectorsPerFat; i++)
    {
        abswrite(origDriveInfo->driveNumber, 1, 1+i, clearedFatSector);
        // don't reserve first two entries in subsequent sectors
        clearedFatSector[0] = 0;
        clearedFatSector[1] = 0;
    }

    return layoutMaker;
}

#define FAT_ENTRIES_PER_SECTOR 256
#define FAT_ENTRIES_PER_SECTOR_BITS 8

void assignClusterToEntry(struct LayoutMaker* layoutMaker, unsigned int clusterIndex, unsigned int entryIndex)
{
    struct FileInfo* fileInfo = layoutMaker->fileInfos+entryIndex;
    assert(clusterIndex < layoutMaker->origDriveInfo->numClusters);

    if(fileInfo->firstCluster == 0)
    {
        assert(fileInfo->lastCluster == 0);
        fileInfo->firstCluster = clusterIndex;
        fileInfo->lastCluster = clusterIndex;
    }
    else
    {
        layoutMaker->fat[fileInfo->lastCluster] = clusterIndex;
        fileInfo->lastCluster = clusterIndex;
    }
    layoutMaker->fat[clusterIndex] = 0xFFFFu;
    fileInfo->numClusters++;
}

void applyLayout(struct LayoutMaker* layoutMaker)
{
    struct DirectoryEntry* directoryEntries;
    unsigned long rootDirectorySectorBase;
    unsigned int i, j;
    unsigned long clusterSize;

    clusterSize = (unsigned long)layoutMaker->origDriveInfo->numSectorsPerCluster * SECTOR_SIZE;

    /* update root directory entries */
    rootDirectorySectorBase = 1 + layoutMaker->origDriveInfo->numFatCopies * layoutMaker->origDriveInfo->numSectorsPerFat;
    directoryEntries = malloc(sizeof(struct DirectoryEntry) * layoutMaker->origDriveInfo->numDirectoryEntries);

    for(i=0; i<layoutMaker->origDriveInfo->numDirectoryEntries; i++)
    {
        memset(&directoryEntries[i], 0, sizeof(struct DirectoryEntry));
        /* Fills filename and extension. Also overruns into the attribute field, but benignly. */
        sprintf(directoryEntries[i].filename, "%08d000", i);
        directoryEntries[i].firstCluster = layoutMaker->fileInfos[i].firstCluster;
        directoryEntries[i].fileSize = layoutMaker->fileInfos[i].numClusters * clusterSize;
    }

    abswrite(layoutMaker->origDriveInfo->driveNumber, calcRootDirectorySectors(layoutMaker->origDriveInfo->numDirectoryEntries), rootDirectorySectorBase, directoryEntries);
    free(directoryEntries);
    directoryEntries = 0;

    /* write fat, a sector at a time */
    for(i=0; i<layoutMaker->origDriveInfo->numSectorsPerFat; i++)
    {
        unsigned int sector[FAT_ENTRIES_PER_SECTOR];
        for(j=0; j<FAT_ENTRIES_PER_SECTOR; j++)
        {
            sector[j] = layoutMaker->fat[(i << FAT_ENTRIES_PER_SECTOR_BITS) | j];
        }
        for(j=1; j<layoutMaker->origDriveInfo->numFatCopies; j++)
        {
            abswrite(layoutMaker->origDriveInfo->driveNumber, 1, 1+layoutMaker->origDriveInfo->numSectorsPerFat*j + i, sector);
        }
    }
}
