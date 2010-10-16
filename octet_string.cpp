#include "octet_string.h"

data_t octet_string::serialize()
{
    data_t serialized;

    // encode size
    int size = value.size();
    serialized.push_back(size << 24 & 0xff);
    serialized.push_back(size << 16 & 0xff);
    serialized.push_back(size << 8 & 0xff);
    serialized.push_back(size << 0 & 0xff);

    // encode value
    serialized += value;
}
