//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    image_io.cpp
  \brief   C++ Implementation: ImageIO
  \author  Stefan Zickler, (C) 2008
*/
//========================================================================
#include <stdio.h>
#include <QImage>
#include "image_io.h"

void ImageIO::copyBGRAtoRGBA(rgba * dst,unsigned char * src,unsigned int size) {
  unsigned int i;
  for (i=0;i!=size;i++) {
    dst->b=(*src);
    src++;
    dst->g=(*src);
    src++;
    dst->r=(*src);
    src++;
    dst->a=(*src);
    if (i != (size-1)) {
      src++;
      dst++;
    }
  }
}

void ImageIO::copyBGRtoRGB(rgb * dst,unsigned char * src,unsigned int size) {
  unsigned int i;
  for (i=0;i!=size;i++) {
    dst->b=(*src);
    src++;
    dst->g=(*src);
    src++;
    dst->r=(*src);
    if (i != (size-1)) {
      src++;
      dst++;
    }
  }
}

unsigned char * ImageIO::readGrayscale(int &width,int &height, const char *filename)
{
  uchar *imgbuf = NULL;
  width = height = 0;
  // load image
  QImage *img = NULL;
  
  //Fl_RGB_Image *img = NULL;
  img = new QImage(filename);
  if (img==0) return NULL;
  if (img->isGrayscale()==false) {
    printf("IMAGE IS NOT GREYSCALE!\n");
  }
  
  /*if(strstr(filename,".jpg")){    
    img = new Fl_JPEG_Image(filename);
  }else if(strstr(filename,".png")){
    img = new Fl_PNG_Image(filename);
  }else if(strstr(filename,".ppm")){
    img = new Fl_PNM_Image(filename);
  }else{
    return(NULL);
  }*/
  // make sure its the right format
  int sz = img->width() * img->height();
  if(!img->bits() || img->depth()!=8 || sz==0) goto end;

  // TODO: handle 2,3,4 channel images

  // allocate an image buffer
  imgbuf = new uchar[sz];
  if(!imgbuf) goto end;

  // copy the data
  width  = img->width();
  height = img->height();

  memcpy(imgbuf,img->bits(),width*height*sizeof(uchar));


 end:
  delete(img);
  return(imgbuf);
}

rgb * ImageIO::readRGB(int &width,int &height, const char *filename)
{
  rgb *imgbuf = NULL;
  width = height = 0;
  
  QImage *img = NULL;
  
  //Fl_RGB_Image *img = NULL;
  img = new QImage(filename);
  
  if (img==0) return NULL;

  // make sure its the right format
  int sz = img->width() * img->height();
  if((img->bits()==0) || img->depth()!=24 || sz==0) goto end;
  // allocate an image buffer
  imgbuf = new rgb[sz];
  if(!imgbuf) goto end;

  // copy the data
  width  = img->width();
  height = img->height();
  //memcpy(imgbuf,img->bits(),width*height*sizeof(rgb));
  copyBGRtoRGB(imgbuf,img->bits(),width*height);
 end:
  delete(img);
  return(imgbuf);
}

rgba * ImageIO::readRGBA(int &width,int &height, const char *filename)
{
  rgba *imgbuf = NULL;
  width = height = 0;
  QImage *img = NULL;
  //Fl_RGB_Image *img = NULL;
  img = new QImage(filename);
  if (img==0) return NULL;
  // make sure its the right format
  int sz = img->width() * img->height();
  if((img->bits()==0) || img->depth()!=32 || sz==0) goto end;
  // allocate an image buffer
  imgbuf = new rgba[sz];
  if(!imgbuf) goto end;

  // copy the data
  width  = img->width();
  height = img->height();
  //memcpy(imgbuf,img->bits(),width*height*sizeof(rgba));
  copyBGRAtoRGBA(imgbuf,img->bits(),width*height);
 end:
  delete(img);
  return(imgbuf);
}

bool ImageIO::writePPM(rgb *imgbuf, int width, int height, const char *filename)
{
  // open output file
  FILE *out = fopen(filename,"wb");
  if(!out) return(false);

  // write the image
  fprintf(out,"P6\n%d %d\n%d\n",width,height,255);
  int result=fwrite(imgbuf,3,width*height,out);
  (void)result; //get the compiler to shut up.

  return(fclose(out) == 0);
}

bool ImageIO::writeRGB(rgb *imgbuf, int width, int height, const char *filename)
{
  const char *ext = strrchr(filename,'.');

  if(strcmp(ext,".ppm") == 0) return(ImageIO::writePPM (imgbuf,width,height,filename));
  if(strcmp(ext,".jpg") == 0) return(ImageIO::writeJPEG(imgbuf,width,height,filename,90));
 // if(strcmp(ext,".png") == 0) return(WritePNG (imgbuf,width,height,filename));

  printf("WriteRGB: Unknown extension \"%s\"\n",ext);
  return(false);
}




bool ImageIO::writeJPEG (rgb *imgbuf, int width, int height, const char * filename, int quality, bool flipY) {
  //Most code in this function has been adapted from the libjpeg
  //example code.
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * outfile;               /* target file */
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */
  unsigned char * image_buffer=(unsigned char *)imgbuf;

  /* Step 1: allocate and initialize JPEG compression object */
  /* Setup error handler */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);
  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    jpeg_destroy_compress(&cinfo);
    return false;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = width;      /* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;           /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
  * (You must set at least cinfo.in_color_space before calling this,
  * since the defaults depend on the source color space.)
  */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
  * Here we just illustrate the use of quality (quantization table) scaling:
  */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
  * Pass TRUE unless you are very sure of what you're doing.
  */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
  * loop counter, so that we don't have to keep track ourselves.
  * To keep things simple, we pass one scanline per call; you can pass
  * more if you wish, though.
  */
  row_stride = width * 3; /* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
  /* jpeg_write_scanlines expects an array of pointers to scanlines.
    * Here the array is only one element long, but you could pass
    * more than one scanline at a time if that's more convenient.
    */
    row_pointer[0] = & image_buffer[ (flipY ? (height-cinfo.next_scanline) : cinfo.next_scanline) * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */
  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */
  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);
  /* And we're done! */
  return true;
}










//bool WritePNG(rgb *imgbuf, int width, int height, const char *filename)
//{
  /*png_structp png = NULL;
  png_infop info = NULL;
  png_bytep *row = NULL;
  bool ok = false;

  // open the output file
  FILE *out = fopen(filename,"wb");
  if(!out) return(false);

  // create writing and info header structures
  png = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
  if(!png) goto end;
  info = png_create_info_struct(png);
  if(!info) goto end;

  // set the error handler
  if(setjmp(png_jmpbuf(png))) goto end;

  // Set up the output stream
  png_init_io(png,out);

  // Set image information
  png_set_IHDR(png, info, width, height, 8,
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_ADAM7,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // Write the file header information.
  png_write_info(png,info);

  // swap location of alpha bytes from ARGB to RGBA
  // png_set_swap_alpha(png);

  // Get rid of filler byte if present (4->3 channels)
  // png_set_filler(png, 0, PNG_FILLER_BEFORE);

  // set up row pointers
  row = new png_bytep[height];
  if(!row) goto end;
  for(int i=0; i<height; i++){
    row[i] = (png_bytep)(&imgbuf[i * width]);
  }

  // write out the image
  png_write_image(png,row);
  png_write_end(png,info);

  // clean up and exit
  ok = true;
 end:
  delete[](row);
  if(png) png_destroy_write_struct(&png, &info);
  fclose(out);
  return(ok);*/
//}
