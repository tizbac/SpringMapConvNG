

#include <IL/il.h>
#include <iostream>
#include <string.h>
#include "SMFMap.h"

#include <stdlib.h>
void help(char ** argv)
{
  std::cout << "Usage: " << argv[0] << " -t [maintexture] -m [metalmap] -z [typemap] -h [heightmap] -maxh [white height value] -minh [black height value] -o [outputsuffix] -minimap [minimap_image]" << std::endl;
  std::cout << " -ct [compression_type] -ccount [compare_tilecount] -th [compression_level] -features [featurefile]" << std::endl;
  std::cout << "Compression types:\n\t1: No compression \n\t2: Fast compression , compare tile with last -ccount tiles , take first which difference is below -th\n\t3: Insane Compression: compare each tile with whole map , it is very SLOW, not recomended\n\t4: High quality Fast compression: Slightly slower than 2 , it searchs for less different tile in last -ccount tiles" << std::endl;
  std::cout << "Feature file: Each line is a feature instance and has the fields in the following order [tdfname] [xpos] [ypos] [zpos] [rotation yaxis] , please do not leave whitespaces at the end or it\n will give errors." << std::endl;
  std::cout << "If you specify less than -490000 as ypos , it will calculate ypos depending on terrain height" << std::endl;
}
int main(int argc, char** argv)
{
    ilInit();
    if ( argc == 1 )
    {
      
      help(argv);
      return 1;
    }else{
      bool valid2 = false;
      bool valid1 = false;
      std::string outputname;
      std::string texture;
      std::string minimap;
      std::string metalmap;
      std::string typemap;
      std::string heightmap;
      std::string vegmap;
      std::string featurefile;
      bool smooth = false;
      int tcount = 64;
      float minh = 0.0f;
      float maxh = 1.0f;
      int ct = COMPRESS_REASONABLE;
      float th = 0.8;
      for ( int i = 1; i < argc; i++ )
      {
	if ( strlen(argv[i]) > 1 )
	{
	  if ( argv[i][0] == '-' )
	  {
	    if ( strcmp(&argv[i][1],"t") == 0 )
	    {
	      valid1 = true;
	      if ( i+1 < argc )
	      {
		texture = argv[++i];
	      }else{
		goto error;
	      }
	    }else if ( strcmp(&argv[i][1],"m") == 0 )//Metal
	    {
	      if ( i+1 < argc )
	      {
		metalmap = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"z") == 0 )//Type
	    {
	      if ( i+1 < argc )
	      {
		typemap = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"h") == 0 )//Height
	    {
	      if ( i+1 < argc )
	      {
		heightmap = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"maxh") == 0 )//Max Height
	    {
	      if ( i+1 < argc )
	      {
		maxh = atof(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"minh") == 0 )//Min Height
	    {
	      if ( i+1 < argc )
	      {
		minh = atof(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"o") == 0 )//Output name
	    {
	      if ( i+1 < argc )
	      {
		outputname = argv[++i];
	      }else{
		goto error;
	      }
	      valid2 = true;
	      
	    }else if ( strcmp(&argv[i][1],"minimap") == 0 )//Minimap
	    {
	      if ( i+1 < argc )
	      {
		minimap = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"th") == 0 )//Compression level
	    {
	      if ( i+1 < argc )
	      {
		th = atof(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }
	    else if ( strcmp(&argv[i][1],"ct") == 0 )//Compression mode 
	    {
	      if ( i+1 < argc )
	      {
		ct = atoi(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"ccount") == 0 )//Compression , count of tiles to comapre in mode 2 and 4
	    {
	      if ( i+1 < argc )
	      {
		tcount = atoi(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }
	    else if ( strcmp(&argv[i][1],"v") == 0 )//Vegetation map
	    {
	      if ( i+1 < argc )
	      {
		vegmap = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }else if ( strcmp(&argv[i][1],"features") == 0 )//features file
	    {
	      
	      if ( i+1 < argc )
	      {
		featurefile = argv[++i];
	      }else{
		goto error;
	      }
	      
	    }
	      else if ( strcmp(&argv[i][1],"smooth") == 0 )//Smooth
	    {
	      smooth = true;
	      
	    }
	    else if ( strncmp(&argv[i][1],"h",1) == 0 )//Help
	    {
	      goto error;
	    }
	    
	    
	    
	    
	    
	    
	  }
	  
	}
	
      }
      
      if ( valid1 && valid2 )
	goto success;
      error:
	help(argv);
	return 1;
      success:
	SMFMap * m = new SMFMap(outputname,texture);
	m->SetBlur(smooth);
	if ( heightmap.length() > 0 ) m->SetHeightMap(heightmap);
	if ( metalmap.length() > 0 ) m->SetMetalMap(metalmap);
	if ( typemap.length() > 0 ) m->SetTypeMap(typemap);
	if ( minimap.length() > 0 ) m->SetMiniMap(minimap);
	if ( vegmap.length() > 0 ) m->SetVegetationMap(vegmap);
	m->SetCompareTileCount(tcount);
	m->SetHeightRange(minh,maxh);
	m->SetCompressionTol(th);
	m->SetCompressionType(ct);
	if ( featurefile.length() > 0 )
	{
	  FILE * ff = fopen(featurefile.c_str(),"r");
	  if (!ff)
	  {
	    std::cerr << "Cannot open feature file for reading!" <<std::endl;
	    return 1;
	  }
	  char line[512];
	  while ( fgets(line,511,ff ) )
	  {
	    float x,y,z,o;
	    char name[512];
	    if ( strlen(line) < 1 )
	      continue;
	    if ( sscanf(line,"%s %f %f %f %f",name,&x,&y,&z,&o) != 5 )
	    {
	      std::cerr << "Parse error @ '" << line << "'" << std::endl;
	      return 1;
	    }
	    m->AddFeature(name,x,y,z,o);
	  }
	  fclose(ff);
	  
	}
	m->Compile();
    }
}

