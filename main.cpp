#include <iostream>
#include <string>
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

int main(int argc, char* argv[])
{
	// create an image object from file
	FlexibleImage<unsigned char> scene("scene.jpg");

	// create a display
	CImgDisplay before(DISP_X, DISP_Y, "Before");
	CImgDisplay after(DISP_X, DISP_Y, "After");

	scene.moveCornersTo(-1.f, -1.f,
		2.f, -1.f,
		-1.f, 2.f,
		2.f, 2.f);

	// show the image and its coordinate values
	before = scene.data();
	after = scene.rasterize();

	while(!(before.is_closed() && after.is_closed()));

	return 0;
}

// rectify image:
//		apply anti-distortion map
//		apply perspective transform (implemented)
