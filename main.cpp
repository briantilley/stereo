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

inline double pointDistance(double x1, double y1, double x2, double y2)
	{ return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)); }

inline int clamp(double a, int lowerBound, int upperBound)
	{ return min(max(static_cast<int>(a), lowerBound), upperBound); }

// attach mutable coordinates to image
// offer operations to conveniently manipulate image
template<typename T>
class FlexibleImage
{
private:
	CImg<T> img;
	CImg<float> x;
	CImg<float> y;

	// initialize float-value image coordinates from 0 to 1
	// (members only)
	void setDefaultCoordinates(void)
	{
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

public:
	// create image by opening file
	FlexibleImage<T>(string filepath)
	{
		img = CImg<T>(filepath.c_str());
		x = CImg<float>(img.width(), img.height());
		y = CImg<float>(img.width(), img.height());

		setDefaultCoordinates();
	}

	// create empty image with given dimensions
	// assume rgb
	FlexibleImage<T>(int width, int height)
	{
		img = CImg<T>(width, height, 1, 3);
		x = CImg<float>(width, height);
		y = CImg<float>(width, height);
	}

	~FlexibleImage() = default;

	int width(void) const { return img.width(); }
	int height(void) const { return img.height(); }

	CImg<T>& data(void) { return img; }
	CImg<float>& xData(void) { return x; }
	CImg<float>& yData(void) { return y; }

	// return desired coord of specified pixel (mutable)
	float& xAt(int pixRow, int pixCol)
		{ return x.at(pixRow * width() + pixCol); }
	float& yAt(int pixRow, int pixCol)
		{ return y.at(pixRow * width() + pixCol); }

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

				// apply slope-intercept formula to find new coordinates
				xAt(r, c) = ((xFinalSlope - xInitSlope) * x_base + xInitSlope) * y_base
					+ (xFinalIntercept - xInitIntercept) * x_base + xInitIntercept;
				yAt(r, c) = ((yFinalSlope - yInitSlope) * y_base
					+ yInitSlope) * x_base + (yFinalIntercept - xInitIntercept) * y_base + yInitIntercept;
			}
		}
	}

	// create a new raster image with transformations applied
	// coordinate value of -1.f indicates pixel is blank, not dark
	CImg<unsigned char> rasterize(void)
	{
		// resulting image (black by default)
		CImg<unsigned char> result(width(), height(), 1, 3);
		result.fill(0);

		// let each pixel contribute to an accumulator image
		// rgb, fourth color == 0.f means blank pixel
		CImg<float> accumulation(width(), height(), 1, 4);
		accumulation.fill(0.f);

		// accumulate weighted and record sums of weights into would-be alpha channel
		for(int r = 0; r < height(); ++r)
		{
			for(int c = 0; c < width(); ++c)
			{
				double denormX = xAt(r, c) * width();
				double denormY = yAt(r, c) * height();

				// accumulate into upper left neighboring pixel
				double distance = pointDistance(floor(denormX), floor(denormY), xAt(r, c), yAt(r, c));

				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 0)
					+= data().atX(c, r, 0, 0) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 1)
					+= data().atX(c, r, 0, 1) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 2)
					+= data().atX(c, r, 0, 2) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 3)
					+= 1 / distance;
				
				// return result;

				// accumulate into upper right neighboring pixel
				distance = pointDistance(floor(denormX + .5), floor(denormY), xAt(r, c), yAt(r, c));
				
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 0)
					+= data().atX(c, r, 0, 0) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 1)
					+= data().atX(c, r, 0, 1) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 2)
					+= data().atX(c, r, 0, 2) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 3)
					+= 1 / distance;

				// accumulate into lower left neighboring pixel
				distance = pointDistance(floor(denormX), floor(denormY + .5), xAt(r, c), yAt(r, c));
				
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 0)
					+= data().atX(c, r, 0, 0) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 1)
					+= data().atX(c, r, 0, 1) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 2)
					+= data().atX(c, r, 0, 2) / distance;
				accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 3)
					+= 1 / distance;

				// accumulate into lower left neighboring pixel
				distance = pointDistance(floor(denormX + .5), floor(denormY + .5), xAt(r, c), yAt(r, c));
				
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 0)
					+= data().atX(c, r, 0, 0) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 1)
					+= data().atX(c, r, 0, 1) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 2)
					+= data().atX(c, r, 0, 2) / distance;
				accumulation.atX(clamp(denormX + .5, 0, width() - 1), clamp(denormY + .5, 0, height() - 1), 0, 3)
					+= 1 / distance;
			}
		}

		// divide each pixel by accumulated weight and store result in output image
		for(int r = 0; r < height(); ++r)
		{
			for(int c = 0; c < width(); ++c)
			{
				// if not blank
				if(accumulation.atX(c, r, 0, 3) > 0.f)
				{
					// divide all 3 color channels by divisor
					// write to result image
					for(int i = 0; i < 3; ++i)
					{
						result.atX(c, r, 0, i)
							= clamp(accumulation.atX(c, r, 0, i) / accumulation.atX(c, r, 0, 3) + .5, 0, 255);
					}
				}
			}
		}

		return result;
	}
};

int main(int argc, char* argv[])
{
	// create an image object from file
	FlexibleImage<unsigned char> scene("scene.jpg");

	// create a display
	CImgDisplay before(DISP_X, DISP_Y, "Before");
	CImgDisplay after(DISP_X, DISP_Y, "After");
	CImgDisplay xDisp(DISP_X, DISP_Y, "X Coordinates", 2);
	CImgDisplay yDisp(DISP_X, DISP_Y, "Y Coordinates", 2);

	// set coordinate display scaling
	xDisp = scene.xData();
	yDisp = scene.yData();

	scene.moveCornersTo(.1f, .2f,
		.7f, .3f,
		0.f, 1.f,
		.9f, .9f);

	// show the image and its coordinate values
	before = scene.data();
	after = scene.rasterize();
	xDisp = scene.xData();
	yDisp = scene.yData();

	while(!(before.is_closed() || after.is_closed() || xDisp.is_closed() || yDisp.is_closed()));

	return 0;
}

// linear transforms
/*
give ea. pixel float coords [0.0, 1.0)
mul. coords by transformation matrix
linearly interpolate mem. pixel positions from float coords?
*/

// rectify image:
//		apply anti-distortion map
//		apply perspective transform