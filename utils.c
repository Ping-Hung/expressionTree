#include "utils.h"

int count_acutal_length(char *str)
{
    if (str == NULL)
    {
        fprintf(stderr, "input str to %s should be a valid string\n", __func__);
        return -1;
    }

    int actualLength = 0;
    for (char ch = *str; ch != '\0'; ch = *(++str))
    {
        actualLength += 1;
    }

    return actualLength;
}