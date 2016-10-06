#include <iostream>
#include <cstring>

#include "Image.h"

// helper to compute indices
inline unsigned Image::linearIndex(unsigned column, unsigned row) {
	return row * this->width + column;
}

Image::Image(): values(nullptr), width(0), height(0), coords(nullptr) { }

Image::Image(unsigned width, unsigned height, float* values, float* coords): width(width), height(height) {
	this->values = new float[width * height];
	this->coords = new float[width * height * 2];

	// either copy or fill as blank
	if(values != nullptr)
		memcpy(this->values, values, sizeof(float) * width * height);
	else
		memset(this->values, 0, sizeof(float) * width * height);

	// either copy or fill with default coordinates matching image coordinates
	if(coords != nullptr)
		memcpy(this->coords, coords, sizeof(float) * width * height * 2);
	else {
		float xCoords[width];

		// compute first column and save xCoords for speed
		for(int col = 0; col < width; ++col) {
				// compute and store x coordinate
				float xCoord = static_cast<float>(col) / width;
				xCoords[col] = xCoord;

				this->coords[linearIndex(2 * col, 0)] = xCoord;
				this->coords[linearIndex(2 * col + 1, 0)] = 0.f;
		}

		// fill all subsequent rows using recall for x coords
		for(int row = 1; row < height; ++row) {
			float yCoord = static_cast<float>(row) / height;

			for(int col = 0; col < width; ++col) {
				float xCoord = static_cast<float>(col) / width;

				this->coords[linearIndex(2 * col, row)] = xCoords[col];
				this->coords[linearIndex(2 * col + 1, 0)] = yCoord;
			}
		}
	}
}

Image::Image(const Image& arg) {

}

Image::~Image() {
	// free heap memory
	delete this.values;
	delete this.coords;
}