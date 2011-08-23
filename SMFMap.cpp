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


#include "SMFMap.h"
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
SMFMap::SMFMap(std::string name,std::string texturepath)
{
    m_tiles = new TileStorage();
    metalmap = NULL;
    heightmap = NULL;
    typemap = NULL;
    minimap = NULL;
    vegetationmap = NULL;
    texture = new Image(texturepath.c_str());
    
    if ( texture->w < 1 )
        throw CannotLoadTextureException();
    if ( texture->w % 1024 != 0 || texture->h % 1024 != 0)
    {
        throw InvalidMapSizeException();

    }
    mapx = ( texture->w / 1024 ) * 128;
    mapy = ( texture->w / 1024 ) * 128;
    m_minh = 0.0;
    m_maxh = 1.0;
    m_name = name;
    m_th = 0;
    m_comptype = COMPRESS_REASONABLE;
    m_smooth = false;
    texpath = texturepath;
}
void SMFMap::SetVegetationMap(std::string path)
{
    Image * img = new Image(path.c_str());
    if ( img->w > 0 )
    {
        if ( vegetationmap )
            delete vegetationmap;
        vegetationmap = img;
        vegetationmap->ConvertToLUM();
        if ( img->w != mapx/4 || img->h != mapy/4 )
        {
            std::cerr << "Warning: Vegetation map has wrong size , rescaling!" << std::endl;
            vegetationmap->Rescale(mapx/4,mapy/4);

        }
    } 
}
void SMFMap::AddFeature(std::string name, float x, float y, float z, float orientation)
{
  if ( features.find(name) == features.end() )//Allocate new vector
  {
    features[name] = new std::list<MapFeatureStruct*>();
  }
  MapFeatureStruct * feat = (MapFeatureStruct*)malloc(sizeof(MapFeatureStruct));
  feat->xpos = x;
  feat->ypos = y;
  feat->zpos = z;
  feat->rotation = 32767.0f-((orientation/360.0)*65535.0f);
  feat->relativeSize = 1;
  features[name]->push_back(feat);
}

void SMFMap::SetHeightRange(float minh, float maxh)
{
  m_minh = minh;
  m_maxh = maxh;
}

SMFMap::~SMFMap()
{
    delete m_tiles;
    if ( metalmap )
      delete metalmap;
    if ( heightmap )
      delete heightmap;
    if ( typemap )
      delete typemap;
    if ( minimap )
      delete minimap;
    if ( texture )
      delete texture;
    if ( vegetationmap )
      delete vegetationmap;
}
void SMFMap::SetMiniMap(std::string path)
{
    delete texture;
    std::cout << "Loading minimap " << path << std::endl;
    Image * img = new Image(path.c_str());
    if ( img->w > 0 )
    {
        if ( minimap )
            delete minimap;
        minimap = img;
        minimap->ConvertToRGBA();
	minimap->FlipVertical();
	if ( img->w != 1024 || img->h != 1024 )
      {
	  std::cerr << "Warning: Minimap has wrong size , rescaling!" << std::endl;
	  minimap->Rescale(1024,1024);

      }
      texture = new Image(texpath.c_str());
      return;
    }
    std::cout << "Failed " << path << std::endl;
    texture = new Image(texpath.c_str());
}

