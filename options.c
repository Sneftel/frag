#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>

char option_driveLetter = 0;
unsigned option_fileCount = 511;
unsigned option_minRunLength = 16;
unsigned option_maxRunLength = 512;
unsigned option_utilizationPercentage = 75;
char* option_driveLabel = "FRAGME";

void printUsage(char* arg0)
{
    printf(
        "FRAG: A tool for quickly and destructively creating fragmentation.\n"
        "CAUTION: THIS TOOL WILL DESTROY ALL DATA ON THE TARGET DRIVE.\n\n"
        "Usage:\n"
        "\t%s D: [options] (where D is the drive letter)\n\n"
        "Options:\n"
        "\t/a nnn  Set the minimum length in clusters of each run (default 16)\n"
        "\t/b nnn  Set the maximum length in clusters of each run (default 512)\n"
        "\t/c nnn  Set the number of files to create (default 512)\n"
        "\t/u pct  Set the percentage utilization target (default 75)\n"
        "\t/l str  Set the expected volume label (default FRAGME)\n",
        arg0);
}

static unsigned processUnsignedOption(int argc, char** argv, int* pi, int start, unsigned minVal, unsigned maxVal, unsigned* out)
{
    char* buf;
    char* bufEnd;
    unsigned long value;

    buf = argv[*pi] + start;

    if(*buf == '\0')
    {
        if(*pi == argc)
        {
            printf("Option %s not followed by an integer\n", argv[*pi]);
            return 0;
        }

        buf = argv[++*pi];
    }

    value = strtoul(buf, &bufEnd, 10);

    if(*bufEnd != '\0')
    {
        printf("Invalid integer in option token %s\n", argv[*pi]);
        return 0;
    }

    if(value < minVal)
    {
        printf("Value %lu below minimum of %u\n", value, minVal);
        return 0;
    }

    if(value > maxVal)
    {
        printf("Value %lu above maximum of %u\n", value, maxVal);
        return 0;
    }

    *out = (unsigned)value;
    return 1;
}

int processOptions(int argc, char** argv)
{
    int i;
    for(i=1; i<argc; i++)
    {
        if(argv[i][0] == '/' || argv[i][0] == '-')
        {
            switch(tolower(argv[i][1]))
            {
                case 'c':
                    if(!processUnsignedOption(argc, argv, &i, 2, 1, 511, &option_fileCount))
                    {
                        return 0;
                    }
                    break;

                case 'a':
                    if(!processUnsignedOption(argc, argv, &i, 2, 1, 65535, &option_minRunLength))
                    {
                        return 0;
                    }
                    break;

                case 'b':
                    if(!processUnsignedOption(argc, argv, &i, 2, 1, 65535, &option_maxRunLength))
                    {
                        return 0;
                    }
                    break;
                
                case 'u':
                    if(!processUnsignedOption(argc, argv, &i, 2, 0, 100, &option_utilizationPercentage))
                    {
                        return 0;
                    }
                    break;

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

    if(option_driveLetter == 0)
    {
        printf("Drive letter not specified\n");
        return 0;
    }

    return 1;
}
