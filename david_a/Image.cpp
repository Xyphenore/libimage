#include "Image.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <array>
#include <vector>
#include <cmath>

extern "C" {
#include <jpeglib.h>
}


using alwaysData = std::runtime_error;
using invalidValue = std::runtime_error;

using badValueOfGrayPixel = std::runtime_error;
using badValueOfColorPixel = std::runtime_error;

using invalidType = std::invalid_argument;
using invalidWidth = std::invalid_argument;
using invalidHeight = std::invalid_argument;
using invalidIntensity = std::invalid_argument;
using invalidPosition = std::invalid_argument;

Color operator+( const Color& c1, const Color& c2 ) {
    return { static_cast<uint8_t>((static_cast<double>(c1.r_) + c2.r_)/2),
             static_cast<uint8_t>((static_cast<double>(c1.g_) + c2.g_)/2),
             static_cast<uint8_t>((static_cast<double>(c1.b_) + c2.b_)/2)
    };
}
Color operator*( const double alpha, const Color& c ) {
    return { static_cast<uint8_t>(std::round(c.r_ * alpha)),
             static_cast<uint8_t>(std::round(c.g_ * alpha)),
             static_cast<uint8_t>(std::round(c.b_ * alpha))
    };
}
Color operator*( const Color& c, const double alpha ) {
    return operator*(alpha, c);
}


