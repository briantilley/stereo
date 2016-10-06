#include <iostream>
#include <string>
#include <cstring>
#include "CImg.h"

// use libjpeg rather than ImageMagick
#define cimg_use_jpeg

// window dimensions
#define DISP_X 1280
#define DISP_Y 800

using namespace std;
using namespace cimg_library;

#include "FlexibleImage.h"
#include "Image.h"

template<typename T>
T* loadImage(string filepath, unsigned& width, unsigned& height)
{
	// load from file
	CImg<T> container(filepath.c_str());

	// allocate copy
	T* retVal = new T[container.size()*sizeof(T)];

	// copy data
	for(int i = 0; i < container.size(); ++i)
		retVal[i] = container.data()[i];
	width = container.width();
	height = container.height();

	return retVal;
}

template<typename T, bool color>
void showImage(const Image<T, color>& img, CImgDisplay& disp)
{
	disp = CImg<T>(img.data(), img.width(), img.height(), 1, (color ? 3 : 1));
}

int main(int argc, char* argv[])
{
	// create an image object from file
	unsigned width, height;
	Image<unsigned char, true> img(loadImage<unsigned char>("scene.jpg", width, height), width, height);
	// FlexibleImage<unsigned char> scene("scene.jpg")

	// create a display
	CImgDisplay before(DISP_X, DISP_Y, "Before");
	CImgDisplay after(DISP_X, DISP_Y, "After");

	showImage(img, before);

	// scene.moveCornersTo(-1.f, -1.f,
	// 	2.f, -1.f,
	// 	-1.f, 2.f,
	// 	2.f, 2.f);

	// // show the image and its coordinate values
	// before = scene.data();
	// after = scene.rasterize();

	while(!(before.is_closed() && after.is_closed()));

	return 0;
}

// rectify image:
//		apply anti-distortion map
//		apply perspective transform (implemented)
