#include <IL/il.h>
#include <iostream>
#include <string.h>
#include "SMFMap.h"

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#endif
void help(char ** argv)
{
  std::cout << "Usage: " << argv[0] << " -directory [directory where .smt files are] -mapfile [name of the smf file , NOT path ]" << std::endl;
  
}
int main(int argc, char** argv)
{
    ilInit();
    if ( argc == 1 )
    {
      
      help(argv);
      return 1;
    }else{
      std::string mapdirectory;
      std::string mapfile;
      bool valid1=false,valid2=false;
      for ( int i = 1; i < argc; i++ )
      {
	if ( strlen(argv[i]) > 1 )
	{
	  if ( argv[i][0] == '-' )
	  {
	    if ( strcmp(&argv[i][1],"directory") == 0 )
	    {
	      valid1 = true;
	      if ( i+1 < argc )
	      {
		mapdirectory = argv[++i];
	      }else{
		goto error;
	      }
	    }else if ( strcmp(&argv[i][1],"mapfile") == 0 )
	    {
	      if ( i+1 < argc )
	      {
		mapfile = argv[++i];
	      }else{
		goto error;
	      }
	      valid2 = true;
	      
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
#ifndef WIN32
	if ( chdir(mapdirectory.c_str()) )
#else
	if ( _chdir(mapdirectory.c_str()) )
#endif
	{
	  std::cerr << "Cannot change working directory to " << mapdirectory << std::endl;
	  return 1;
	}
	SMFMap * m = new SMFMap(mapfile);
	m->SaveSourceFiles();
	delete m;
    }
}

