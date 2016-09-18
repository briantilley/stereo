#ifndef IMAGE_H
#define IMAGE_H

// define a standard container for image data and simple operations
// allow 24 bit RGB, 8 bit grayscale, and floating point grayscale

// data type (unsigned char or float) and color flag (valid only for unsigned char)
template<typename T, bool color>
class Image
{
private:
	T* m_data = nullptr;
	unsigned m_width = 0;
	unsigned m_height = 0;

	// inline utility to spit out a 1D index given coordinates
	unsigned indexOf(unsigned x, unsigned y) { return (y * width() + x) * (color ? 3 : 1); }

public:
	Image(T*, unsigned, unsigned); // create an image from data
	Image(); // create an empty image
	~Image();

	// operations
	Image<T, color> downsample(unsigned);
	Image<T, color> convolve(float*, unsigned);

	// status
	bool empty(void) { return static_cast<bool>(width() | height()); }
	unsigned width() const { return m_width; }
	unsigned height() const { return m_height; }
};

template<typename T, bool color>
Image<T, color>::Image(T* _data, unsigned _width, unsigned _height): m_width(_width), m_height(_height)
{
	// absolute size of image in bytes
	unsigned imageSize = sizeof(T) * width() * height() * (color ? 3 : 1);

	// restrict color images to 24 bit type
	if(color && is_same<T, float>::value)
	{
		cerr << "cannot create color floating point image" << endl;
		return;
	}

	// allocate space to copy data
	try { m_data = new T[width() * height() * (color ? 3 : 1)]; }
	catch(exception e)
	{
		cerr << "failed to allocate space to copy image" << endl;
		width = height = 0; // leave dimensions indicating "empty"
		return;
	}

	// copy image
	memcpy(m_data, _data, imageSize);
}

template<typename T, bool color>
Image<T, color>::~Image()
{
	// clean up memory
	delete [] m_data;
}

// return a downsampled Image with dimensions width/factor, height/factor\
// integer factor downsampling is simpler, faster, and easier
template<typename T, bool color>
Image<T, color> Image<T, color>::downsample(unsigned factor)
{
	// initialize result instance
	Image<T, color> result();
	result.width = width() / factor;
	result.height = height() / factor;

	// compute size and allocate new space for new image
	unsigned newImageSize = sizeof(T) * (color ? 3 : 1) * width() * height() / (factor * factor);
	try { result.m_data = new T[width() * height() * (color ? 3 : 1)]; }
	catch(exception e)
	{
		cerr << "failed to allocate space for output image";
		result.m_width = result.m_height = 0;
		return result;
	}

	// copy strided pixels
	for(int r = 0; r < result.height; ++r)
	{
		for(int c = 0; c < result.width; ++c)
		{
			result.m_data[indexOf(c, r)] = m_data[indexOf(c * factor, r * factor)];

			// copy green and blue channels of RGB image
			if(color)
			{
				result.m_data[indexOf(c, r) + 1] = m_data[indexOf(c * factor, r * factor) + 1];
				result.m_data[indexOf(c, r) + 2] = m_data[indexOf(c * factor, r * factor) + 2];
			}
		}
	}

	return result;
}

#endif