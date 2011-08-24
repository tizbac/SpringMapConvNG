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


#ifndef IMAGE_H
#define IMAGE_H
#include <IL/il.h>
#include <IL/ilu.h>
#include <string>
#include <stdexcept>
class CannotLoadImageException : public std::runtime_error
{
public:
  CannotLoadImageException(std::string path);
};
class Image{
  public:
    ILuint image;
    int w;
    int h;
    int d;
    unsigned char * datapointer;
    Image();
    
    Image(const char* filename, bool hdrlum = false);
    
    void Save(const char * filename);
    
    void AllocateLUM(int x, int y,char * data=NULL);
    
    void AllocateRGBA(int x, int y,char * data=NULL);
    
    void Rescale(int x , int y);
    
    void GetPixelRGBA(int x_,int y_,unsigned char * pix);
    
    void SetPixelRGBA(int x_, int y_,char r, char g, char b, char a);
    
    void SetPixelLUM(int x_, int y_, char val);
    
    void GetPixelLUM(int x_, int y_,unsigned char * p);
    
    void ConvertToLUM();
    
    void ConvertToLUMHDR();
    
    void ConvertToRGBA();
    
    void FlipVertical();
    void GetRect(int x, int y, int w, int h, ILenum format, void* dest);
    void GetRect(int x, int y, int w, int h, ILenum format , ILenum type, void* dest);
    ~Image();
    
};

#endif // IMAGE_H
