#ifndef DRVINFO_H
#define DRVINFO_H

struct DriveInfo
{
    unsigned char driveNumber;
    
    /** The number of clusters in the fat. This includes the reserved clusters. */
    unsigned int numClusters;

    /** The total number of entries in the root directory table, including those which have never been assigned. */
    unsigned int numDirectoryEntries;

    /** Number of copies of the fat stored on disk */
    unsigned int numFatCopies;

    /** Number of sectors each copy of the fat takes up */
    unsigned int numSectorsPerFat;

    /** Size of each cluster, in sectors */
    unsigned int numSectorsPerCluster;
};

/** Read info for a drive. The drive must be FAT16 and have no subdirectories and no bad clusters. */
struct DriveInfo getDriveInfo(unsigned char driveNumber);

#endif
