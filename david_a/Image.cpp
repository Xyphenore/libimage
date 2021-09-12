#include "Image.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <limits>
#include <algorithm>


static void skip_comments( std::istream &is ) {
    while ( '#' == is.peek() ) {
        is.ignore();
    }
}


static bool isEndOfLine( const uint16_t pos, const uint16_t width, const uint16_t formatLimit ) {
    return ( 0 == ( pos % width ) ) || ( 0 == ( pos % formatLimit ) );
}

static void isGoodFormat( std::istream &is, const std::string &goodformat ) {
    using invalidType = std::invalid_argument;

    std::string type;

    is >> type;

    if ( type != goodformat ) {
        throw invalidType( "Bad format of file" );
    }
}

static uint16_t isGoodWidth( std::istream &is, const uint16_t limit ) {
    using invalidWidth = std::invalid_argument;

    int32_t width = 0;

    is >> width;

    if ( ( 0 >= width ) || ( limit < width ) ) {
        throw invalidWidth( "Bad width of Image" );
    }

    return static_cast<uint16_t>(width);
}

static uint16_t isGoodHeight( std::istream &is, const uint16_t limit ) {
    using invalidHeight = std::invalid_argument;

    int32_t height = 0;

    is >> height;

    if ( ( 0 >= height ) || ( limit < height ) ) {
        throw invalidHeight( "Bad width of Image" );
    }

    return static_cast<uint16_t>(height);
}

static uint8_t isGoodIntensity( std::istream &is, const uint8_t limit ) {
    using invalidIntensity = std::invalid_argument;

    int16_t intensity = 0;

    is >> intensity;

    if ( ( 0 >= intensity ) || ( limit < intensity ) ) {
        throw invalidIntensity( "Bad intensity of Image" );
    }

    return static_cast<uint8_t>(intensity);
}

static GrayImage* createGrayImage(const uint16_t width, const uint16_t height) {
    return new GrayImage(width, height);
}

static void isGoodPosition( const uint16_t x, const uint16_t limit ) {
    using invalidPosition = std::invalid_argument;

    if ( limit <= x ) {
        throw invalidPosition("The given position is invalid");
    }
}

static void drawHorizontalLine( GrayImage& image, uint16_t x, const uint16_t y, const uint16_t length, const uint8_t color ) {
    for ( ; x < (x+length); ++x) {
        image.pixel(x, y) = color;
    }
}

static void drawVertcalLine( GrayImage& image, const uint16_t x, uint16_t y, const uint16_t length, const uint8_t color ) {
    for ( ; y < (y+length); ++y) {
	    image.pixel(x, y) = color;
    }
}

static void noData( std::istream& is ) {
    using alwaysData = std::runtime_error;

    if ( !(is.eof()) ) {
        throw alwaysData("Input stream always contain data");
    }
}

using invalidValue = std::runtime_error;

static uint8_t readGoodRawValue( std::istream& is, const uint8_t limit ) {
    auto const value = new uint8_t[1];
    *value = std::numeric_limits<uint8_t>::max();

    is.read(reinterpret_cast<char*>(value), sizeof(uint8_t));

    if ( limit > *value ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return *value;
}

static uint8_t readGoodValue( std::istream& is, const uint8_t limit ) {
    auto value = std::numeric_limits<uint8_t>::max();
    is >> value;

    if ( limit < value ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}

static void isGoodGrayShade( const uint8_t* pixels, const uint8_t limit, const uint16_t length ) {
    using badValueOfGrayShade = std::runtime_error;

    for ( uint16_t i = 0; i < length; ++i ) {
        if ( pixels[i] > limit ) {
            throw badValueOfGrayShade("Bad value of gray shade at positon " + i);
        }
    }
}



// Définition of GrayImage's methods

GrayImage::GrayImage(const uint16_t width, const uint16_t height)
: width_(width), height_(height) {
      pixels = new uint8_t[width * height];
}

GrayImage::GrayImage(const GrayImage& src)
{
    if ( (src.width_ != width_) || (src.height_ != height_) ) {
        delete[] pixels;

        pixels = new uint8_t[src.width_ * src.height_];
    }

    width_ = src.width_;
    height_ = src.height_;

    for( std::size_t i = 0; i < (width_ * height_); ++i ) {
        pixels[i] = src.pixels[i];
    }
}

GrayImage::~GrayImage() {
  delete[] pixels;
}


uint8_t& GrayImage::pixel(const uint16_t x, const uint16_t y) {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels[(height_ * y) + x];
}

const uint8_t& GrayImage::pixel(const uint16_t x, const uint16_t y) const {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels[(height_ * y) + x];
}


void GrayImage::clear( const uint8_t color ) {
    using Iterator = const std::iterator<std::forward_iterator_tag, uint8_t>::pointer;
    Iterator first = pixels;
    Iterator end = pixels + (width_ * height_);

    std::fill(first, end, color);
}

void GrayImage::rectangle( const uint16_t x, const uint16_t y,
                          const uint16_t width, const uint16_t height,
                          const uint8_t color) {
    ::drawHorizontalLine(*this, x, y, width, color);
    ::drawHorizontalLine(*this, x, y+height, width, color);

    ::drawVertcalLine( *this, x, y, height, color);
    ::drawVertcalLine( *this, x+width, y, height, color);
}

void GrayImage::fillRectangle( const uint16_t x, uint16_t y,
                               const uint16_t width, const uint16_t height,
                               const uint8_t color ) {
    for ( ; y < (y+height); ++y ) {
        ::drawHorizontalLine( *this, x, y, width, color);
    }
}



// P2
/*
void GrayImage::writePGM( std::ostream &os ) const {
    ::isGoodGrayShade(pixels, intensity_, width_ * height_);

    const uint16_t pgm_limit_char = 70;

    os << "P2\n" << "# Image sauvegardée par " << ::identifier << '\n'
       << width_ << " " << height_ << '\n'
       << intensity_ << '\n';

    for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
        if ( ( 0 != i ) && ::isEndOfLine( i, width_, pgm_limit_char ) ) { os << '\n'; }

        os << pixels[i] << " ";
    }

    os.flush();
}
*/

// P5
void GrayImage::writePGM( std::ostream& os ) const {
    ::isGoodGrayShade(pixels, intensity_, width_ * height_);

    os << "P5\n" << "# Image sauvegardée par " << ::identifier << '\n'
       << width_ << " " << height_ << '\n'
       << intensity_ << '\n';

    os.write(reinterpret_cast<const char*>(pixels), sizeof(pixels));

    os.flush();
}

// P2
/*
GrayImage* GrayImage::readPGM( std::istream& is ) {
    ::isGoodFormat( is, "P2" );

    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );

    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );

    GrayImage* image = ::createGrayImage(width, height);

    for ( uint16_t i = 0; i < (width * height); ++i ) {
        image->pixels[i] = ::readGoodValue(is, intensity);

        //is >> image->pixels[i];
    }

    ::noData( is );

    return image;
}
*/

// P5
GrayImage *GrayImage::readPGM( std::istream &is ) {
    ::isGoodFormat( is, "P5" );

    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );

    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );

    GrayImage* image = ::createGrayImage(width, height);

    for ( uint16_t i = 0; i < (width * height); ++i ) {
        image->pixels[i] = ::readGoodRawValue(is, intensity);
    }

    //is.read( reinterpret_cast<char*>(image->pixels), sizeof(pixels));

    ::noData(is);

    return image;
}
