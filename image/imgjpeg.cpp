extern "C" {
    #include <stdio.h>
    #include <jpeglib.h>
    #include <string.h>
    #include <unistd.h>
}

#include "imgjpeg.h"

// jpeg opener. return -1 to indicate an error, or -2 upon success
inT8 open_jpeg_image(               //read jpeg header
		int fd,        //file to read
		inT32 *xsize,  //size of image
		inT32 *ysize,
		inT8 *bpp,     //bits per pixel
		inT8 *photo,   //unused
		inT32 *res     //resolution
		) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile;

	if ((infile = fdopen(fd, "rb")) == NULL) {
		fprintf(stderr, "error fdopen fd %d\n", fd); 
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	jpeg_read_header(&cinfo, TRUE);

	jpeg_calc_output_dimensions(&cinfo);
	*xsize = cinfo.output_width;
	*ysize = cinfo.output_height;
	*bpp = cinfo.out_color_components * 8;

	jpeg_destroy_decompress(&cinfo);

	return -2;
}

inT8 read_jpeg_image(                //read header
		int fd,         //file to read
		uinT8 *pixels,  //pixels of image
		inT32 xsize,    //size of image, unused
		inT32 ysize,
		inT8 bpp,       //bits per pixel, unused
		inT32            //bytes per line, unused
		) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int row_stride, nread = 0;
	JSAMPARRAY buffer;
	FILE *infile;

	lseek(fd, 0, SEEK_SET);
	if ((infile = fdopen(fd, "rb")) == NULL) {
		fprintf(stderr, "error fdopen fd %d\n", fd); 
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1); 
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(&pixels[nread*row_stride], buffer[0], row_stride);
		nread ++;
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}

inT8 write_jpeg_image(                //write whole image
		int fd,         //file to write on
		uinT8 *pixels,  //image pixels
		inT32 xsize,    //size of image
		inT32 ysize,
		inT8 bpp,       //bits per pixel
		inT8,
		inT32 res           //resolution
		) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int row_stride, nwrite = 0;
	JSAMPROW row_pointer[1];
	FILE *outfile;

	if ((outfile = fdopen(fd, "a")) == NULL) {
		fprintf(stderr, "error fdopen fd %d\n", fd); 
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = xsize;
	cinfo.image_height = ysize;
	cinfo.input_components = bpp / 8;
	switch(cinfo.input_components) {
	case 1:
		cinfo.in_color_space = JCS_GRAYSCALE;
		break;
	case 4:
		cinfo.in_color_space = JCS_YCCK;
		break;
	case 3:
	default:
		cinfo.in_color_space = JCS_RGB;
		break;
	}

	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = xsize * cinfo.input_components; 
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & pixels[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
}