/*void SMFMap::SetFeatureMap(std::string path)
{
  Image * img = new Image(path.c_str());
  if ( img->w > 0 )
  {
    if ( featuremap )
      delete featuremap;
    featuremap = img;
    featuremap->ConvertToLUM();
  }
  if ( img->w != mapx/2 || img->h != mapy/2 )
  {
    std::cerr << "Warning: Feature map has wrong size , rescaling!" << std::endl;
    heightmap->Rescale(mapx+1,mapy+1);

  }
}*/
void SMFMap::SetHeightMap(std::string path)
{
    Image * img = new Image(path.c_str(),true);
    if ( img->w > 0 )
    {
        if ( heightmap )
            delete heightmap;
        heightmap = img;
        heightmap->ConvertToLUMHDR();
        if ( img->w != mapx+1 || img->h != mapy+1 )
        {
            std::cerr << "Warning: Height map has wrong size , rescaling!" << std::endl;
            heightmap->Rescale(mapx+1,mapy+1);

        }
        //Clamp heightmap before blurring
        float _min = 65535.0f;
	float _max = -65335.0f;
	short * pixels = (short*)heightmap->datapointer;
	for ( int i = 0; i < heightmap->w*heightmap->h; i++ )
	{
	  if ( _min > pixels[i] )
	    _min = pixels[i];
	  if ( _max < pixels[i] )
	    _max = pixels[i];
	  
	}
	std::cout << "Range : " << _min << " -> " << _max << std::endl;
	float range = _max-_min;
	for ( int i = 0; i < heightmap->w*heightmap->h; i++ )
	{
	  pixels[i] = short(((pixels[i]-_min)/range)*32767.0f);
	  
	}
        if ( m_smooth )
	{
	  std::cout << "Blurring heightmap..." << std::endl;
	  ilBindImage(heightmap->image);
	  iluBlurGaussian(30);
	  
	}
    }

}
void SMFMap::SetBlur(bool b)
{
  m_smooth = b;
}
void SMFMap::SetCompareTileCount(uint32_t count)
{
  m_tiles->SetDictSize(count);
}

void SMFMap::SetMetalMap(std::string path)
{
    Image * img = new Image(path.c_str());
    if ( img->w > 0 )
    {
        if ( metalmap )
            delete metalmap;
        metalmap = img;
        metalmap->ConvertToLUM();
	
        if ( img->w != mapx/2 || img->h != mapy/2 )
        {
            std::cerr << "Warning: Metal map has wrong size , rescaling!" << std::endl;
            metalmap->Rescale( mapx/2,mapy/2);

        }
    }
}
void SMFMap::SetTypeMap(std::string path)
{
    Image * img = new Image(path.c_str());
    if ( img->w > 0 )
    {
        if ( typemap )
            delete typemap;
        typemap = img;
        typemap->ConvertToLUM();
        if ( img->w != mapx/2 || img->h != mapy/2 )
        {
            std::cerr << "Warning: Type map has wrong size , rescaling!" << std::endl;
            typemap->Rescale(mapx/2,mapy/2);

        }

    }

}

