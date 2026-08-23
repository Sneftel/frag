
#include <dos.h>
#include <assert.h>
#include <stdlib.h>

#include "bitfield.h"
#include "drvinfo.h"
#include "laymake.h"
#include "dosutil.h"

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

#define MAX_RUN_LENGTH 16

int main(int argc, char** argv)
{
    struct DriveInfo driveInfo;
    struct LayoutMaker* layoutMaker;
    struct Bitfield* clustersAllocated;
    unsigned int numFreeClusters;
    unsigned int targetNumFreeClusters;

    /* invalidate buffers in case the FAT is cached */
    diskReset();

    randomize();

    driveInfo = getDriveInfo(3); /* D drive */
    numFreeClusters = driveInfo.numClusters - 2; /* Account for reserved clusters */
    // Targeting 75% utilization
    targetNumFreeClusters = (numFreeClusters >> 2);
    layoutMaker = createLayoutMaker(&driveInfo);

    clustersAllocated = allocateBitfield(driveInfo.numClusters);
    /* reserve first 2 clusters */
    setBits(clustersAllocated, 0, 2);

    while(numFreeClusters > targetNumFreeClusters)
    {
        unsigned int startCluster;
        unsigned int numClustersToFill;
        unsigned int entryIndex;
        unsigned int desiredNumClustersToFill;
        int i;

        desiredNumClustersToFill = ((rand() >> 5) % MAX_RUN_LENGTH) + 1;

        sampleFreeSpace(clustersAllocated, driveInfo.numClusters, desiredNumClustersToFill, &startCluster, &numClustersToFill);

        entryIndex = rand() % driveInfo.numDirectoryEntries;

        for(i=0; i<numClustersToFill; i++)
        {
            assignClusterToEntry(layoutMaker, startCluster+i, entryIndex);
        }

        setBits(clustersAllocated, startCluster, numClustersToFill);

        numFreeClusters -= numClustersToFill;
    }

    applyLayout(layoutMaker);

    diskReset();

    return 0;
}
