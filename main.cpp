#include <iostream>
#include <string>
#include "CImg.h"

// use libjpeg rather than ImageMagick
#define cimg_use_jpeg

using namespace std;
using namespace cimg_library;

// rectify image:
//		apply anti-distortion map
//		apply perspective transform

// class ScalableImage
// {
// private:
// 	CImg<unsigned char> img;
// 	CImgList<float> coords;

// public:
// 	void ScalableImage(string filepath)
// 	{
// 		img = CImg<unsigned char>(filename.c_str());
// 		coords = CI
// 	}
// };

// uses a 2x2 matrix to transform pixel coords
void linearTransform(CImgList<float> coords, float matrix[])
{

}

int main(int argc, char* argv[])
{
	// create an image object from file
	// ScalableImage ubuntu
	CImg<unsigned char> ubuntu("scene.jpg");

	// display the image
	CImgDisplay disp(ubuntu);

	while(!disp.is_closed());

	return 0;
}

// linear transforms
/*
give ea. pixel float coords [0.0, 1.0)
mul. coords by transformation matrix
linearly interpolate mem. pixel positions from float coords?
*/
