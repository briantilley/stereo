#ifndef IMAGE_H
#define IMAGE_H

struct Coordinate {
	float x, y;
};

struct Pixel {
	float value;
	Coordinate pos;
};

// manage only float values and give each pixel its own coordinates
class Image {
private:
	float* values; // grayscale values from 0.0 to 1.0
	const unsigned width, height; // image coordinates
	float* coords; // pixel positions (interleaved as xyxyxyxyxy)

	// make accesses easier
	unsigned linearIndex(unsigned column, unsigned row);

public:
	Image();
	Image(unsigned width, unsigned height, float* values = nullptr, float* coords = nullptr);
	Image(const Image& arg);

	// make sure to delete dynamic memory
	~Image();

	// all data of a pixel at a certain image coordinate
	Coordinate at(unsigned column, unsigned row);

	Image resample(unsigned width, unsigned height);
	void shiftCorners(Coordinate upRight, Coordinate upLeft,
		Coordinate downRight, Coordinate downLeft);
	void spatialScale();
	void spatialNormalize();
	Image rasterize();
};

#endif