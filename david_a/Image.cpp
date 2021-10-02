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
#include <sstream>

extern "C" {
#include <jpeglib.h>
}


using alwaysData = std::runtime_error;
using invalidValue = std::runtime_error;

using badValueOfGrayPixel = std::runtime_error;
using badValueOfColorPixel = std::runtime_error;
using badValuePixel = std::runtime_error;

using invalidType = std::invalid_argument;
using invalidWidth = std::invalid_argument;
using invalidHeight = std::invalid_argument;
using invalidIntensity = std::invalid_argument;
using invalidPosition = std::invalid_argument;
using invalidArray = std::invalid_argument;
using invalidColor = std::invalid_argument;
using invalidFormat = std::invalid_argument;
using invalidLength = std::invalid_argument;

using WidthUnit = Width;
using HeightUnit = Height;

constexpr static auto maxWidth = std::numeric_limits<Width>::max();
constexpr static auto maxHeight = std::numeric_limits<Height>::max();
constexpr static auto maxGrayIntensity = std::numeric_limits<GrayShade>::max();


namespace imageUtils {
    constexpr static double mean( const double v1, const double v2 ) {
        return (v1 + v2) / 2;
    }

    /// Skip all comments up to a non-comment
    /// \param[in,out] input stream
    /// \exception THINK exception of input stream
    static void skip_comments( std::istream &is ) {
        // Detect the start of a comments
        while ( '#' == std::ws(is).peek() ) {
            // Ignore all characters up to \n
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /// Skip one whitespace of the input stream
    /// \param[in,out] input stream
    /// \exception THINK exception of input stream
    static void skip_ONEwhitespace( std::istream& is ) {
        constexpr std::array<char,6> whitespace({ '\t', '\n', '\v',
                                                  '\f', '\r', ' '});

        const auto isWhiteSpace = std::binary_search(whitespace.cbegin(), whitespace.cend(), is.peek());

        if ( isWhiteSpace ) {
            is.get();
        }
    }

    static void verifyStreamContainData( std::istream& is ) {
        // Actualisation du flux
        is.peek();

        if ( !(is.eof()) ) {
            throw alwaysData( "Input stream always contain data" );
        }
    }

    template<typename Type>
    static void verifyOver0UnderOrEqualLimitOf( const std::intmax_t value, const Type limit ) {
        if ( ( 0 >= value ) || ( limit < value ) ) {
            std::ostringstream oss( "Value out ]0, ", std::ios::ate );
            oss << std::numeric_limits<std::intmax_t>::max() << "]";

            throw std::range_error( oss.str() );
        }
    }

    template<typename TWidth>
    static void verifyWidth( const std::intmax_t width, const TWidth limit ) {
        try {
            imageUtils::verifyOver0UnderOrEqualLimitOf<TWidth>( width, limit );
        }
        catch ( const std::range_error& ) {
            throw invalidWidth("Bad width of Image");
        }
    }
    template<typename TWidth>
    static TWidth readWidth( std::istream& is ) {
        std::intmax_t length = 0;
        is >> length;

        imageUtils::verifyWidth( length, std::numeric_limits<TWidth>::max() );

        return static_cast<TWidth>(length);
    }


    template<typename THeight>
    static void verifyHeight( const std::intmax_t height, const THeight limit ) {
        try {
            imageUtils::verifyOver0UnderOrEqualLimitOf<THeight>( height, limit );
        }
        catch ( const std::range_error& ) {
            throw invalidHeight("Bad height of Image");
        }
    }
    template<typename THeight>
    static THeight readHeight( std::istream& is ) {
        std::intmax_t height = 0;
        is >> height;

        imageUtils::verifyHeight( height, std::numeric_limits<THeight>::max() );

        return static_cast<THeight>(height);
    }

    template<typename TIntensity>
    static void verifyIntensity( const std::intmax_t intensity, const TIntensity limit ) {
        try {
            imageUtils::verifyOver0UnderOrEqualLimitOf<TIntensity>( intensity, limit );
        }
        catch ( const std::range_error& ) {
            throw invalidIntensity( "Bad intensity of Image" );
        }
    }
    template<typename TIntensity>
    static TIntensity readIntensity( std::istream& is ) {
        std::intmax_t intensity = 0;
        is >> intensity;

        imageUtils::verifyIntensity( intensity, std::numeric_limits<TIntensity>::max() );

        return static_cast<TIntensity>(intensity);
    }

    template<typename TLength>
    static void verifyLength( const std::intmax_t length, const TLength limit ) {
        try {
            imageUtils::verifyOver0UnderOrEqualLimitOf<TLength>( length, limit );
        }
        catch ( const std::range_error& ) {
            throw invalidLength("Bad length");
        }
    }

    template<typename Type>
    static void verifyOverEqual0UnderEqualLimitOf( const std::intmax_t value, const Type limit ) {
        if ( ( 0 > value ) || ( limit < value ) ) {
            std::ostringstream oss( "Value out [0, ", std::ios::ate );
            oss << std::numeric_limits<std::intmax_t>::max() << "]";

            throw std::range_error( oss.str() );
        }
    }
    template<typename TColor>
    static void verifyColor( const std::intmax_t color, const TColor limit ) {
        try {
            imageUtils::verifyOverEqual0UnderEqualLimitOf<TColor>( color, limit );
        }
        catch ( const std::range_error& ) {
            throw invalidColor( "Bad Color" );
        }
    }


    template<typename TPixel>
    static void verifyPixel( const std::vector<TPixel>& pixels, const TPixel limit ) {
        if ( limit < std::numeric_limits<TPixel>::max() ) {
            const auto overLimit = [limit]( const TPixel pixel ) { return pixel > limit; };

            const auto pos = std::find_if( pixels.cbegin(), pixels.cend(), overLimit );

            if ( pos != pixels.cend() ) {
                std::ostringstream oss( "Bad pixel at ", std::ios::ate );
                oss << static_cast<std::intmax_t>( pos - pixels.cbegin() );
                throw badValuePixel( oss.str() );
            }
        }
    }

    template<typename TPixel>
    static void verifySizeArray( const std::vector<TPixel>& pixels, const std::size_t maxSize ) {
        if ( pixels.size() != maxSize ) {
            throw invalidArray("The given array does not match with the size of Image");
        }
    }

    template<typename TPosition>
    static void verifyPosition( const std::intmax_t x, const TPosition limit ) {
        if ( (x < 0) || (limit <= x) ) {
            throw invalidPosition("The given position is invalid");
        }
    }
}



Color operator+( const Color& c1, const Color& c2 ) {
    return { static_cast<uint8_t>( std::round( imageUtils::mean( c1.r_, c2.r_)) ),
             static_cast<uint8_t>( std::round( imageUtils::mean( c1.g_, c2.g_)) ),
             static_cast<uint8_t>( std::round( imageUtils::mean( c1.b_, c2.b_)) )
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
bool operator<( const Color& c1, const Color& c2 ) {
    return (c1.r_ + c1.g_ + c1.b_) < (c2.r_ + c2.g_ + c2.b_);
}
bool operator>( const Color& c1, const Color& c2 ) {
    return (c1.r_ + c1.g_ + c1.b_) > (c2.r_ + c2.g_ + c2.b_);
}

// THINK Déplacer les méthodes utilitaires dans un espace de noms dédiées
static bool isEndOfLine( const uint16_t pos, const uint16_t width, const uint16_t formatLimit ) {
    return ( 0 == ( pos % width ) ) || ( 0 == ( pos % formatLimit ) );
}


// Modify
static void isGoodFormat( std::istream &is, const std::string &goodformat ) {
    std::string type;

    is >> type;

    if ( type != goodformat ) {
        throw invalidType( "Bad format of file" );
    }
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
            std::ostringstream oss("Bad value of gray pixel at positon ");
            oss << i;
            throw badValueOfGrayPixel( oss.str() );
        }
    }
}
static void isGoodColorPixel( const Color* const pixels, const uint8_t limit, const uint16_t length ) {
    for ( uint16_t i = 0; i < length; ++i ) {
        if ( (pixels[i].r_ > limit) || (pixels[i].g_ > limit) || (pixels[i].b_ > limit) ) {
            std::ostringstream oss("Bad value of pixel at positon ");
            oss << i;
            throw badValueOfColorPixel( oss.str());
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
    const uint8_t maxValue = std::numeric_limits<uint8_t>::max();
    Color value{maxValue,maxValue,maxValue};

    is.read(reinterpret_cast<char*>(&value), sizeof(Color));

    if ( (value.r_ > limit) || (value.g_ > limit) || (value.b_ > limit) ) {
        throw invalidValue("Redden value was over the given intensity");
    }

    return value;
}

// Definition of GrayImage's methods

// Public builders
GrayImage::GrayImage( const std::intmax_t width, const std::intmax_t height )
: GrayImage( width, height, defaultIntensity) {}

GrayImage::GrayImage( const std::intmax_t width, const std::intmax_t height, const std::intmax_t intensity )
: width_( static_cast<Width>(width) ), height_( static_cast<Height>(height) ),
  intensity_( static_cast<GrayShade>(intensity) ), pixels_( width_ * height_ ) {
    // Verify all preconditions
    imageUtils::verifyWidth( width, ::maxWidth );
    imageUtils::verifyHeight( height, ::maxHeight );
    imageUtils::verifyIntensity( intensity, ::maxGrayIntensity );

    // Fill the image with the default Color
    fill( defaultColor);
}


// Private builders
GrayImage::GrayImage( const std::intmax_t width, const std::intmax_t height, const std::intmax_t intensity,
                      const std::vector<GrayShade>& pixels )
: width_( static_cast<Width>(width) ), height_( static_cast<Height>(height) ),
  intensity_( static_cast<GrayShade>(intensity) ), pixels_( pixels) {
    // verify all preconditions
    imageUtils::verifyWidth( width, ::maxWidth );
    imageUtils::verifyHeight( height, ::maxHeight );
    imageUtils::verifySizeArray( pixels_, width_ * height_ );
    imageUtils::verifyIntensity( intensity, ::maxGrayIntensity );
    imageUtils::verifyPixel( pixels_, intensity_ );
}

GrayImage::GrayImage( const std::intmax_t width, const std::intmax_t height, const std::intmax_t intensity,
                      std::vector<GrayShade>&& pixels )
: width_( static_cast<Width>(width) ), height_( static_cast<Height>(height) ),
  intensity_( static_cast<GrayShade>(intensity) ), pixels_( std::move( pixels ) ) {
    // Verify all preconditions
    imageUtils::verifyWidth( width, ::maxWidth );
    imageUtils::verifyHeight( height, ::maxHeight );
    imageUtils::verifySizeArray( pixels_, width_ * height_ );
    imageUtils::verifyIntensity( intensity, ::maxGrayIntensity );
    imageUtils::verifyPixel( pixels_, intensity_ );
}


GrayShade& GrayImage::pixel( const std::intmax_t x, const std::intmax_t y ) {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);

    return pixels_.at( static_cast<std::size_t>( (width_ * y) + x ) );
}
const GrayShade& GrayImage::pixel( const std::intmax_t x, const std::intmax_t y ) const {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);

    return pixels_.at( static_cast<std::size_t>( (width_ * y) + x ) );
}


void GrayImage::fill( const std::intmax_t color ) {
    imageUtils::verifyColor( color, intensity_ );

    const auto colorGrayShade = static_cast<GrayShade>( color );

    std::fill( pixels_.begin(), pixels_.end(), colorGrayShade );
}

void GrayImage::horizontalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length, const std::intmax_t color ) {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);
    imageUtils::verifyLength( length, width_ - x);
    imageUtils::verifyColor( color, intensity_);

