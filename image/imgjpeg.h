#ifndef IMGJPEG_H
#define IMGJPEG_H

#include "host.h"

inT8 open_jpeg_image(               //read header
	int fd,        //file to read
	inT32 *xsize,  //size of image
	inT32 *ysize,
	inT8 *bpp,     //bits per pixel
	inT8 *photo,
	inT32 *res     //resolution
);
inT8 read_jpeg_image(                //read header
	int fd,         //file to read
	uinT8 *pixels,  //pixels of image
	inT32 xsize,    //size of image
	inT32 ysize,
	inT8 bpp,       //bits per pixel
	inT32           //bytes per line
);
inT8 write_jpeg_image(                //write whole image
	int fd,         //file to write on
	uinT8 *pixels,  //image pixels
	inT32 xsize,    //size of image
	inT32 ysize,
	inT8 bpp,       //bits per pixel
	inT8,
	inT32           //resolution
);

#endif

