#ifndef DRVINFO_H
#define DRVINFO_H

struct DriveInfo
{
    unsigned char driveNumber;
    
    /** The number of clusters in the fat. This includes the reserved clusters. */
    unsigned int numClusters;
    /** The FAT itself. Each entry is the index of the next cluster in the file, or one of the  */
    unsigned int* fat;

    /** The total number of entries in the root directory table, including those which have never been assigned. */
    unsigned int numDirectoryEntries;

    /** The total number of assigned entries in the root directory table, including deleted entries. */
    unsigned int numAssignedDirectoryEntries;

    /** The size in clusters of each file in the root directory table. Deleted entries are given size 0. */
    unsigned int* assignedDirectoryEntrySizesInClusters;

    /** The number of free clusters in the fat. */
    unsigned int numFreeClusters;

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
