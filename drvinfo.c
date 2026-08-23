#include "drvinfo.h"
#include "dosutil.h"
#include "structs.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>

struct DriveInfo getDriveInfo(unsigned char driveNumber)
{
    struct DriveInfo driveInfo;
    long fileInfoSectorBase;
    struct DirectoryEntry* directoryEntries;
    struct BootSector bootSector;
    int i;

    driveInfo.driveNumber = driveNumber;

    driveInfo.numClusters = getNumClusters(driveNumber);

    absread(driveNumber, 1, 0, &bootSector);

    driveInfo.numDirectoryEntries = bootSector.rootEntries;
    fileInfoSectorBase = 1 + bootSector.fatCopies * bootSector.sectorsPerFat;
    directoryEntries = malloc(sizeof(struct DirectoryEntry) * bootSector.rootEntries);
    assert(directoryEntries);
    absread(driveNumber, calcRootDirectorySectors(bootSector.rootEntries), fileInfoSectorBase, directoryEntries);

    free(directoryEntries);

    driveInfo.numFatCopies = bootSector.fatCopies;
    driveInfo.numSectorsPerFat = bootSector.sectorsPerFat;
    driveInfo.numSectorsPerCluster = bootSector.sectorsPerCluster;

    return driveInfo;
}
