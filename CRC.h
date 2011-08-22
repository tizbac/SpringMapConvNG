#ifndef CRC_H
#define CRC_H

#include <stdint.h>

class Crc32
{
public:
    Crc32() ;
    ~Crc32();
    void Reset();
    void AddData(const uint8_t* pData, const uint32_t length);
    
    const uint32_t GetCrc32();

private:
    uint32_t _crc;
};

#endif // CRC_H