void SMFMap::Compile()
{
    SMFHeader hdr;
    strcpy(hdr.magic,"spring map file");
    hdr.version = 1;
    hdr.mapid = rand();
    hdr.mapx = (texture->w / 1024)*128;
    hdr.mapy = (texture->h / 1024)*128;
    hdr.squareSize = 8;
    hdr.texelPerSquare = 8;
    hdr.tilesize = 32;
    hdr.minHeight = m_minh;
    hdr.maxHeight = m_maxh;


    short int * hmap = new short int[(mapy+1)*(mapx+1)];bzero(hmap,((mapy+1)*(mapx+1))*2);
    if ( heightmap )
    {
        heightmap->GetRect(0,0,heightmap->w,heightmap->h,IL_LUMINANCE,IL_SHORT,hmap);
    }
    unsigned char * typedata = new unsigned char[mapy/2 * mapx/2];bzero(typedata,(mapy/2 * mapx/2));
    if ( typemap )
    {
      typemap->GetRect(0,0,typemap->w,typemap->h,IL_LUMINANCE,IL_UNSIGNED_BYTE,typedata);
      
    }
    uint8_t * minimap_data = new uint8_t[699064];
    bzero(minimap_data,699064);
    if ( minimap )
    {
        int p = 0;
        int s = 1024;

        Image * im2 = new Image();
        im2->AllocateRGBA(1024,1024,(char*)minimap->datapointer);
        for ( int i = 0; i < 9; i++ )
        {
	    //std::cout << ">Mipmap " << i << std::endl;
	    im2->Rescale(s,s);
	    //std::cout << "<Mipmap " << i << std::endl;
	    ILuint ss;
            ILubyte * dxtdata = ilCompressDXT(im2->datapointer,s,s,1,IL_DXT1,&ss);
	    //std::cout << ss << " " << s;
	    memcpy(&minimap_data[p],dxtdata,ss);
	    free(dxtdata);
            p += ss;
	    
            s = s >> 1;
	    
        }
        delete im2;

    }
    unsigned char * metalmap_data = new unsigned char[mapx/2 * mapy/2];bzero(metalmap_data,(mapy/2 * mapx/2));
    if ( metalmap )
    {
      metalmap->GetRect(0,0,metalmap->w,metalmap->h,IL_LUMINANCE,IL_UNSIGNED_BYTE,metalmap_data);
      
    }
    /*hdr.heightmapPtr = sizeof(hdr);
    hdr.typeMapPtr = hdr.heightmapPtr + ((mapy+1)*(mapx+1))*2;
    hdr.minimapPtr = hdr.typeMapPtr + (mapy/2 * mapx/2);
    hdr.metalmapPtr = hdr.minimapPtr + 699048;
    hdr.featurePtr = hdr.metalmapPtr + (mapy/2 * mapx/2);*/
    MapFeatureHeader mfhdr;
    
    hdr.tilesPtr = hdr.featurePtr + sizeof(mfhdr);
    hdr.numExtraHeaders = 1;
    ExtraHeader grassHeader;
    grassHeader.size = 4;
    grassHeader.type = 1;
    MapTileHeader mthdr;
    mthdr.numTileFiles = 1;
    unsigned char * grass_data = new unsigned char[mapx/4 * mapy/4];bzero(grass_data,mapx/4 * mapy/4);
    if ( vegetationmap )
    {
      vegetationmap->GetRect(0,0,vegetationmap->w,vegetationmap->h,IL_LUMINANCE,IL_UNSIGNED_BYTE,grass_data);
      
    }
    int * tiles = new int[mapx/4 * mapy/4];
    std::vector<uint64_t> order;
    DoCompress(tiles,order);
   /* for ( int y = 0; y < mapy/4; y++ )
    {
    for ( int x = 0; x < mapx/4; x++ )
      {
	printf("%5d,",tiles[(mapx/4)*y+x]);
      }
      printf("\n");
    }*/
    
    FILE * tilefile = fopen((m_name+std::string(".smt")).c_str(),"wb");
    delete texture; //Temporarily delete texture from memory to reduce mem usage
    m_tiles->WriteToFile(tilefile,order);
    texture = new Image(texpath.c_str());
     
    fclose(tilefile);
    FILE * smffile = fopen((m_name+std::string(".smf")).c_str(),"wb");
    fwrite(&hdr,sizeof(hdr),1,smffile);
    fwrite(&grassHeader,sizeof(grassHeader),1,smffile);
    int _ofs = ftell(smffile)+4;
    fwrite(&_ofs,4,1,smffile);
    fwrite(grass_data,mapx/4 * mapy/4,1,smffile);
    
    hdr.minimapPtr  = ftell(smffile);
    fwrite(minimap_data,699064,1,smffile);
    hdr.heightmapPtr = ftell(smffile);
    fwrite(hmap,((mapy+1)*(mapx+1))*2,1,smffile);
    hdr.typeMapPtr = ftell(smffile);
    fwrite(typedata,mapy/2 * mapx/2,1,smffile);
    
    hdr.metalmapPtr  = ftell(smffile);
    fwrite(metalmap_data,mapy/2 * mapx/2,1,smffile);
    hdr.featurePtr  = ftell(smffile);
    

    mfhdr.numFeatures = 0;
    for ( std::map<std::string,std::list<MapFeatureStruct*> * >::iterator it = features.begin(); it != features.end(); it++ )//Enumerate features
      mfhdr.numFeatures += (*it).second->size();
    mfhdr.numFeatureType = features.size();
    fwrite(&mfhdr,sizeof(mfhdr),1,smffile);
    {
      std::map<std::string,unsigned int> featureTypes;
      unsigned int z = 0;
      for ( std::map<std::string,std::list<MapFeatureStruct*> * >::iterator it = features.begin(); it != features.end(); it++ )//Write feature types
      {
	fwrite((*it).first.c_str(),(*it).first.size()+1,1,smffile);
	featureTypes[(*it).first] = z++;
      }
      for ( std::map<std::string,std::list<MapFeatureStruct*> * >::iterator it = features.begin(); it != features.end(); it++ )//Write feature types
      {
	for ( std::list<MapFeatureStruct*>::iterator it2 = (*it).second->begin(); it2 != (*it).second->end(); it2++ )
	{
	  (*it2)->featureType = featureTypes[(*it).first];
	  if ( (*it2)->ypos < 490000.0f ) // Align on terrain
	  {
	    unsigned int hmapx = ((*it2)->xpos/float((mapx/128)*1024))*heightmap->w;
	    unsigned int hmapy = ((*it2)->zpos/float((mapy/128)*1024))*heightmap->h;
	    (*it2)->ypos = hdr.minHeight+(float(hmap[hmapy*(mapx+1)+hmapx])/32767.0)*(hdr.maxHeight-hdr.minHeight);
	    std::cout << "Feature " << (*it).first << " Instance " << (*it2) << " Terrain height: " << (*it2)->ypos << std::endl;
	  }
	  
	  fwrite((*it2),sizeof(MapFeatureStruct),1,smffile);
	}
	
      }
      
      
      
    }
    
    hdr.tilesPtr = ftell(smffile);
    uint32_t tc = m_tiles->GetTileCount();
    mthdr.numTiles = tc;
    fwrite(&mthdr,sizeof(mthdr),1,smffile);
    fwrite(&tc,4,1,smffile);
    fwrite((m_name+std::string(".smt")).c_str(),(m_name+std::string(".smt")).length()+1,1,smffile);
    fwrite(tiles,(mapx/4 * mapy/4)*4,1,smffile);
    fseek(smffile,0,SEEK_SET);
    fwrite(&hdr,sizeof(hdr),1,smffile);
    fclose(smffile);
    delete metalmap_data;
    delete hmap;
    delete typedata;
    delete tiles;
    delete minimap_data;
    delete grass_data;
}