    for ( std::intmax_t i = x; i < (x+length); ++i ) {
        pixel(i, y) = static_cast<GrayShade>(color);
    }
}
void GrayImage::verticalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length, const std::intmax_t color ) {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);
    imageUtils::verifyLength( length, height_ - y);
    imageUtils::verifyColor( color, intensity_);

    for ( std::intmax_t j = y; j < (y+length); ++j ) {
        pixel(x, j) = static_cast<GrayShade>(color);
    }
}

void GrayImage::rectangle( const std::intmax_t x, const std::intmax_t y, const std::intmax_t width, const std::intmax_t height,
                           const std::intmax_t color) {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);
    imageUtils::verifyWidth( width, width_ - x);
    imageUtils::verifyHeight( height, height_ - y);
    imageUtils::verifyColor( color, intensity_);

    horizontalLine( x, y, width, color );
    horizontalLine( x, (y-1)+height, width, color );

    verticalLine( x, y+1, height-2, color );
    verticalLine( (x-1)+width, y+1, height-2, color );
}
void GrayImage::fillRectangle( const std::intmax_t x, const std::intmax_t y, const std::intmax_t width, const std::intmax_t height,
                               const std::intmax_t color ) {
    imageUtils::verifyPosition( x, width_);
    imageUtils::verifyPosition( y, height_);
    imageUtils::verifyWidth( width, width_ - x);
    imageUtils::verifyHeight( height, height_ - y);
    imageUtils::verifyColor( color, intensity_);

    for ( std::intmax_t i = y; i < (y + height); ++i ) {
        horizontalLine( x, i, width, color );
    }
}




