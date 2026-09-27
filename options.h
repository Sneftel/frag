
extern char option_driveLetter;
extern int option_fileCount;
extern int option_minRunLength;
extern int option_maxRunLength;
extern int option_utilizationPercentage;
extern char* option_driveLabel;

void printUsage(char* arg0);
int processOptions(int argc, char** argv);
