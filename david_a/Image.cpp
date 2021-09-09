#include "Image.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <limits>


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

    return width;
}

static uint16_t isGoodHeight( std::istream &is, const uint16_t limit ) {
    using invalidHeight = std::invalid_argument;

    int32_t height = 0;

    is >> height;

    if ( ( 0 >= height ) || ( limit < height ) ) {
        throw invalidHeight( "Bad width of Image" );
    }

    return height;
}

static uint8_t isGoodIntensity( std::istream &is, const uint8_t limit ) {
    using invalidIntensity = std::invalid_argument;

    int16_t intensity = 0;

    is >> intensity;

    if ( ( 0 >= intensity ) || ( limit < intensity ) ) {
        throw invalidIntensity( "Bad intensity of Image" );
    }

    return intensity;
}

static GrayImage* createGrayImage(const uint16_t width, const uint16_t height) {
    return new GrayImage(width, height);
}

void GrayImage::writePGM( std::ostream &os ) const {
    const uint16_t pgm_limit_char = 70;

    os << "P2\n" << "# Image sauvegardée par " << ::identifier << '\n' << width_ << " " << height_ << " " << intensity
       << '\n';

    for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
        if ( ( 0 != i ) && ::isEndOfLine( i, width_, pgm_limit_char ) ) { os << '\n'; }

        os << pixels[i];
    }

    os.flush();
}

GrayImage *GrayImage::readPGM( std::istream &is ) {
    ::isGoodFormat( is, "P5" );

    ::skip_comments( is );

    const auto width = ::isGoodWidth( is, std::numeric_limits<uint16_t>::max() );
    const auto height = ::isGoodHeight( is, std::numeric_limits<uint16_t>::max() );

    ::skip_comments( is );

    const auto intensity = ::isGoodIntensity( is, std::numeric_limits<uint8_t>::max() );

    GrayImage* image = ::createGrayImage( width, height);

    // A corriger
    is.read( reinterpret_cast<char*>(image->pixels), image->getWidth() * image->getHeight());

    return image;
}
