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


#ifndef SMFMAP_H
#define SMFMAP_H
#include "TileStorage.h"
#include "Image.h"
#include <string>

typedef struct {
	char magic[16];      ///< "spring map file\0"
	int version;         ///< Must be 1 for now
	int mapid;           ///< Sort of a GUID of the file, just set to a random value when writing a map

	int mapx;            ///< Must be divisible by 128
	int mapy;            ///< Must be divisible by 128
	int squareSize;      ///< Distance between vertices. Must be 8
	int texelPerSquare;  ///< Number of texels per square, must be 8 for now
	int tilesize;        ///< Number of texels in a tile, must be 32 for now
	float minHeight;     ///< Height value that 0 in the heightmap corresponds to	
	float maxHeight;     ///< Height value that 0xffff in the heightmap corresponds to

	int heightmapPtr;    ///< File offset to elevation data (short int[(mapy+1)*(mapx+1)])
	int typeMapPtr;      ///< File offset to typedata (unsigned char[mapy/2 * mapx/2])
	int tilesPtr;        ///< File offset to tile data (see MapTileHeader)
	int minimapPtr;      ///< File offset to minimap (always 1024*1024 dxt1 compresed data plus 8 mipmap sublevels)
	int metalmapPtr;     ///< File offset to metalmap (unsigned char[mapx/2 * mapy/2])
	int featurePtr;      ///< File offset to feature data (see MapFeatureHeader)

	int numExtraHeaders; ///< Numbers of extra headers following main header
} SMFHeader ;
typedef struct 
{
	int numFeatureType;
	int numFeatures;
}MapFeatureHeader ;
typedef struct
{
	int numTileFiles; ///< Number of tile files to read in (usually 1)
	int numTiles;     ///< Total number of tiles
} MapTileHeader;
typedef struct{
	int size; ///< Size of extra header
	int type; ///< Type of extra header
} ExtraHeader;
class InvalidMapSizeException
{
  
};
class CannotLoadTextureException
{
  
};
class SMFMap
{

public:
    SMFMap(std::string name,std::string texturepath);
    virtual ~SMFMap();
    void SetMetalMap(std::string path);
    void SetTypeMap(std::string path);
    //void SetFeatureMap(std::string path);
    void SetHeightMap(std::string path);
    void SetVegetationMap(std::string path);
    void SetMiniMap(std::string path);
    void Compile();
    void SetHeightRange(float minh,float maxh);
    void SetCompressionTol(float th);
    void SetCompressionType(int c);
    void SetBlur(bool b);
private:
    void DoCompress(int * indices , std::vector<uint64_t>& order);
    TileStorage * m_tiles;
    Image * metalmap;
    //Image * featuremap;
    Image * heightmap;
    Image * typemap;
    Image * texture;
    Image * minimap;
    Image * vegetationmap;
    int mapx,mapy;
    std::string m_name;
    float m_minh;
    float m_maxh;
    float m_th;
    bool m_smooth;
    int m_comptype;
     std::string texpath;
};

#endif // SMFMAP_H