static void skip_comments( std::istream &is ) {
    while ( '#' == std::ws(is).peek() ) {
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
static void skip_ONEwhitespace( std::istream& is ) {
    const std::array<char,6> whitespace({ '\t', '\n', '\v',
                                          '\f', '\r', ' '});

    if ( std::binary_search(whitespace.begin(), whitespace.end(), is.peek()) ) {
        is.get();
    }
}



static bool isEndOfLine( const uint16_t pos, const uint16_t width, const uint16_t formatLimit ) {
    return ( 0 == ( pos % width ) ) || ( 0 == ( pos % formatLimit ) );
}
static void noData( std::istream& is ) {
    // Actualisation du flux
    is.peek();

    if ( !(is.eof()) ) {
        throw alwaysData( "Input stream always contain data" );
    }
}

static void isGoodFormat( std::istream &is, const std::string &goodformat ) {
    std::string type;

    is >> type;

    if ( type != goodformat ) {
        throw invalidType( "Bad format of file" );
    }
}
static uint16_t isGoodWidth( std::istream &is, const uint16_t limit ) {
    int32_t width = 0;

    is >> width;

    if ( ( 0 >= width ) || ( limit < width ) ) {
        throw invalidWidth( "Bad width of Image" );
    }

    return static_cast<uint16_t>(width);
}
static uint16_t isGoodHeight( std::istream &is, const uint16_t limit ) {
    int32_t height = 0;

    is >> height;

    if ( ( 0 >= height ) || ( limit < height ) ) {
        throw invalidHeight( "Bad width of Image" );
    }

    return static_cast<uint16_t>(height);
}
static uint8_t isGoodIntensity( std::istream &is, const uint8_t limit ) {
    int16_t intensity = 0;

    is >> intensity;

    if ( ( 0 >= intensity ) || ( limit < intensity ) ) {
        throw invalidIntensity( "Bad intensity of Image" );
    }

    return static_cast<uint8_t>(intensity);
}
static void isGoodPosition( const uint16_t x, const uint16_t limit ) {
    if ( limit <= x ) {
        throw invalidPosition("The given position is invalid");
    }
}
static void isGoodGrayPixel( const uint8_t* const pixels, const uint8_t limit, const uint16_t length ) {
    for ( uint16_t i = 0; i < length; ++i ) {
        if ( pixels[i] > limit ) {
            throw badValueOfGrayPixel( "Bad value of gray pixel at positon " + i);
        }
    }
}
static void isGoodColorPixel( const Color* const pixels, const uint8_t limit, const uint16_t length ) {
    for ( uint16_t i = 0; i < length; ++i ) {
        if ( (pixels[i].r_ > limit) || (pixels[i].g_ > limit) || (pixels[i].b_ > limit) ) {
            throw badValueOfColorPixel( "Bad value of pixel at positon " + i);
        }
    }
}


static GrayImage* createGrayImage(const uint16_t width, const uint16_t height, const uint8_t intensity) {
    return new GrayImage(width, height, intensity);
}
static ColorImage* createColorImage( const uint16_t width, const uint16_t height, const uint8_t intensity) {
    return new ColorImage( width, height, intensity);
}



static void drawHorizontalColorLine( ColorImage& image, const uint16_t x, const uint16_t y, const uint16_t length, const Color color ) {
    for ( uint16_t  i = x; i < (x+length); ++i ) {
        image.pixel(i, y) = color;
    }
}
static void drawVerticalColorLine( ColorImage& image, const uint16_t x, const uint16_t y, const uint16_t length, const Color color ) {
    for ( uint16_t j = y; j < (y+length); ++j ) {
        image.pixel(x, j) = color;
    }
}


static uint8_t readGoodRawGrayValue( std::istream& is, const uint8_t limit ) {
    uint8_t value = std::numeric_limits<uint8_t>::max();

    is.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));

    if ( limit < value ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}
static uint8_t readGoodGrayValue( std::istream& is, const uint8_t limit ) {
    auto value = std::numeric_limits<uint8_t>::max();
    is >> value;

    if ( limit < value ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}
static Color readGoodColorValue( std::istream& is, const uint8_t limit ) {
    const uint8_t maxValue = std::numeric_limits<uint8_t>::max();
    Color value{maxValue,maxValue,maxValue};

    is >> value.r_ >> value.g_ >> value.b_;

    if ( (value.r_ > limit) || (value.g_ > limit) || (value.b_ > limit) ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}
static Color readGoodRawColorValue( std::istream& is, const uint8_t limit ) {
    const uint8_t valueMax = std::numeric_limits<uint8_t>::max();
    Color value(valueMax, valueMax, valueMax);

    is.read(reinterpret_cast<char*>(&value), sizeof(Color));

    if ( (value.r_ > limit) || (value.g_ > limit) || (value.b_ > limit) ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}

// Définition of GrayImage's methods

GrayImage::GrayImage(const uint16_t width, const uint16_t height)
: width_(width), height_(height), pixels_( width_ * height_) {}

GrayImage::GrayImage(const uint16_t width, const uint16_t height, const uint8_t intensity)
: width_(width), height_(height), intensity_(intensity), pixels_( width_ * height_) {}

GrayImage::GrayImage(const uint16_t width, const uint16_t height, const uint8_t intensity, std::vector<uint8_t>&& pixels)
: width_(width), height_(height), intensity_(intensity), pixels_(pixels) {}

uint8_t& GrayImage::pixel(const uint16_t x, const uint16_t y) {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels_.at( ( width_ * y) + x);
}
const uint8_t& GrayImage::pixel(const uint16_t x, const uint16_t y) const {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels_.at( ( width_ * y) + x);
}


void GrayImage::clear( const grayShade color ) {
    std::fill( pixels_.begin(), pixels_.end(), color);
}
void GrayImage::clear() { clear(defaultColor); }

void GrayImage::horizontalLine( const uint16_t x, const uint16_t y, const uint16_t length, const grayShade color ) {
    for ( uint16_t i = x; i < (x+length); ++i ) {
        pixel(i, y) = color;
    }
}
void GrayImage::verticalLine( const uint16_t x, const uint16_t y, const uint16_t length, const grayShade color ) {
    for ( uint16_t j = y; j < (y+length); ++j ) {
        pixel(x, j) = color;
    }
}

void GrayImage::rectangle( const uint16_t x, const uint16_t y,
                           const uint16_t width, const uint16_t height ) {
    rectangle(x,y,width,height,defaultColor);
}
void GrayImage::rectangle( const uint16_t x, const uint16_t y,
                           const uint16_t width, const uint16_t height,
                           const grayShade color) {
    horizontalLine( x, y, width, color );
    horizontalLine( x, (y-1)+height, width, color );

    verticalLine( x, y+1, height-2, color );
    verticalLine( (x-1)+width, y+1, height-2, color );
}
void GrayImage::fillRectangle( const uint16_t x, const uint16_t y,
                               const uint16_t width, const uint16_t height ) {
    fillRectangle(x,y,width,height, defaultColor);
}
void GrayImage::fillRectangle( const uint16_t x, const uint16_t y,
                               const uint16_t width, const uint16_t height,
                               const uint8_t color ) {
    for ( uint16_t i = y; i < (y+height); ++i ) {
        horizontalLine( x, i, width, color );
    }
}



// P2
/*
void GrayImage::writePGM( std::ostream &os ) const {
    ::isGoodGrayPixel(pixels_, intensity_, width_ * height_);

    const uint16_t pgm_limit_char = 70;

    os << "P2\n" << "# Image sauvegardée par " << ::identifier << '\n'
       << width_ << " " << height_ << '\n'
       << intensity_ << '\n';

    for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
        if ( ( 0 != i ) && ::isEndOfLine( i, width_, pgm_limit_char ) ) { os << '\n'; }

        os << pixels_[i] << " ";
    }

    os.flush();
}
*/

// P3
/*
void ColorImage::writePPM( std::ostream& os ) const {
    ::isGoodColorPixel(pixels_, intensity_, width_ * height_);

    const uint16_t pgm_limit_char = 70;

    os << "P3\n" << "# Image sauvegardée par " << ::identifier << '\n'
       << width_ << " " << height_ << '\n'
       << intensity_ << '\n';

    for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
        if ( ( 0 != i ) && ::isEndOfLine( i, width_, pgm_limit_char ) ) { os << '\n'; }

        os << pixels_[i].r_ << " "
           << pixels_[i].g_ << " "
           << pixels_[i].r_ << " ";
    }

    os.flush();
}
*/

// P5
void GrayImage::writePGM( std::ostream& os ) const {
    ::isGoodGrayPixel( pixels_.data(), intensity_, width_ * height_ );

    os << "P5\n" << "# Image sauvegardée par " << ::identifier << '\n'
       << width_ << " " << height_ << '\n'
       << static_cast<uint16_t>(intensity_) << '\n';

    os.write( reinterpret_cast<const char*>(pixels_.data()), static_cast<long>(width_ * height_) * sizeof(uint8_t));

    os << '\n' << std::flush;
}

// P6
void ColorImage::writePPM( std::ostream& os ) const {
    ::isGoodColorPixel( pixels, intensity_, width_ * height_ );

    os << "P6\n" << "# Image sauvegardée par " << ::identifier << '\n'
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
        image->pixels_[i] = ::readGoodGrayValue(is, intensity);

        //is >> image->pixels_[i];
    }

    ::noData( is );

    return image;
}
*/

// P3
/*
ColorImage* ColorImage::readPPM( std::istream& is ) {
    ::isGoodFormat( is, "P3" );

    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );

    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );

    ColorImage* image = ::createColorImage(width, height);

    for ( uint16_t i = 0; i < (width * height); ++i ) {
        image->pixels_[i] = ::readGoodColorValue(is, intensity);

        //is >> image->pixels_[i];
    }

    ::noData( is );

    return image;
}
*/
// P5
GrayImage *GrayImage::readPGM( std::istream &is ) {
    // NOTE : isGoodFormat( is, "PG") devra être renommé en CompareFormat
    ::isGoodFormat( is, "P5" );
    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    ::skip_comments( is );

    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );
    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );
    ::skip_ONEwhitespace(is );

    std::vector<uint8_t>pixels(width*height);

    // Expliquer la lecture
    is.read(reinterpret_cast<char*>(pixels.data()), static_cast<long>(width * height) * sizeof(uint8_t));

    ::isGoodGrayPixel(pixels.data(), intensity, width * height);

    GrayImage* const image = ::createGrayImage(width, height, intensity);

    std::swap(pixels, image->pixels_);

    ::skip_ONEwhitespace(is);

    ::noData(is);

    return image;
}

// P6
ColorImage* ColorImage::readPPM( std::istream& is ) {
    ::isGoodFormat( is, "P6" );

    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );

    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );

    ColorImage* const image = ::createColorImage(width, height, intensity);

    for ( uint32_t i = 0; i < (width * height); ++i ) {
        image->pixels[i] = ::readGoodRawColorValue( is, intensity );
    }

    return image;
}





