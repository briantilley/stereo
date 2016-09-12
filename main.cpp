#include <iostream>
#include <string>
#include "CImg.h"

// use libjpeg rather than ImageMagick
#define cimg_use_jpeg

// window dimensions
#define DISP_X 640
#define DISP_Y 400

using namespace std;
using namespace cimg_library;

// rectify image:
//		apply anti-distortion map
//		apply perspective transform

// attach mutable coordinates to image
template<typename T>
class FlexibleImage
{
private:
	CImg<T> img;
	CImg<float> x;
	CImg<float> y;

public:
	FlexibleImage<T>(string filepath)
	{
		img = CImg<T>(filepath.c_str());
		x = CImg<float>(img.width(), img.height());
		y = CImg<float>(img.width(), img.height());

		// initialize float-value image coordinates
		float currentWidth = 0.f, currentHeight = 0.f;
		for(int r = 0; r < height(); ++r)
		{
			currentHeight = static_cast<float>(r) / height();

			for(int c = 0; c < width(); ++c)
			{
				currentWidth = static_cast<float>(c) / width();

				*(x.begin() + r * width() + c) = currentWidth;
				*(y.begin() + r * width() + c) = currentHeight;
			}
		}
	}

	~FlexibleImage() = default;

	int width(void) { return img.width(); }
	int height(void) { return img.height(); }

	CImg<T>& data(void) { return img; }
	CImg<float>& xData(void) { return x; }
	CImg<float>& yData(void) { return y; }

	// return desired coord of specified pixel
	float& xAt(int pixRow, int pixCol)
		{ return *(x.begin() + pixRow * width() + pixCol); }
	float& yAt(int pixRow, int pixCol)
		{ return *(y.begin() + pixRow * width() + pixCol); }
};

// // uses a 2x2 matrix to transform pixel coords
// void linearTransform(CImgList<float> coords, float matrix[])
// {

// }

int main(int argc, char* argv[])
{
	// create an image object from file
	FlexibleImage<unsigned char> ubuntu("scene.jpg");

	// create a display
	CImgDisplay disp(DISP_X, DISP_Y, "Image");
	CImgDisplay xDisp(DISP_X, DISP_Y, "X Coordinates", 2);
	CImgDisplay yDisp(DISP_X, DISP_Y, "Y Coordinates", 2);

	// set coordinate display scaling
	xDisp = ubuntu.xData();
	yDisp = ubuntu.yData();

	for(int i = 0; i < ubuntu.height(); ++i)
	{
		for(int j = 0; j < ubuntu.width(); ++j)
		{
			ubuntu.xAt(i, j) *= static_cast<float>(i) / ubuntu.height();
		}
	}

	// show the image and its coordinate values
	disp = ubuntu.data();
	xDisp = ubuntu.xData();
	yDisp = ubuntu.yData();

	while(!(disp.is_closed() || xDisp.is_closed() || yDisp.is_closed()));

	return 0;
}

// linear transforms
/*
give ea. pixel float coords [0.0, 1.0)
mul. coords by transformation matrix
linearly interpolate mem. pixel positions from float coords?
*/
