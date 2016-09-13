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

		// initialize float-value image coordinates from 0 to 1
		float currentWidth = 0.f, currentHeight = 0.f;
		for(int r = 0; r < height(); ++r)
		{
			currentHeight = static_cast<float>(r) / height();

			for(int c = 0; c < width(); ++c)
			{
				currentWidth = static_cast<float>(c) / width();

				xAt(r, c) = currentWidth;
				yAt(r, c) = currentHeight;
			}
		}
	}

	~FlexibleImage() = default;

	int width(void) const { return img.width(); }
	int height(void) const { return img.height(); }

	CImg<T>& data(void) { return img; }
	CImg<float>& xData(void) { return x; }
	CImg<float>& yData(void) { return y; }

	// return desired coord of specified pixel (mutable)
	float& xAt(int pixRow, int pixCol)
		{ return *(x.begin() + pixRow * width() + pixCol); }
	float& yAt(int pixRow, int pixCol)
		{ return *(y.begin() + pixRow * width() + pixCol); }

	// alter coordinates so pixel coordinates follow these corner positions
	// (a.k.a. perspective shift)
	void moveCornersTo(float xTopLeft, float yTopLeft,
		float xTopRight, float yTopRight,
		float xBottomLeft, float yBottomLeft,
		float xBottomRight, float yBottomRight)
	{
		// calculate initial and final slopes and constant offsets
		float	yInitSlope = yTopRight - yTopLeft,
				yFinalSlope = yBottomRight - yBottomLeft,
				yInitIntercept = yTopLeft,
				yFinalIntercept = yBottomLeft;

		float	xInitSlope = xBottomLeft - xTopLeft,
				xFinalSlope = xBottomRight - xTopRight,
				xInitIntercept = xTopLeft,
				xFinalIntercept = xTopRight;

		// perform transformation
		float x_base, y_base;
		for(int r = 0; r < height(); ++r)
		{
			// base values calculated for each pixel
			y_base = static_cast<float>(r) / height();

			for(int c = 0; c < width(); ++c)
			{
				// base values calculated for each pixel
				x_base = static_cast<float>(c) / width();

				xAt(r, c) = ((xFinalSlope - xInitSlope) * x_base + xInitSlope) * y_base	+ (xFinalIntercept - xInitIntercept) * x_base + xInitIntercept;
				yAt(r, c) = ((yFinalSlope - yInitSlope) * y_base + yInitSlope) * x_base + (yFinalIntercept - xInitIntercept) * y_base + yInitIntercept;
			}
		}
	}
};

int main(int argc, char* argv[])
{
	// create an image object from file
	FlexibleImage<unsigned char> scene("scene.jpg");

	// create a display
	CImgDisplay disp(DISP_X, DISP_Y, "Image");
	CImgDisplay xDisp(DISP_X, DISP_Y, "X Coordinates", 2);
	CImgDisplay yDisp(DISP_X, DISP_Y, "Y Coordinates", 2);

	// set coordinate display scaling
	xDisp = scene.xData();
	yDisp = scene.yData();

	scene.moveCornersTo(0.f, 0.f,
		1.f, 0.f,
		0.f, 0.f,
		1.f, 1.f);

	// show the image and its coordinate values
	disp = scene.data();
	xDisp = scene.xData();
	yDisp = scene.yData();

	while(!(disp.is_closed() || xDisp.is_closed() || yDisp.is_closed()));

	return 0;
}

// linear transforms
/*
give ea. pixel float coords [0.0, 1.0)
mul. coords by transformation matrix
linearly interpolate mem. pixel positions from float coords?
*/