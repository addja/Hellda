
#include "include/cTexture.h"
#include "include/corona.h"
#include "include/Globals.h"

cTexture::cTexture(void) {}

cTexture::~cTexture(void) {}

bool cTexture::Load(char *filename,int type,int wraps,int wrapt,int magf,int minf,bool mipmap) {
	corona::Image* img;
	int components;

	img = corona::OpenImage(filename);

	if (type==GL_RGB) {
		// img = corona::openimage(filename,corona::pf_r8g8b8);
		components = 3;
	} else if (type==GL_RGBA) {
		// img = corona::openimage(filename,corona::pf_r8g8b8a8);
		components = 4;
	} else return false;

	if (img==NULL) return false;

	width  = img->getWidth();
	height = img->getHeight();

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D,id);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wraps);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrapt);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magf);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minf);

	if (!mipmap) {
		glTexImage2D(GL_TEXTURE_2D,0,components,width,height,0,type,
					 GL_UNSIGNED_BYTE,img->getPixels());
	} else {
		gluBuild2DMipmaps(GL_TEXTURE_2D,components,width,height,type,
						  GL_UNSIGNED_BYTE,img->getPixels());
	}

	return true;
}

int cTexture::GetID() {
	return id;
}

void cTexture::GetSize(int *w,int *h) {
	*w = width;
	*h = height;
}