void GrayImage::writePGM( std::ostream& os, const Format f ) const {
    if ( (f != Format::ASCII) && (f != Format::BINARY) ) {
        throw invalidFormat( "Unknown image format");
    }

    imageUtils::verifyPixel<GrayShade>( pixels_, intensity_ );

    if ( f == Format::BINARY ) {
        os << "P5\n" << "# Image sauvegardée par " << ::identifier << '\n'
           << width_ << " " << height_ << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';


        os.write( reinterpret_cast<const char*>(pixels_.data()),
                  static_cast<std::streamsize>(width_ * height_ * sizeof( GrayShade )) );
    }
    else {
        // Full ASCII format
        constexpr uint16_t pgm_limit_char = 70;

        os << "P2\n" << "# Image sauvegardée par " << ::identifier << '\n'
           << width_ << " " << height_ << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';


        for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
            if ( ( 0 != i ) && ::isEndOfLine( i+1, width_, pgm_limit_char ) ) { os << '\n'; }

            os << pixels_[i] << " ";
        }
    }

    os << '\n' << std::flush;
}

void ColorImage::writePPM( std::ostream& os, const Format f ) const {
    if ( (f != Format::ASCII) && (f != Format::BINARY) ) {
        throw invalidFormat( "Unknown image format");
    }

    ::isGoodColorPixel( pixels_.data(), intensity_, width_ * height_ );
    imageUtils::verifyPixel<Color>( pixels_, Color{ intensity_, intensity_, intensity_});

    if ( f == Format::BINARY ) {
        os << "P6\n" << "# Image sauvegardée par " << ::identifier << '\n'
           << width_ << " " << height_ << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';

        os.write( reinterpret_cast<const char*>(pixels_.data()),
                  static_cast<std::streamsize>(width_ * height_ * sizeof( Color )) );
    }
    else {
        // Full ASCII format
        constexpr uint16_t pgm_limit_char = 70;

        os << "P3\n" << "# Image sauvegardée par " << ::identifier << '\n'
           << width_ << " " << height_ << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';

        for ( uint16_t i = 0; i < ( width_ * height_ ); ++i ) {
            if ( ( 0 != i ) && ::isEndOfLine( i+1, width_, pgm_limit_char ) ) { os << '\n'; }

            os << static_cast<uint16_t>(pixels_[i].r_) << " "
               << static_cast<uint16_t>(pixels_[i].g_) << " "
               << static_cast<uint16_t>(pixels_[i].b_) << " ";
        }
    }

    os << '\n' << std::flush;
}



