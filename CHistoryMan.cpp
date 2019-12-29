#include "framework.h"
#include "CHistoryMan.h"

int SKMKeyComparator(const void* key1, const void* key2)
{
    int result = 0;

    const SKMKey * k1 = (const SKMKey*)key1;
    const SKMKey* k2 = (const SKMKey*)key2;

    if (k1->uDate < k2->uDate)
        result = -1;
    else if (k1->uDate > k2->uDate)
        result = 1;

    return result;

}


