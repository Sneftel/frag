#ifndef LAYMAKE_H
#define LAYMAKE_H

#include "drvinfo.h"

struct LayoutMaker;

/** Create a ready-to-fill layout maker, given (original) information about the drive which it will apply to */
struct LayoutMaker* createLayoutMaker(struct DriveInfo* origDriveInfo);

/** Allocate the next unallocated cluster on to the end of the cluster chain for the file with the given entry index. */
void assignClusterToEntry(struct LayoutMaker* layoutMaker, unsigned int clusterIndex, unsigned int entryIndex);

/** Write the given layout to the drive. 
 * All file entries must have had the correct number of clusters allocated to them. */
void applyLayout(struct LayoutMaker* layoutMaker);

#endif