GrayImage *GrayImage::readPGM( std::istream &is ) {
    std::string type;
    is >> type;

    if ( (type != "P5") && (type != "P2") ) {
        throw invalidType( "Bad format of file" );
    }

    imageUtils::skip_comments( is );

    const auto width = imageUtils::readWidth<Width>( is );
    imageUtils::skip_comments( is );

    const auto height = imageUtils::readHeight<Height>( is );
    imageUtils::skip_comments( is );

    const auto intensity = imageUtils::readIntensity<GrayShade>( is );
    imageUtils::skip_ONEwhitespace( is );

    std::vector<GrayShade> pixels( width * height );

    // Attraper l'exception EOF, badRead(bad bit), fail bit
    if ( type == "P5" ) {
        // Expliquer la lecture
        try {
            is.read( reinterpret_cast<char*>(pixels.data()),
                     static_cast<std::streamsize>(width * height * sizeof( GrayShade )) );
        } catch ( ... ) {
            std::cerr << "Error occured on stream";
        }
    }
    else {
        // P2 format
        // Utiliser from_chars()
        for ( std::size_t i = 0; i < (width * height); ++i ) {
            is >> pixels.at(i);
        }
    }

    imageUtils::verifyPixel( pixels, intensity );
    imageUtils::skip_ONEwhitespace( is );

    imageUtils::verifyStreamContainData( is );

    return new GrayImage( width, height, intensity, std::move(pixels) );
}