void SMFMap::DoCompress(int* indices, std::vector< uint64_t >& order)
{
  order.clear();
  
  uint8_t tiledata[32*32*4];
  std::map<uint64_t,uint32_t> existingtiles;
  int c;
  for ( int y = 0; y < mapy/4; y++ )
  {
    for ( int x = 0; x < mapx/4; x++ )
    {
      if ( c % 50 == 0 )
	printf("\rCompressing %8d/%8d      - %6d tiles                    ",c,mapy/4*mapx/4,m_tiles->GetTileCount());
      c++;
      texture->GetRect(x*32,y*32,32,32,IL_RGBA,IL_UNSIGNED_BYTE,tiledata);
      for ( int yy = 0; yy < 16; yy++ )//Flip vertically
      {
	char tmprow[32*4];
	memcpy(tmprow,&tiledata[(31-yy)*32*4],32*4);
	memcpy(&tiledata[(31-yy)*32*4],&tiledata[yy*32*4],32*4);
	memcpy(&tiledata[yy*32*4],tmprow,32*4);
      }
     // std::cout << "Compressing (" << x << "," << y << ")" << std::endl;
      uint64_t uid = m_tiles->AddTileOrGetSimiliar(tiledata,m_th,m_comptype);
      if ( existingtiles.find(uid) == existingtiles.end() )
      {
	indices[(mapx/4)*y+x] = order.size();
	existingtiles[uid] = order.size();
	order.push_back(uid);
      }else{
	indices[(mapx/4)*y+x] = existingtiles[uid];
	
      }
      
      
    }
    
    
    
  }
  printf("\n");
  std::cout << "Compress done , ratio: " << float(existingtiles.size())/float(mapy/4 * mapx/4)*100.0 << std::endl;
  
}
void SMFMap::SetCompressionTol(float th)
{
  m_th = th;
}
void SMFMap::SetCompressionType(int c)
{
  m_comptype = c;
}