ColorImage::ColorImage( const uint16_t width, const uint16_t height )
: width_(width), height_(height), pixels(new Color[width_ * height_]) {}

ColorImage::ColorImage( const uint16_t width, const uint16_t height, const uint8_t intensity )
: width_(width), height_(height),intensity_(intensity), pixels(new Color[width_ * height_]) {}

ColorImage::ColorImage(const ColorImage& src)
: width_(src.width_), height_(src.height_) {
    delete[] pixels;

    pixels = new Color[src.width_ * src.height_];


    width_ = src.width_;
    height_ = src.height_;

    for( std::size_t i = 0; i < (width_ * height_); ++i ) {
        pixels[i] = src.pixels[i];
    }
}

ColorImage::~ColorImage() {
    delete[] pixels;
}

Color& ColorImage::pixel(const uint16_t x, const uint16_t y) {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels[(width_ * y) + x];
}
const Color& ColorImage::pixel(const uint16_t x, const uint16_t y) const {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels[(width_ * y) + x];
}


void ColorImage::clear( const Color color ) {
    using Iterator = const std::iterator<std::forward_iterator_tag, Color>::pointer;
    Iterator first = pixels;
    Iterator end = pixels + (width_ * height_);

    std::fill(first, end, color);
}

void ColorImage::rectangle( const uint16_t x, const uint16_t y,
                           const uint16_t width, const uint16_t height,
                           const Color color) {
    ::drawHorizontalColorLine( *this, x, y, width, color );
    ::drawHorizontalColorLine( *this, x, y + height, width, color );

    ::drawVerticalColorLine( *this, x, y, height, color );
    ::drawVerticalColorLine( *this, x + width, y, height, color );
}
void ColorImage::fillRectangle( const uint16_t x, uint16_t y,
                               const uint16_t width, const uint16_t height,
                               const Color color ) {
    for ( ; y < (y+height); ++y ) {
        ::drawHorizontalColorLine( *this, x, y, width, color );
    }
}



