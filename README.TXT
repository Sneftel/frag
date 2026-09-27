-=-=-= FRAG: A utility for creating fragmentation =-=-=-

FRAG is a simple tool for destructively filling a FAT16 volume with (bogus)
fragmented data.

Rather than actually write the data, FRAG fills only the file allocation tables
(FATs); the boot sector and the actual clusters are left untouched. All
existing files are removed and their content becomes effectively inaccessible
(though not securely unrecoverable). Needless to say, you should not run FRAG
on a volume containing data you care about.

For safety, the label of the chosen volume must be specified on the command
line, unless it is "FRAGME".

The volume is "filled" pseudorandomly, in patterns loosely approximating real-
world fragmentation. The volume starts out empty. In a loop a random file is
chosen and a maximum run length is randomly chosen between the specified 
min/max. The volume is linearly scanned for a free cluster, starting at a
random cluster and looping if necessary. The run of free clusters starting at
that cluster, up to the specified max, is appended to the current set of
clusters for the chosen file.

The number of files and the target cluster utilization (between 0 and 100
percent) can also be specified. If 100 percent utilization is selected a
single cluster will still be left unallocated so the volume can be 
defragmented.

Usage:

    FRAG.EXE D: [options] (where D is the drive letter)

Options:

    /a nnn  Set the minimum length in clusters of each run (default 16)
    /b nnn  Set the maximum length in clusters of each run (default 512)
    /c nnn  Set the number of files to create (default 512)
    /u pct  Set the percentage utilization target (default 75)
    /l str  Set the expected volume label (default FRAGME)

FAT12 and FAT32 volumes are not (yet) supported. Certain unusual features of
FAT16, such as unusual numbers of reserved sectors, are not supported either.
FRAG is designed to work on volumes which were formatted with MS-DOS'
FORMAT.EXE, though other FAT16 creation utilities are expected to work as long
as things are left at their defaults.
