/*
    <one line to give the program's name and a brief idea of what it does.>
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


#include "Image.h"

CannotLoadImageException::CannotLoadImageException(std::string path) : runtime_error(path) 
{

}

Image::Image()
{
    ilGenImages(1,&image);



}
void Image::GetRect(int x, int y, int w, int h,ILenum format, void * dest)
{
  ilBindImage(image);
  ilCopyPixels(x,y,0,w,h,1,format,IL_UNSIGNED_BYTE,dest);
}
void Image::GetRect(int x, int y, int w, int h, ILenum format, ILenum type, void* dest)
{
  ilBindImage(image);
  ilCopyPixels(x,y,0,w,h,1,format,type,dest);
}
void Image::FlipVertical()
{
  ilBindImage(image);
  iluFlipImage();
}

Image::Image(const char* filename, bool hdrlum )
{

    ilGenImages(1,&image);
    ilBindImage(image);
    if ( ! ilLoadImage(filename) )
    {
      throw CannotLoadImageException(std::string(filename));
      
    }
    if (!hdrlum)
    {
      ConvertToRGBA();
    }else if ( ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL ) != 2 ||  ilGetInteger(IL_IMAGE_FORMAT) != IL_LUMINANCE ) {
      ConvertToLUMHDR();
    }
    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    d = ilGetInteger(IL_IMAGE_DEPTH);
    datapointer = ilGetData();

}
void Image::Save(const char* filename)
{
    ilBindImage(image);
    ilSaveImage(filename);
}
void Image::AllocateLUM(int x, int y, char* data)
{
    ilBindImage(image);
    ilTexImage(x,y,1,1,IL_LUMINANCE,IL_UNSIGNED_BYTE,data);
    datapointer = ilGetData();
    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    d = ilGetInteger(IL_IMAGE_DEPTH);
}

void Image::AllocateRGBA(int x, int y, char* data)
{
    ilBindImage(image);
    ilTexImage(x,y,1,4,IL_RGBA,IL_UNSIGNED_BYTE,data);
    datapointer = ilGetData();
    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    d = ilGetInteger(IL_IMAGE_DEPTH);
}
void Image::Rescale(int x, int y)
{
    ilBindImage(image);
    iluScale(x,y,1);
    datapointer = ilGetData();
    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    d = ilGetInteger(IL_IMAGE_DEPTH);
}

void Image::GetPixelRGBA(int x_, int y_, unsigned char* pix)
{

    int x = x_ % w;
    int y = h -(y_ % h);

    if ( datapointer )
    {
        pix[0] = datapointer[w*y*4+x*4];
        pix[1] = datapointer[w*y*4+x*4+1];
        pix[2] = datapointer[w*y*4+x*4+2];
	pix[3] = datapointer[w*y*4+x*4+3];
    } else {
        printf("GetPixelRGBA(%i,%i): datapointer is NULL\n",x_,y_);
    }
    //printf("%i %i %i\n",(int)pix[0],(int)pix[1],int(pix[2]));
}



void Image::SetPixelRGBA(int x_, int y_, char r, char g, char b, char a)
{
    int x = (x_ % w);
    int y = h - (y_ % h) - 1;
    datapointer[w*y*3+x*3] = r;
    datapointer[w*y*3+x*3+1] = g;
    datapointer[w*y*3+x*3+2] = b;
    datapointer[w*y*4+x*4+3] = a;

}

void Image::SetPixelLUM(int x_, int y_, char val)
{
    int x = x_ % w;
    int y = y_ % h;
    datapointer[y*w+x] = val;


}
void Image::GetPixelLUM(int x_, int y_, unsigned char* p)
{
    //printf("GetPixelLUM(%i,%i)\n",x_,y_);
    int x = x_ % w;
    int y = y_ % h;
    p[0] = datapointer[y*w+x];

}
void Image::ConvertToLUM()
{
    ilBindImage(image);
    ilConvertImage(IL_LUMINANCE,IL_UNSIGNED_BYTE);
    datapointer = ilGetData();

}
void Image::ConvertToLUMHDR()
{
    ilBindImage(image);
    ilConvertImage(IL_LUMINANCE,IL_UNSIGNED_SHORT);
    datapointer = ilGetData();
}

void Image::ConvertToRGBA()
{
    ilBindImage(image);
    ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);
    datapointer = ilGetData();
}
Image::~Image()
{
    ilDeleteImages(1,&image);
}


