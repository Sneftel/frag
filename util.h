#ifndef UTIL_H
#define UTIL_H

#include "structs.h"

#define SECTOR_SIZE 512

static unsigned long calcRootDirectorySectors(int numEntries)
{
    unsigned long rootDirectorySize = numEntries * sizeof(struct DirectoryEntry);
    return rootDirectorySize / SECTOR_SIZE;
}

#endif
