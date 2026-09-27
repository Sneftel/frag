
#include <dos.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "bitfield.h"
#include "drvinfo.h"
#include "laymake.h"
#include "dosutil.h"
#include "options.h"

static void sampleFreeSpace(struct Bitfield const* allocated, unsigned int numClusters, int maxLength, unsigned int* startClusterOut, unsigned int* lengthOut)
{
    unsigned int startCluster;
    unsigned int desiredEndCluster;
    unsigned int endCluster;

    startCluster = ((unsigned int)rand() << 1) % numClusters;
    startCluster = scanForZeroBit(allocated, startCluster, numClusters);
    if(startCluster == numClusters)
    {
        startCluster = scanForZeroBit(allocated, 0, numClusters);
        assert(startCluster != numClusters);
    }
    
    desiredEndCluster = startCluster+maxLength;
    if(desiredEndCluster < startCluster || desiredEndCluster > numClusters)
    {
        desiredEndCluster = numClusters;
    }
    endCluster = scanForOneBit(allocated, startCluster, desiredEndCluster);

    *startClusterOut = startCluster;
    *lengthOut = endCluster-startCluster;
}

static void randomize()
{
    struct dostime_t time;
    _dos_gettime(&time);
    srand(time.second | (time.minute << 8));
}

int main(int argc, char** argv)
{
    struct DriveInfo driveInfo;
    struct LayoutMaker* layoutMaker;
    struct Bitfield* clustersAllocated;
    unsigned int numFreeClusters;
    unsigned int targetNumFreeClusters;

    if(!processOptions(argc, argv))
    {
        printUsage(argv[0]);
        exit(1);
    }

    if(option_minRunLength >= option_maxRunLength)
    {
        printf("Invalid runlength interval.\n");
        exit(1);
    }

    printf("Initializing...\n");

    /* invalidate buffers in case the FAT is cached */
    diskReset();

    randomize();

    driveInfo = getDriveInfo(3); /* D drive */

    if(strcmp(driveInfo.volumeLabel, option_driveLabel) != 0)
    {
        printf("Volume label was '%s', expected '%s'\n", driveInfo.volumeLabel, option_driveLabel);
        exit(1);
    }

    if(option_fileCount > driveInfo.numDirectoryEntries-1)
    {
        printf("File count of %d requested, max is %d", option_fileCount, driveInfo.numDirectoryEntries-1);
    }

    numFreeClusters = driveInfo.numClusters - 2; /* Account for reserved clusters */
    // Targeting 75% utilization
    targetNumFreeClusters = numFreeClusters * (100ul - option_utilizationPercentage) / 100;

    layoutMaker = createLayoutMaker(&driveInfo);

    clustersAllocated = allocateBitfield(driveInfo.numClusters);
    /* reserve first 2 clusters */
    setBits(clustersAllocated, 0, 2);

    printf("Filling clusters...\n");

    while(numFreeClusters > targetNumFreeClusters)
    {
        unsigned int startCluster;
        unsigned int numClustersToFill;
        unsigned int entryIndex;
        unsigned int desiredNumClustersToFill;
        int i;

        if(option_maxRunLength == option_minRunLength)
        {
            desiredNumClustersToFill = option_minRunLength;
        }
        else
        {
            desiredNumClustersToFill = (rand() % (option_maxRunLength-option_minRunLength)) + option_minRunLength;
        }

        sampleFreeSpace(clustersAllocated, driveInfo.numClusters, desiredNumClustersToFill, &startCluster, &numClustersToFill);

        entryIndex = rand() % (option_fileCount-1) + 1;

        for(i=0; i<numClustersToFill; i++)
        {
            assignClusterToEntry(layoutMaker, startCluster+i, entryIndex);
        }

        setBits(clustersAllocated, startCluster, numClustersToFill);

        numFreeClusters -= numClustersToFill;
    }

    printf("Applying layout...\n");

    applyLayout(layoutMaker);

    printf("Done.\n");

    diskReset();

    return 0;
}
