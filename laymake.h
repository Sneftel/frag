#ifndef LAYMAKE_H
#define LAYMAKE_H

#include "drvinfo.h"

#define LM_FREE_ENTRY 0xFFFFu

struct LayoutMaker;

/** Create a ready-to-fill layout maker, given (original) information about the drive which it will apply to */
struct LayoutMaker* createLayoutMaker(struct DriveInfo* origDriveInfo);

/** Return the number of clusters which need to be added to the chain for the file with the given entry index.
 * If the entry index is LM_FREE_ENTRY, returns the number of clusters which still remain to be marked as free.
 */
unsigned int getRemainingClustersForEntry(struct LayoutMaker* layoutMaker, unsigned int entryIndex);

/** Allocate the next unallocated cluster on to the end of the cluster chain for the file with the given entry index.
 * If the entry index is LM_FREE_ENTRY, assigns the next unallocated cluster as free.
 */
void assignNextClusterToEntry(struct LayoutMaker* layoutMaker, unsigned int entryIndex);

/** Write the given layout to the drive. 
 * All file entries (including LM_FREE_ENTRY) must have had the correct number of clusters allocated to them. */
void applyLayout(struct LayoutMaker* layoutMaker);

#endif
