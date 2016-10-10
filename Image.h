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
	float* m_values; // grayscale values from 0.0 to 1.0
	const unsigned m_width, m_height; // image coordinates
	float* m_coords; // pixel positions (interleaved as xyxyxyxyxy)

	// make accesses easier
	unsigned linearIndex(unsigned column, unsigned row) const;

public:
	Image();
	Image(unsigned width, unsigned height, const float* values = nullptr, const float* coords = nullptr);
	Image(const Image& arg);

	// make sure to delete dynamic memory
	~Image();

	// all data of a pixel at a certain image coordinate
	Pixel at(unsigned column, unsigned row) const;
	unsigned width() const { return m_width; }
	unsigned height() const { return m_height; }

	Image resample(unsigned width, unsigned height) const;
	void shiftCorners(Coordinate upRight, Coordinate upLeft,
		Coordinate downRight, Coordinate downLeft);
	void spatialScale();
	void spatialNormalize();
	Image rasterize() const;
};

#endif