ColorImage* ColorImage::readPPM( std::istream& is ) {
    std::string type;
    is >> type;

    if ( (type != "P6") && (type != "P3") ) {
        throw invalidType( "Bad format of file" );
    }

    imageUtils::skip_comments( is );
    const auto width = imageUtils::readWidth<Width>( is );

    imageUtils::skip_comments( is );
    const auto height = imageUtils::readHeight<Height>( is );

    imageUtils::skip_comments( is );
    const auto intensity = imageUtils::readIntensity<GrayShade>( is );

    imageUtils::skip_ONEwhitespace( is );

    std::vector<Color> pixels( width * height );

    // Attraper l'exception EOF, badRead(bad bit), fail bit
    if ( type == "P6" ) {
        // Expliquer la lecture
        try {
            is.read( reinterpret_cast<char*>(pixels.data()),
                     static_cast<std::streamsize>(width * height * sizeof( Color )) );
        }
        catch ( ... ) {
            std::cerr << "Error occured on stream";
        }
    }
    else {
        // type P3
        for ( std::size_t i = 0; i < (width * height); ++i ) {
            is >> pixels.at(i).r_ >> pixels.at(i).g_ >> pixels.at(i).b_;
        }
    }

    imageUtils::verifyPixel( pixels, Color{intensity, intensity, intensity} );
    imageUtils::skip_ONEwhitespace( is );

    imageUtils::verifyStreamContainData( is );

    return new ColorImage( width, height, intensity, std::move(pixels));
}





ColorImage::ColorImage( const uint16_t width, const uint16_t height )
: width_(width), height_(height), pixels_(width_ * height_) {}

ColorImage::ColorImage( const uint16_t width, const uint16_t height, const uint8_t intensity )
: width_(width), height_(height),intensity_(intensity), pixels_(width_ * height_) {}


Color& ColorImage::pixel(const uint16_t x, const uint16_t y) {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels_[(width_ * y) + x];
}
const Color& ColorImage::pixel(const uint16_t x, const uint16_t y) const {
    ::isGoodPosition(x, getWidth());
    ::isGoodPosition(y, getHeight());

    return pixels_[(width_ * y) + x];
}


