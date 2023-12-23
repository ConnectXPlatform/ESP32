#include <string.h>
#include "StringUtils.h"

size_t splitString(const char* input, const char delimiter, const char** output, size_t* outputLengths, size_t maxParts)
{
    size_t partCount = 0;
    size_t startIndex = 0;
    const char *endIndex = strchr(input, delimiter);

    while (endIndex != nullptr && partCount < maxParts - 1)
    {
        int partLength = endIndex - (input + startIndex);
        output[partCount] = input + startIndex;
        outputLengths[partCount] = partLength;
        partCount++;

        startIndex = endIndex - input + 1;
        endIndex = strchr(input + startIndex, delimiter);
    }

    // Add the last part of the string
    int lastPartLength = strlen(input + startIndex);
    output[partCount] = input + startIndex;
    outputLengths[partCount] = lastPartLength;
    partCount++;

    return partCount;
}
