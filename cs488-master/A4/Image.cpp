// Winter 2020

#include "Image.hpp"

#include <iostream>
#include <cstring>
#include <lodepng/lodepng.h>

using namespace std;

const uint Image::m_colorComponents = 3; // Red, blue, green

//---------------------------------------------------------------------------------------
Image::Image()
  : m_width(0),
    m_height(0),
    m_data(0)
{
}

//---------------------------------------------------------------------------------------
Image::Image(
		uint width,
		uint height
)
  : m_width(width),
    m_height(height)
{
	size_t numElements = m_width * m_height * m_colorComponents;
	m_data = new double[numElements];
	memset(m_data, 0, numElements*sizeof(double));
}

//---------------------------------------------------------------------------------------
Image::Image(const Image & other)
  : m_width(other.m_width),
    m_height(other.m_height),
    m_data(other.m_data ? new double[m_width * m_height * m_colorComponents] : 0)
{
  if (m_data) {
    std::memcpy(m_data, other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double));
  }
}

//---------------------------------------------------------------------------------------
Image::~Image()
{
  delete [] m_data;
}

//---------------------------------------------------------------------------------------
Image & Image::operator=(const Image& other)
{
  delete [] m_data;

  m_width = other.m_width;
  m_height = other.m_height;
  m_data = (other.m_data ? new double[m_width * m_height * m_colorComponents] : 0);

  if (m_data) {
    std::memcpy(m_data,
                other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double)
    );
  }

  return *this;
}

//---------------------------------------------------------------------------------------
uint Image::width() const
{
  return m_width;
}

//---------------------------------------------------------------------------------------
uint Image::height() const
{
  return m_height;
}

double Image::operator()(uint x, uint y, uint i) const {
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

double & Image::operator()(uint x, uint y, uint i) {
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

float Image::divergence(uint x, uint y) {
  float ret = 0.0;
  for (int component = 0; component < 3; component++) {
    float sum = 0.0;
    float sum_sqr = 0.0;
    for (int dx=-1; dx<=1; dx++) {
        for (int dy=-1; dy<=1; dy++) {
            float val = (*this)(x+dx, y+dy, component);
            sum += val;
            sum_sqr += val*val;
        }
    }
    float expectation_of_x = sum / 9;
    float expectation_of_xx = sum_sqr / 9;
    ret += expectation_of_xx - expectation_of_x*expectation_of_x;
  }
  return ret/3;
}

//---------------------------------------------------------------------------------------
static double clamp(double x, double a, double b)
{
	return x < a ? a : (x > b ? b : x);
}

//---------------------------------------------------------------------------------------
bool Image::savePng(const std::string & filename) const {
	std::vector<unsigned char> image;

	image.resize(m_width * m_height * m_colorComponents);

	double color;
	for (uint y(0); y < m_height; y++) {
		for (uint x(0); x < m_width; x++) {
			for (uint i(0); i < m_colorComponents; ++i) {
				color = m_data[m_colorComponents * (m_width * y + x) + i];
				color = clamp(color, 0.0, 1.0);
				image[m_colorComponents * (m_width * y + x) + i] = (unsigned char)(255 * color);
			}
		}
	}

	// Encode the image
	unsigned error = lodepng::encode(filename, image, m_width, m_height, LCT_RGB);

	if(error) {
		std::cerr << "encoder error " << error << ": " << lodepng_error_text(error)
				<< std::endl;
	}

	return true;
}

//---------------------------------------------------------------------------------------
const double * Image::data() const
{
  return m_data;
}

//---------------------------------------------------------------------------------------
double * Image::data()
{
  return m_data;
}