void ColorImage::clear( const Color color ) {
    std::fill(pixels_.begin(), pixels_.end(), color);
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


GrayImage* GrayImage::simpleScale( const std::intmax_t newWidth, const std::intmax_t newHeight ) const {
    if ( 0 == newWidth ) {
        throw invalidWidth( "The new width can't be 0." );
    }

    if ( 0 == newHeight ) {
        throw invalidHeight( "The new height can't be 0." );
    }

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
    auto* const image = new ColorImage( width, height );

    for ( uint16_t y = 0; y < height; ++y ) {
        for ( uint16_t x = 0; x < width; ++x ) {
            image->pixel(x,y) = pixel(static_cast<uint16_t>(x * (width_ / width)),
                                      static_cast<uint16_t>(y * (height_ / height))
            );
        }
    }

    return image;
}

GrayImage* GrayImage::bilinearScale( const std::intmax_t newWidth, const std::intmax_t newHeight ) const {
    // Peut simplifier la valeur de y2 et x2 car la division en bas vaut toujours 1
    // Car y2 = std::ceil(y) ou bien y2 = y1 + 1 = std::floor(y) + 1
    // Donc ratioY = (y -y1) / (y2 - y1) = y - y1
    imageUtils::verifyWidth( newWidth, std::numeric_limits<Width>::max() );

    imageUtils::verifyHeight( newHeight, std::numeric_limits<Height>::max() );

    std::vector<uint8_t> pixels(newWidth * newHeight);

    // The ratioW and ratioH was scale ratio between the new image and the old image
    const auto ratioW = static_cast<double>(width_) / newWidth;
    const auto ratioH = static_cast<double>(height_) / newHeight;

    // xp and yp was coordinates of pixels in the new image
    for ( uint16_t yp = 0; yp < newHeight; ++yp) {
        // We scale the coordinate yp in the scale of the old image
        const auto y = ratioH * yp;

        // We determine the boundary of the double y, between two nearest integers
        // Such as : y1 <= y <= y2
        const auto y1 = static_cast<uint16_t>( std::floor(y) );
        const auto y2 = static_cast<uint16_t>( std::ceil(y) < height_ ? std::ceil(y) : (height_ - 1) );

        // We determine the ratio of y between y1 and y2
        // In the case where y1 == y2, we have a mathematical error for the division, so the result was 0.0
        const auto ratioY = ( ( y2 != y1) ? ( ( y - y1) / ( y2 - y1) ) : 0.0 );

        // We do the same process of the coordinate yp, for the coordinate xp
        for ( uint16_t xp = 0; xp < newWidth; ++xp ) {
            const auto x = ratioW * xp;
            const auto x1 = static_cast<uint16_t>( std::floor(x) );
            const auto x2 = static_cast<uint16_t>( std::ceil(x) < width_ ? std::ceil(x) : (width_ - 1) );

            const auto ratioX = ( ( x2 != x1) ? ( ( x - x1) / ( x2 - x1) ) : 0.0 );

            // We take the gray shade of old image's pixels, who surround the new image's pixel's coordinate of x and y
            const auto p1 = pixel(x1, y1);
            const auto p2 = pixel(x1, y2);
            const auto p3 = pixel(x2, y1);
            const auto p4 = pixel(x2, y2);

            // We apply the bilinear scale's method to the new image's pixel
	        pixels.at(xp + (yp * newWidth)) = static_cast<uint8_t>(std::round(
            (( 1 - ratioY ) * ((( 1 - ratioX ) * p1 ) + ( ratioX * p3 )))
                + ( ratioY * ((( 1 - ratioX ) * p2 ) + ( ratioX * p4 )))
                ));
        }
    }

    return new GrayImage(newWidth, newHeight, intensity_, std::move(pixels));
}
