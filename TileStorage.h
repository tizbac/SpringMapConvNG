/*
    Copyright (C) 2011  Tiziano <tizbac2@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TILESTORAGE_H
#define TILESTORAGE_H
#include <stdint.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <list>
#include "CRC.h"
inline uint64_t tilechecksum( uint8_t * data )
{
  uint64_t r = 0;
  for ( int x = 0; x < 32*32*4; x++ )
  {
    r += data[x]*63018038201L*x*x;
    r ^= 13091204281L;
    r *= 13091204281L*x;
    r *= 226673591177742970257407LL*data[x];
  }
  Crc32 c;
  c.AddData(data,32*32*4);
  r *= c.GetCrc32();
  return r;
  
}
class InvalidTileIndexException
{
  
  
};
class InvalidTileDataPointerException
{
  
  
};
enum CompressLevels
{
  COMPRESS_SHITTY = 1,
  COMPRESS_REASONABLE = 2,
  COMPRESS_INSANE = 3,
  COMPRESS_REASONABLE_BESTQUALITY = 4
  
  
};
class TileStorage
{

public:
    TileStorage();
    virtual ~TileStorage();
    uint64_t AddTile( uint8_t * data ); // 32x32 RGBA
    void WriteToFile( FILE * f , std::vector<uint64_t>& tile_order);
    uint64_t AddTileOrGetSimiliar( uint8_t* data, float th, int compresslevel );
    uint32_t GetTileCount();
    void SetDictSize(uint32_t s);
    void Reset();
private:
    void CompressAll();
    void CompressTile(uint64_t uid);
    std::map<uint64_t,uint8_t*> m_tiles;
    std::map<uint64_t,uint8_t*> m_tiles_compressed;
    std::list<uint64_t> m_lasttiles;
    unsigned int m_dictcount;
};

#endif // TILESTORAGE_H
