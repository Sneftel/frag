
extern char option_driveLetter;
extern unsigned option_fileCount;
extern unsigned option_minRunLength;
extern unsigned option_maxRunLength;
extern unsigned option_utilizationPercentage;
extern char* option_driveLabel;
extern char option_suppressConfirmation;

void printUsage(char* arg0);
int processOptions(int argc, char** argv);
