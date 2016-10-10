#include <iostream>
#include <cstring>

#include "Image.h"

// helper to compute indices
inline unsigned Image::linearIndex(unsigned column, unsigned row) const {
	return row * width() + column;
}

Image::Image(): m_values(nullptr), m_width(0), m_height(0), m_coords(nullptr) { }

Image::Image(unsigned width, unsigned height, const float* values, const float* coords): m_width(width), m_height(height) {
	// allocate space even for blank images
	m_values = new float[width * height];
	m_coords = new float[width * height * 2];

	// either copy or fill as blank
	if(values != nullptr)
		memcpy(m_values, values, sizeof(float) * width * height);
	else
		memset(m_values, 0, sizeof(float) * width * height);

	// either copy or fill with default coordinates matching image coordinates
	if(coords != nullptr)
		memcpy(m_coords, coords, sizeof(float) * width * height * 2);
	else {
		float xCoords[width];

		// compute first column and save xCoords for speed
		for(int col = 0; col < width; ++col) {
				// compute and store x coordinate
				float xCoord = static_cast<float>(col) / width;
				xCoords[col] = xCoord;

				m_coords[linearIndex(2 * col, 0)] = xCoord;
				m_coords[linearIndex(2 * col + 1, 0)] = 0.f;
		}

		// fill all subsequent rows using recall for x coords
		for(int row = 1; row < height; ++row) {
			float yCoord = static_cast<float>(row) / height;

			for(int col = 0; col < width; ++col) {
				float xCoord = static_cast<float>(col) / width;

				m_coords[linearIndex(2 * col, row)] = xCoords[col];
				m_coords[linearIndex(2 * col + 1, 0)] = yCoord;
			}
		}
	}
}

Image::Image(const Image& arg): Image(arg.width(), arg.height(), arg.m_values, arg.m_coords) { }

Image::~Image() {
	// free heap memory
	delete m_values;
	delete m_coords;
}

Pixel Image::at(unsigned col, unsigned row) const {
	return
	{
		m_values[linearIndex(col, row)],
		{ m_coords[linearIndex(col * 2, row)], m_coords[linearIndex(col * 2 + 1, row)] }
	};
}

Image Image::resample(unsigned width, unsigned height) const {
	Image result(width, height);


	return result;
}

void Image::shiftCorners(Coordinate upRight, Coordinate upLeft, Coordinate downRight, Coordinate downLeft) {

}

void Image::spatialScale() {

}

void Image::spatialNormalize() {

}

Image Image::rasterize() const {
	Image result(width(), height());


	return result;
}
