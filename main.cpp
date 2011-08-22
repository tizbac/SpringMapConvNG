

#include <IL/il.h>
#include <iostream>
#include <string.h>
#include "SMFMap.h"

#include <stdlib.h>
int main(int argc, char** argv)
{
    ilInit();
    if ( argc == 1 )
    {
      std::cout << "Usage: " << argv[0] << " -t [maintexture] -m [metalmap] -z [typemap] -h [heightmap] -maxh [white height value] -minh [black height value] -o [outputsuffix] -minimap [minimap_image]" << std::endl;
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
      float minh = 0.0f;
      float maxh = 1.0f;
      int ct = COMPRESS_REASONABLE;
      float th = 1.0;
      for ( int i = 1; i < argc-1; i++ )
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
	      
	    }else if ( strcmp(&argv[i][1],"th") == 0 )//Compression 
	    {
	      if ( i+1 < argc )
	      {
		th = atof(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }
	    else if ( strcmp(&argv[i][1],"ct") == 0 )//Compression 
	    {
	      if ( i+1 < argc )
	      {
		ct = atoi(argv[++i]);
	      }else{
		goto error;
	      }
	      
	    }else if ( strncmp(&argv[i][1],"h",1) == 0 )//Help
	    {
	      goto error;
	    }
	    
	    
	    
	    
	    
	    
	  }
	  
	}
	
      }
      
      if ( valid1 && valid2 )
	goto success;
      error:
	std::cout << "Usage: " << argv[0] << " -t [maintexture] -m [metalmap] -z [typemap] -h [heightmap] -maxh [white height value] -minh [black height value] -o [outputsuffix] -minimap [minimap_image]" << std::endl;
	return 1;
      success:
	SMFMap * m = new SMFMap(outputname,texture);
	if ( heightmap.length() > 0 ) m->SetHeightMap(heightmap);
	if ( metalmap.length() > 0 ) m->SetMetalMap(metalmap);
	if ( typemap.length() > 0 ) m->SetTypeMap(typemap);
	if ( minimap.length() > 0 ) m->SetMiniMap(minimap);
	m->SetHeightRange(minh,maxh);
	m->SetCompressionTol(th);
	m->SetCompressionType(ct);
	m->Compile();
    }
}
