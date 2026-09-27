#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "drvinfo.h"
#include "dosutil.h"
#include "structs.h"
#include "util.h"

struct DriveInfo getDriveInfo(unsigned char driveNumber)
{
    struct DriveInfo driveInfo;
    long fileInfoSectorBase;
    struct BootSector bootSector;
    int i;

    driveInfo.driveNumber = driveNumber;

    driveInfo.numClusters = getNumClusters(driveNumber);

    absread(driveNumber, 1, 0, &bootSector);

    if(bootSector.reservedSectors != 1)
    {
        printf("Unexpected number of reserved sectors.\n");
        exit(1);
    }
    if(bootSector.mediaDescriptor != 0xF8)
    {
        printf("Target drive not marked as a fixed disk\n");
        exit(1);
    }
    if(bootSector.ebSignature != 0x29)
    {
        printf("Failed to find EBPB signature\n");
        exit(1);
    }
    
    strncpy(driveInfo.volumeLabel, bootSector.volumeLabel, 11);
    driveInfo.volumeLabel[11] = '\0';

    if(strncmp(bootSector.fsType, "FAT16", 5) != 0)
    {
        printf("FS type not FAT16\n");
        exit(1);
    }

    driveInfo.numDirectoryEntries = bootSector.rootEntries;
    fileInfoSectorBase = 1 + bootSector.fatCopies * bootSector.sectorsPerFat;

    driveInfo.numFatCopies = bootSector.fatCopies;
    driveInfo.numSectorsPerFat = bootSector.sectorsPerFat;
    driveInfo.numSectorsPerCluster = bootSector.sectorsPerCluster;

    return driveInfo;
}
