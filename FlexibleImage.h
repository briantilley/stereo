#ifndef FLEXIBLE_IMAGE_H
#define FLEXIBLE_IMAGE_H

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
				yAt(r, c) = ((yFinalSlope - yInitSlope) * y_base + yInitSlope) * x_base
					+ (yFinalIntercept - yInitIntercept) * y_base + yInitIntercept;
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
				double distance = 0.0;
				double denormX = xAt(r, c) * width() + .5; // initial run subtracts .5
				double denormY = yAt(r, c) * height();

				for(int pixel = 0; pixel < 4; ++pixel)
				{
					// adding and subtracting .5 selects
					// left, right, up, down pixels
					if(pixel % 2)
						denormX += .5;
					else
						denormX -= .5;

					if(pixel == 2)
						denormY += .5;
					
					// distance from shifted point to grid-aligned point
					distance = pointDistance(floor(denormX), floor(denormY), xAt(r, c), yAt(r, c));

					// actual accumulation happens
					for(int color = 0; color < 3; ++color)
					{
						accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, color)
							+= data().atX(c, r, 0, color) / distance;
					}

					// keep track of total weight put into pixel
					accumulation.atX(clamp(denormX, 0, width() - 1), clamp(denormY, 0, height() - 1), 0, 3)
					+= 1 / distance;
				}
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

#endif