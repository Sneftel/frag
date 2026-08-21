#ifndef STRUCTS_H
#define STRUCTS_H

#pragma pack(__push, 1)

struct BootSector
{
    /*bpb*/
    unsigned char bootCode[3];
    char osName[8];
    unsigned int bytesPerSector;
    unsigned char sectorsPerCluster;
    unsigned int reservedSectors;
    unsigned char fatCopies;
    unsigned int rootEntries;
    unsigned int smallNumSectors;
    unsigned char mediaDescriptor;
    unsigned int sectorsPerFat;
    unsigned int sectorsPerTrack;
    unsigned int heads;
    unsigned long hiddenSectors;
    unsigned long largeNumSectors;

    /*ebpb*/
    unsigned char biosDriveNumber;
    unsigned char reserved;
    unsigned char ebSignature;
    unsigned long serialNumber;
    char volumeLabel[11];
    char fsType[8];
    unsigned char bootstrapCode[448];
    unsigned int bsSignature;
};

struct DirectoryEntry
{
    char filename[8];
    char extension[3];
    unsigned char attributes;
    unsigned char reserved1;
    unsigned char creationMillisecond;
    unsigned int creationTime;
    unsigned int creationDate;
    unsigned int lastAccessDate;
    unsigned int reserved2;
    unsigned int lastWriteTime;
    unsigned int lastWriteDate;
    unsigned int firstCluster;
    unsigned long fileSize;
};

#pragma pack(__pop)

#endif
