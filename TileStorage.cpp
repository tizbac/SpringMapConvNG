/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Tiziano <email>

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


#include "TileStorage.h"
#include <iostream>
#include <cstring>
#include <math.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <sstream>
#include <stdlib.h>
inline float tilediff(uint8_t * t1,uint8_t * t2)
{
  float diff = 0.0;
  for ( int i = 0; i < 32*32*4; i++ )
  {
    float d1 = fabs(float(t1[i])-float(t2[i]));
    if ( d1 < 30 )
      diff += d1;
    else
      diff += 255.0f;//If it has a point that is VERY different , it must not be reused 
    
    
  }
  diff /= 32.0*32.0*4.0f*10.0f;
  return diff;
}

TileStorage::TileStorage()
{
  Reset();
  m_dictcount = 64;
}

TileStorage::~TileStorage()
{
  Reset();
}
void TileStorage::Reset()
{
  for ( std::map<uint64_t,uint8_t*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++ )
  {
    if ( m_tiles_compressed.find((*it).first) != m_tiles_compressed.end() )
    {
      delete [] m_tiles_compressed[(*it).first];
      
    }
    delete [] (*it).second;
  }
  m_tiles.clear();
  m_lasttiles.clear();
  m_tiles_compressed.clear();
}

uint64_t TileStorage::AddTile(uint8_t* data)
{
  uint64_t checksum = tilechecksum(data);
  if ( m_tiles.find(checksum) != m_tiles.end() )
  {
    std::cerr << "Duplicate tile detected, dropping!" << std::endl;
    return checksum;
  }
  uint8_t* data_copy = new uint8_t[32*32*4];
  memcpy(data_copy,data,32*32*4);
  m_tiles.insert(std::pair<uint64_t,uint8_t*>(checksum,data_copy));
  m_lasttiles.push_back(checksum);
  if ( m_lasttiles.size() > m_dictcount )
    m_lasttiles.pop_front();
  return checksum;
}
void TileStorage::CompressAll()
{
  for ( std::map<uint64_t,uint8_t*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++ )
  {
    if ( m_tiles_compressed.find((*it).first) == m_tiles_compressed.end() )
    {
      CompressTile((*it).first);
      
      
    }
    
    
  }

}
void TileStorage::SetDictSize(uint32_t s)
{
  m_dictcount = s;
}

void TileStorage::CompressTile(uint64_t uid)
{
  uint8_t * m0;
  uint8_t * m1;
  uint8_t * m2;
  uint8_t * m3;
  uint8_t * dataptr = m_tiles[uid];
  uint8_t * compressedmipmaps = new uint8_t[680];
  if ( !dataptr )
  {
    delete [] compressedmipmaps;
    throw InvalidTileDataPointerException();
    
  }
  uint32_t s;
  uint32_t s2 = 0;
  ILuint mip1 = ilGenImage();
  ilBindImage(mip1);
  ilTexImage(32,32,1,4,IL_RGBA,IL_UNSIGNED_BYTE,dataptr);
  /*std::stringstream ss;
  ss << "Tile" << uid << ".png";
  ilSaveImage(ss.str().c_str());*/
  m0 = ilCompressDXT(ilGetData(),32,32,1,IL_DXT1,&s);
  memcpy(&compressedmipmaps[s2],m0,s);
  s2 += s;
  iluScale(16,16,1);
  m1 = ilCompressDXT(ilGetData(),16,16,1,IL_DXT1,&s);
  memcpy(&compressedmipmaps[s2],m1,s);
  s2 += s;
  iluScale(8,8,1);
  m2 = ilCompressDXT(ilGetData(),8,8,1,IL_DXT1,&s);
  memcpy(&compressedmipmaps[s2],m2,s);
  s2 += s;
  iluScale(4,4,1);
  m3 = ilCompressDXT(ilGetData(),4,4,1,IL_DXT1,&s);
  memcpy(&compressedmipmaps[s2],m3,s);
  s2 += s;
  ilDeleteImage(mip1);
  
  /*squish::CompressImage(dataptr,32,32,m0,squish::kDxt1);
  squish::CompressImage(dataptr,16,16,m1,squish::kDxt1);
  squish::CompressImage(dataptr,8,8,m2,squish::kDxt1);
  squish::CompressImage(dataptr,4,4,m3,squish::kDxt1);*/
  
  
  
  
  free(m0);
  free(m1);
  free(m2);
  free(m3);
  
  m_tiles_compressed[uid] = compressedmipmaps;
  //std::cout << "Tile " << uid << " compressed!" << std::endl;
  
}

void TileStorage::WriteToFile(FILE* f, std::vector< uint64_t >& tile_order)
{
  char magic[16];
  strcpy(magic,"spring tilefile");
  int version = 1;
  int numtiles = m_tiles_compressed.size();
  int tileSize = 32;
  int compressionType = 1;
  fwrite(magic,16,1,f);
  fwrite(&version,4,1,f);
  fwrite(&numtiles,4,1,f);
  fwrite(&tileSize,4,1,f);
  fwrite(&compressionType,4,1,f);
  for ( std::vector< uint64_t >::const_iterator it = tile_order.begin(); it != tile_order.end(); it++ )
  {
    if ( m_tiles_compressed.find(*it) == m_tiles_compressed.end() )
    {
      CompressAll();
      
      
    }
    if ( m_tiles_compressed.find(*it) == m_tiles_compressed.end() )
    {
      throw InvalidTileIndexException();
    }
    fwrite(m_tiles_compressed[*it],680,1,f);
  }
  fflush(f);
}
uint64_t TileStorage::AddTileOrGetSimiliar(uint8_t* data, float th, int compresslevel)
{
  uint64_t checksum = tilechecksum(data);
  if ( m_tiles.find(checksum) != m_tiles.end() )
  {
    //std::cout << "Debug(AddTileOrGetSimiliar): " << checksum << " already exists" << std::endl;
    return checksum;
    
  }
  if ( compresslevel == COMPRESS_INSANE )
  {
    for ( std::map<uint64_t,uint8_t*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++ )
    {
      if ( tilediff(data,(*it).second) < th )
      {
	return (*it).first;
	
      }
      
      
    }
  }else if ( compresslevel == COMPRESS_REASONABLE )
  {
    for ( std::list<uint64_t>::iterator it = m_lasttiles.begin(); it != m_lasttiles.end(); it++ )
    {
      if ( tilediff(data,m_tiles[(*it)]) < th )
      {
	return (*it);
	
      }
    }
    
    
  }else if ( compresslevel == COMPRESS_SHITTY )
  {
    //do nothing...
    
  }else if ( compresslevel == COMPRESS_REASONABLE_BESTQUALITY )
  {
    float mindiff = 9999999.0f;
    uint64_t besttile = 0;
    for ( std::list<uint64_t>::iterator it = m_lasttiles.begin(); it != m_lasttiles.end(); it++ )
    {
      float diff = tilediff(data,m_tiles[(*it)]);
      if ( diff < mindiff )
      {
	besttile = (*it);
	mindiff = diff;
	
      }
    }
    if ( mindiff <= th )
      return besttile;
    
    
  }
  return AddTile(data);
}
uint32_t TileStorage::GetTileCount()
{
  return std::max(m_tiles_compressed.size(),m_tiles.size());
}
