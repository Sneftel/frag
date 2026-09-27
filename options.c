#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>

char option_driveLetter = 'd';
int option_fileCount = 512;
int option_minRunLength = 16;
int option_maxRunLength = 512;
int option_utilizationPercentage = 75;
char* option_driveLabel = "FRAGME";

void printUsage(char* arg0)
{
    printf(
        "FRAG: A tool for quickly and destructively creating fragmentation.\n"
        "CAUTION: THIS TOOL WILL DESTROY ALL DATA ON THE TARGET DRIVE.\n\n"
        "Usage:\n"
        "\t%s [L:] [options] (where L is the drive letter, defaulting to D)\n\n"
        "Options:\n"
        "\t/a nnn  Set the minimum length in clusters of each run (default 16)\n"
        "\t/b nnn  Set the maximum length in clusters of each run (default 512)\n"
        "\t/c nnn  Set the number of files to create (default 512)\n"
        "\t/u pct  Set the percentage utilization target (default 75)\n"
        "\t/l str  Set the expected drive label (default FRAGME)\n",
        arg0);
}

static int processIntOption(int argc, char** argv, int* pi, int start, int minVal, int maxVal, int* out)
{
    char* buf;
    char* bufEnd;
    long value;

    buf = argv[*pi] + start;

    if(buf == '\0')
    {
        if(*pi == argc)
        {
            printf("Option %s not followed by an integer\n", argv[*pi]);
            return 0;
        }

        buf = argv[*++pi];
    }

    value = strtol(buf, &bufEnd, 10);

    if(*bufEnd != '\0')
    {
        printf("Invalid integer in option token %s\n", argv[*pi]);
        return 0;
    }

    if(value < minVal || value > maxVal)
    {
        printf("Value %s out of range\n", buf);
        return 0;
    }

    *out = (int)value;
    return 1;
}

int processOptions(int argc, char** argv)
{
    int i;
    for(i=0; i<argc; i++)
    {
        if(argv[i][0] == '/' || argv[i][0] == '-')
        {
            switch(tolower(argv[i][1]))
            {
                case 'c':
                    if(!processIntOption(argc, argv, &i, 2, 1, 511, &option_fileCount))
                    {
                        return 0;
                    }

                case 'a':
                    if(!processIntOption(argc, argv, &i, 2, 1, 65535, &option_minRunLength))
                    {
                        return 0;
                    }

                case 'b':
                    if(!processIntOption(argc, argv, &i, 2, 1, 65535, &option_maxRunLength))
                    {
                        return 0;
                    }
                
                case 'u':
                    if(!processIntOption(argc, argv, &i, 2, 1, 100, &option_utilizationPercentage))
                    {
                        return 0;
                    }

                case 'l':
                    if(argv[i][2] == '\0')
                    {
                        if(i+1 == argc)
                        {
                            printf("Option %s not followed by a value\n", argv[i]);
                            return 0;
                        }
                        option_driveLabel = argv[++i];
                    }
                    else
                    {
                        option_driveLabel = argv[i]+2;
                    }
                    break;
                
                default:
                    printf("Unknown option %s\n", argv[i]);
                    return 0;
            }
        }
        else if(isalpha(argv[i][0]) && argv[i][1] == ':' && argv[i][2] == '\0')
        {
            option_driveLetter = tolower(argv[i][0]);
        }
        else
        {
            printf("Unknown option %s\n", argv[i]);
            return 0;
        }
    }

    return 1;
}