Color::Color( const uint8_t r, const uint8_t g, const uint8_t b )
: r_(r), g_(g), b_(b) {}


GrayImage* GrayImage::simpleScale( const uint16_t newWidth, const uint16_t newHeight ) const {
    auto* const image = ::createGrayImage(newWidth, newHeight, intensity_);

    const auto ratioW = static_cast<double>(width_) / newWidth;
    const auto ratioH = static_cast<double>(height_) / newHeight;

    for ( uint16_t y = 0; y < newHeight; ++y ) {
        for ( uint16_t x = 0; x < newWidth; ++x ) {
            image->pixel(x,y) = pixel(static_cast<uint16_t>(x * ratioW),
                                      static_cast<uint16_t>(y * ratioH)
                                      );
        }
    }

    return image;
}

ColorImage* ColorImage::simpleScale( const uint16_t width, const uint16_t height ) const {
    auto image = new ColorImage( width, height );

    for ( uint16_t y = 0; y < height; ++y ) {
        for ( uint16_t x = 0; x < width; ++x ) {
            image->pixel(x,y) = pixel(static_cast<uint16_t>(x * (width_ / width)),
                                      static_cast<uint16_t>(y * (height_ / height))
            );
        }
    }

    return image;
}

GrayImage* GrayImage::bilinearScale( const uint16_t newWidth, const uint16_t newHeight ) const {
    auto* const image = ::createGrayImage( newWidth, newHeight, intensity_ );

    const auto ratioW = static_cast<double>(width_) / newWidth;
    const auto ratioH = static_cast<double>(height_) / newHeight;

    for ( uint16_t yp = 0; yp < newHeight; ++yp) {
        const auto y = ratioH * yp;
        const auto y1 = std::floor(y);
        const auto y2 = ( std::ceil(y) < height_ ? std::ceil(y) : (height_ - 1) );

        const auto ratioy = ( (y2 - y1) != 0 ? (y - y1)/(y2 - y1) : 0.0 );

        for ( uint16_t xp = 0; xp < newWidth; ++xp ) {
            const auto x = ratioW * xp;
            const auto x1 = std::floor(x);
            const auto x2 = ( std::ceil(x) < width_ ? std::ceil(x) : (width_ - 1) );

            const auto ratiox = ( (x2 - x1) != 0 ? (x - x1)/(x2 - x1) : 0.0 );


            const uint8_t p1 = pixel(static_cast<uint16_t>(x1), static_cast<uint16_t>(y1));
            const uint8_t p2 = pixel(static_cast<uint16_t>(x1), static_cast<uint16_t>(y2));
            const uint8_t p3 = pixel(static_cast<uint16_t>(x2), static_cast<uint16_t>(y1));
            const uint8_t p4 = pixel(static_cast<uint16_t>(x2), static_cast<uint16_t>(y2));


	        image->pixel(xp, yp) = static_cast<uint8_t>(std::round(
            ((1 - ratioy) * (( (1 - ratiox) * p1 ) + ( ratiox * p3 )))
                + (ratioy * (( (1 - ratiox) * p2 ) + ( ratiox * p4 )))
                ));

        }
	}

    return image;
}
