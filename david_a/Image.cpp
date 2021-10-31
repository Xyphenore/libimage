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
#include <bitset>
#include <type_traits>

extern "C" {
#include <jpeglib.h>
}

const char* const identifier = "david_a";
const char* const informations = "Beaucoup de méthodes dépréciées, car j'ai fait des méthodes soit plus sécurisées,"
                                 "soit avec moins de paramètres et une meilleure couche d'abstraction";




using alwaysData = std::runtime_error;
using badValuePixel = std::runtime_error;

using invalidType = std::invalid_argument;
using invalidWidth = std::invalid_argument;
using invalidHeight = std::invalid_argument;
using invalidIntensity = std::invalid_argument;
using invalidPosition = std::invalid_argument;
using invalidSizeArray = std::invalid_argument;
using invalidFormat = std::invalid_argument;
using invalidLength = std::invalid_argument;
using invalidColorMapType = std::invalid_argument;
using invalidCoordinateX = std::invalid_argument;
using invalidCoordinateY = std::invalid_argument;

using invalidShade = std::invalid_argument;
using invalidColor = std::invalid_argument;



// activer les exceptions pour les flux
// on doit vérifier dans quel type de boutisme on lit/ecrit dans les fonctions respective

/*static Color operator*( const Color& c, const long double alpha ) {
    return operator*(alpha, c);
}

 static Color operator*( const Color& c, const double alpha ) {
    return operator*(alpha, c);
}
 static Color operator*( const double alpha, const Color& c ) {
    return { static_cast<uint8_t>(std::round(c.r_ * alpha)),
             static_cast<uint8_t>(std::round(c.g_ * alpha)),
             static_cast<uint8_t>(std::round(c.b_ * alpha))
    };
}
*/
namespace imageUtils {
    // TODO Documenter
    constexpr std::streamsize oneByte = 1;
    constexpr std::streamsize twoBytes = 2;
    constexpr std::streamsize fourBytes = 4;

    // TODO Documenter ou raprocher de l'utilisation
    using invalidEnumTYPE = std::invalid_argument;

    // aka 65535
    constexpr static auto maxWidth = std::numeric_limits<Width>::max();
    constexpr static auto maxHeight = std::numeric_limits<Height>::max();

    // aka 255
    constexpr static auto maxIntensity = std::numeric_limits<Shade>::max();


    /// Activate the exception's throw for failbit at true, on an istream
    static void activateExceptionsForFailBitOn( std::ios& ios ) {
        ios.exceptions( std::ios_base::failbit );
    }

    /// Activate the exception's throw for badbit at true, on an istream
    static void activateExceptionsForBadBitOn( std::ios& ios ) {
        ios.exceptions( std::ios_base::badbit );
    }

    /// Activate the exception's throw for failbit and badbit, on an istream
    static void activateExceptionsOn( std::ios& ios ) {
        ios.exceptions( std::ios_base::failbit | std::ios_base::badbit );
    }

    // TODO
    // Penser que cela doit s'adapter au type resprésentant les channel de couleurs de la classe COLOR
    // Vérifier s'il n'y a pas d'overflox sur l'opération v1 + v2
    constexpr static double meanIfOver255( const double v1, const double v2 ) {
        const double max = 255;
        double value = -1;

        if ( ( v1 + v2 ) > max ) {
            value = ( v1 + v2 ) / 2;
        }
        else {
            value = v1 + v2;
        }

        return value;
    }

    /// Skip all comments up to a non-comment
    /// \param[in,out] input stream
    /// \exception THINK exception to input stream
    static void skip_comments( std::istream &is ) {
        // Activate throwing exceptions when failbit was set to true
        //activateExceptionsForFailBitOn( is );

        // Detect the start of a comments
        while ( '#' == std::ws(is).peek() ) {
            // Ignore all characters up to \n
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /// Skip one whitespace of the input stream
    /// \param[in,out] input stream
    /// \exception THINK exception to input stream
    static void skip_ONEwhitespace( std::istream& is ) {
        constexpr std::array<char,6> whitespace({ '\t', '\n', '\v',
                                                  '\f', '\r', ' '});

        const auto isWhiteSpace = std::binary_search(whitespace.cbegin(), whitespace.cend(), is.peek());

        if ( isWhiteSpace ) {
            is.get();
        }
    }

    static bool isEndOfLine( const intmax_t pos, const intmax_t width, const intmax_t limit ) {
        return ( 0 == ( pos % width ) ) || ( 0 == ( pos % limit ) );
    }

    static void verifyStreamContainData( std::istream& is ) {
        // Actualisation du flux
        is.peek();

        if ( !(is.eof()) ) {
            throw alwaysData( "Input stream always contain data" );
        }
    }

    template <typename Type>
    static void verifyOver0UnderOrEqualLimitOf( const intmax_t value, const Type limit ) {
        if ( ( 0 >= value ) || ( limit < value ) ) {
            std::ostringstream oss( "Value out ]0, ", std::ios::ate );
            oss << std::numeric_limits<intmax_t>::max() << "]";

            throw std::range_error( oss.str() );
        }
    }


    /// All functions contained in this namespace just verify a value is correct
    namespace VERIFY {
        /// Represents an interval between two limits of an interval
        /// \param [in] b1, b2 can be typed as intmax_t or as type with definition interval in the imtmax_t interval
        template <typename T>
        class Interval {
            static_assert( std::is_arithmetic<T>::value, "Error the given type T is not an arithmetic type(int or float)");
        public :
            /// Created an interval with limit b1, and limit b2
            Interval( const intmax_t b1, const intmax_t b2 )
                    : b1_(static_cast<T>(b1)), b2_(static_cast<T>(b2)) {}

            /// Return the minimal limit of the interval
            T min() const { return std::min( b1_, b2_); }

            /// Return the maximal limit of the interval
            T max() const { return std::max( b1_, b2_ ); }

        private :
            T b1_;
            T b2_;
        };


        /// Enum (YES/NO) to tells if we exclude limits of a given interval
        using LIMIT_EXCLUDE = BOOLEAN_TYPE;

        /// \returns true if the given value is in the interval (excluded or included)
        /// \returns false if the given value is not in the interval (excluded or included)
        /// \pre template LIMIT_EXCLUDE : YES or NO
        /// \pre if the given value was YES so interval limits was excluded
        /// \pre if the given value was NO so interval limits was included
        template <LIMIT_EXCLUDE excluded = LIMIT_EXCLUDE::NO, typename TValue, typename TInterval>
        static bool inInterval( const TValue value, const Interval<TInterval> interval ) {
            static_assert( std::is_integral<TValue>::value, "Error the given type TValue is not an integral type");
            static_assert( std::is_integral<TInterval>::value, "Error the given type TInterval is not an integral type");

            switch ( excluded ) {
                case LIMIT_EXCLUDE::NO :
                    return ( value <= interval.max() ) && ( interval.min() <= value );

                case LIMIT_EXCLUDE::YES :
                    return ( value < interval.max() ) && ( interval.min() < value );

                default :
                    throw invalidEnumTYPE( "The given enum type in template setting was unknown");
            }
        }


        /// \returns true if the given value was over 0
        /// \returns false if the given value was not over 0
        template <typename T>
        static bool isOver0( const T value ) {
            static_assert( std::is_arithmetic<T>::value, "Error the given type T is not an arithmetic type");

            return value > 0;
        }

        /// \returns true if the given value is less than the given limit
        /// \returns false if the given value is greater or equal than the given limit
        template <typename T>
        static bool isUnderLimit( const T value, const T limit ) {
            static_assert( std::is_integral<T>::value, "Error the given type T is not an integral type");

            return value < limit;
        }


        /// Do nothing except throw
        /// \throw invalidWidth if the given width don't respect the given interval
        template<typename TWidth>
        static void verifyWidth( const intmax_t width, const Interval<TWidth> interval ) {
            if ( ( !isOver0(width) ) || ( !inInterval( width, interval ))) {
                throw invalidWidth( "Bad width of Image" );
            }
        }

        /// Do nothing except throw
        /// \throw invalidHeight if the given height don't respect the given interval
        template<typename THeight>
        static void verifyHeight( const intmax_t height, const Interval<THeight> interval ) {
            if ( ( !isOver0(height) ) || ( !inInterval( height, interval ))) {
                throw invalidHeight( "Bad height of Image" );
            }
        }

        /// Do nothing except throw
        /// \throw invalidIntensity if the given intensity don't respect the given interval
        template <typename TIntensity>
        static void verifyIntensity( const intmax_t intensity, const Interval<TIntensity> interval ) {
            if ( ( !isOver0(intensity) ) || ( !inInterval( intensity, interval ))) {
                throw invalidIntensity( "Bad intensity of Image" );
            }
        }

        /// Do nothing except throw
        /// \throw invalidLength if the given length don't respect the given interval
        template <typename TLength>
        static void verifyLength( const intmax_t length, const Interval<TLength> interval ) {
            if ( !inInterval( length, interval) ) {
                throw invalidLength( "Bad length" );
            }
        }

        /// Do nothing except throw
        /// \throw invalidShade if the given shade don't respect the given interval
        template <typename TShade>
        static void verifyShade( const intmax_t shade, const Interval<TShade> interval ) {
            if ( !inInterval( shade, interval ) ) {
                throw invalidShade( "Bad shade" );
            }
        }

        /// Verify the given color was under or equal to the given color limit
        /// \throw invalidColor if the given color is not under or equal to the given color limit
        static void verifyColor( const Color c, const Color limit ) {
            if ( (!inInterval( c.r_, Interval<Shade>{0,limit.r_})) ||
                 (!inInterval( c.g_, Interval<Shade>{0,limit.g_})) ||
                 (!inInterval( c.b_, Interval<Shade>{0,limit.b_})) ) {
                throw invalidColor("The given color is over the given limit");
            }
        }

        /// Verify all pixels contained in the given vector, are under or equals to the given limit
        /// \throw invalidType if TPixel is not integral or Color
        /// \throw badValuePixel if a pixel is over the given limit
        template <typename TPixel, typename TLimit>
        static void verifyPixels( const std::vector<TPixel>& pixels, const TLimit limit ) {
            if ( std::is_same<TPixel, Color>::value && (limit < std::numeric_limits<TLimit>::max()) ) {
                const auto overLimit = [limit]( const Color pixel )
                { return !inInterval( pixel.r_, Interval<TLimit>{0,limit}) ||
                         !inInterval( pixel.g_, Interval<TLimit>{0,limit}) ||
                         !inInterval( pixel.b_, Interval<TLimit>{0,limit});
                };

                const auto pos = std::find_if( pixels.cbegin(), pixels.cend(), overLimit );

                if ( pos != pixels.cend() ) {
                    std::ostringstream oss( "Bad pixel at ", std::ios::ate );
                    oss << static_cast<intmax_t>( pos - pixels.cbegin() ) << " element";
                    throw badValuePixel( oss.str() );
                }
            }
            else if ( (!std::is_same<TPixel, Color>::value) && std::is_integral<TPixel>::value && (limit < std::numeric_limits<TLimit>::max()) ) {
                const auto overLimit = [limit]( const TPixel pixel )
                        { return !inInterval(pixel, Interval<TPixel>{0,limit}); };

                const auto pos = std::find_if( pixels.cbegin(), pixels.cend(), overLimit );

                if ( pos != pixels.cend() ) {
                    std::ostringstream oss( "Bad pixel at ", std::ios::ate );
                    oss << static_cast<intmax_t>( pos - pixels.cbegin() ) << " element";
                    throw badValuePixel( oss.str() );
                }
            }
            else {
                throw invalidType( "The given template type was not manage by this function");
            }
        }

        /// Do nothing except throw
        /// \throw invalidSizeArray if the given vector don't have the same size of the given maxSize
        template <typename TPixel>
        static void verifySizeArray( const std::vector<TPixel>& pixels, const size_t maxSize ) {
            if ( pixels.size() != maxSize ) {
                throw invalidSizeArray( "The given array does not match with the size of Image" );
            }
        }

        // TODO
        /// Verify the given Position (x,y) was in Interval[0, given limit[
        /// \throw invalidCoordinateX if x is not in the interval [0, given xlimit[
        /// \throw invalidCoordinateY if y is not in the interval [0, given ylimit[
        template <typename TPosition>
        static void verifyPosition( const Point p, const Interval<TPosition> xInterval, const Interval<TPosition> yInterval ) {
            if ( (!inInterval( p.x, xInterval)) || (!isUnderLimit<intmax_t>( p.x, xInterval.max())) ) {
                throw invalidCoordinateX( "The given x position is invalid" );
            }

            if ( (!inInterval( p.y, yInterval)) || (!isUnderLimit<intmax_t>( p.y, yInterval.max())) ) {
                throw invalidCoordinateY( "The given y position is invalid" );
            }
        }
    }

    /// All functions contained in this namespace can read value represented by ASCII
    namespace ASCII {
        using namespace VERIFY;

        // TODO Complete ios_exception
        /// \throws invalidWidth if the read value from the given stream, was outside of Interval]0, maxWidth]
        /// \throws ios_exception if the given stream failed or if the given stream is invalid
        template <typename TWidth>
        static TWidth readWidth( std::istream& is ) {
            intmax_t length = 0;
            is >> length;

            verifyWidth( length, Interval<TWidth>(0,maxWidth));

            return static_cast<TWidth>(length);
        }

        // TODO Complete ios_exception
        /// \throws invalidHeight if the read value from the given stream, was outside of Interval]0, maxHeight]
        /// \throws ios_exception if the given stream failed or if the given stream is invalid
        template <typename THeight>
        static THeight readHeight( std::istream& is ) {
            intmax_t height = 0;
            is >> height;

            verifyHeight( height, Interval<THeight>(0,maxHeight));

            return static_cast<THeight>(height);
        }

        // TODO Documenter
        template <typename TIntensity>
        static TIntensity readIntensity( std::istream& is ) {
            intmax_t intensity = 0;
            is >> intensity;

            VERIFY::verifyIntensity( intensity, VERIFY::Interval<Shade>{0,maxIntensity} );

            return static_cast<TIntensity>(intensity);
        }
    }



    template <typename Type>
    static void verifyOverEqual0UnderEqualLimitOf( const intmax_t value, const Type limit ) {
        if ( ( 0 > value ) || ( limit < value ) ) {
            std::ostringstream oss( "Value out [0, ", std::ios::ate );
            oss << std::numeric_limits<intmax_t>::max() << "]";

            throw std::range_error( oss.str() );
        }
    }
}

using namespace imageUtils;

// Si on passe a c++17, supprimer ce qui suit
const Shade GrayImage::black = 0;
const Shade& GrayImage::defaultColor = black;
const Shade GrayImage::defaultIntensity = imageUtils::maxIntensity;

const Color ColorImage::black = Color();
const Color& ColorImage::defaultColor = black;
const Shade ColorImage::defaultIntensity = imageUtils::maxIntensity;



// Color's method
Color::Color( const intmax_t r, const intmax_t g, const intmax_t b )
: r_( static_cast<Shade>(r) ), g_( static_cast<Shade>(g) ), b_( static_cast<Shade>(b)) {
    constexpr auto limit = imageUtils::maxIntensity;

    if ( ( 0 > r ) || ( limit < r ) ) {
        throw invalidColor( "Bad Color : red channel was out of range [0;255]" );
    }

    if ( ( 0 > g ) || ( limit < g ) ) {
        throw invalidColor( "Bad Color : green channel was out of range [0;255]" );
    }

    if ( ( 0 > b ) || ( limit < b ) ) {
        throw invalidColor( "Bad Color : blue channel was out of range [0;255]" );
    }
}



// Definitions of Color
static Color operator+( const Color& c1, const Color& c2 ) {
    return { static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.r_, c2.r_)) ),
             static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.g_, c2.g_)) ),
             static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.b_, c2.b_)) )
    };
}
static Color operator*( const long double alpha, const Color& c ) {
    return { static_cast<uint8_t>(std::round(c.r_ * alpha)),
             static_cast<uint8_t>(std::round(c.g_ * alpha)),
             static_cast<uint8_t>(std::round(c.b_ * alpha))
    };
}

static bool operator==( const Color& c1, const Color& c2 ) {
    return (c1.r_ == c2.r_) && (c1.g_ == c2.g_) && (c1.b_ == c2.b_);
}
static bool operator!=( const Color& c1, const Color& c2 ) {
    return !operator==(c1,c2);
}




// Definition of GrayImage's methods

// Public builders
GrayImage::GrayImage( const intmax_t width, const intmax_t height )
: GrayImage(imageUtils::Dimension<>{ width, height }, defaultIntensity ) {}

GrayImage::GrayImage( const imageUtils::Dimension<> dim )
: GrayImage( dim, defaultIntensity ) {}

GrayImage::GrayImage( const intmax_t width, const intmax_t height, const intmax_t intensity )
: GrayImage(imageUtils::Dimension<>{ width, height }, intensity ) {}

GrayImage::GrayImage( const imageUtils::Dimension<> dim, const intmax_t intensity )
: dimension{ static_cast<Width>(dim.width), static_cast<Height>(dim.height) },
intensity_( static_cast<Shade>(intensity) ),pixels_( dimension.width * dimension.height ) {
    // Verify all preconditions
    imageUtils::VERIFY::verifyWidth( dim.width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( dim.height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );
    imageUtils::VERIFY::verifyIntensity( intensity, imageUtils::VERIFY::Interval<Shade>{0,imageUtils::maxIntensity} );

    // Fill the image with the default Color
    fill( defaultColor );
}


// Private builders
GrayImage::GrayImage( const imageUtils::Dimension<> dim, const intmax_t intensity, std::vector<Shade>&& pixels )
        : dimension{ static_cast<Width>(dim.width), static_cast<Height>(dim.height) },
          intensity_( static_cast<Shade>(intensity) ), pixels_( std::move( pixels ) ) {
    // Verify all preconditions
    // TODO Factoriser en verifyDimension
    imageUtils::VERIFY::verifyWidth( dim.width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( dim.height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );

    imageUtils::VERIFY::verifyIntensity( intensity, imageUtils::VERIFY::Interval<Shade>{0,imageUtils::maxIntensity} );

    imageUtils::VERIFY::verifySizeArray( pixels_, dimension.width * dimension.height );
    imageUtils::VERIFY::verifyPixels( pixels_, intensity_ );
}

std::unique_ptr<GrayImage>
GrayImage::createGrayImage( const imageUtils::Dimension<> dim, const intmax_t intensity, std::vector<Shade>&& pixels ) {
    // Tour de magie un constructeur privé devient public dans une classe héritière
    // Nécessaire pour le std::make_unique<GrayImage>
    // http://www.robert-puskas.info/2018/10/lod-using-make-shared-unique-with-private-constructors.html
    class MkGrayImage : public GrayImage {
    public :
        MkGrayImage( const imageUtils::Dimension<> dim, const intmax_t intensity, std::vector<Shade>&& pixels )
                : GrayImage( dim, intensity, std::move( pixels ) ) {}
    };

    return std::make_unique<MkGrayImage>( dim, intensity, std::move( pixels ) );
}

// Getter / Setter
Shade& GrayImage::pixel( const intmax_t x, const intmax_t y ) {
    const auto& imageDim = dimension;
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,imageDim.width},
                            VERIFY::Interval<Height>{0,imageDim.height} );

    return pixels_.at( static_cast<size_t>( ( imageDim.width * y ) + x ) );
}

const Shade& GrayImage::pixel( const intmax_t x, const intmax_t y ) const {
    const auto& imageDim = dimension;
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,imageDim.width},
                            VERIFY::Interval<Height>{0,imageDim.height} );

    return pixels_.at( static_cast<size_t>( ( imageDim.width * y ) + x ) );
}

void GrayImage::setPixel( const imageUtils::Pixel px, const intmax_t color ) {
    const auto& imageDim = dimension;
    imageUtils::VERIFY::verifyPosition( px, VERIFY::Interval<Width>{0,imageDim.width}, VERIFY::Interval<Height>{0,imageDim.height} );

    imageUtils::VERIFY::verifyShade( color, imageUtils::VERIFY::Interval<Shade>{0,intensity_} );

    pixels_.at( static_cast<size_t>( ( imageDim.width * px.y ) + px.x ) ) = static_cast<Shade>(color);
}

Shade GrayImage::getPixel( const imageUtils::Pixel px ) const {
    const auto& imageDim = dimension;
    imageUtils::VERIFY::verifyPosition( px, VERIFY::Interval<Width>{0,imageDim.width}, VERIFY::Interval<Height>{0,imageDim.height} );

    return pixels_.at( static_cast<size_t>( ( imageDim.width * px.y ) + px.x ) );
}

// Filler
void GrayImage::fill( const intmax_t color ) {
    imageUtils::VERIFY::verifyShade( color, imageUtils::VERIFY::Interval<Shade>{0,intensity_} );

    const auto colorGrayShade = static_cast<Shade>( color );

    std::fill( pixels_.begin(), pixels_.end(), colorGrayShade );
}

void GrayImage::drawLine(
        const imageUtils::Point start, const intmax_t length, const intmax_t color, const imageUtils::TYPE type ) {
    const auto& imageDim = dimension;

    imageUtils::VERIFY::verifyPosition( start, VERIFY::Interval<Width>{0,imageDim.width}, VERIFY::Interval<Height>{0,imageDim.height} );
    imageUtils::VERIFY::verifyShade( color, imageUtils::VERIFY::Interval<Shade>{0,intensity_} );

    switch ( type ) {
        case imageUtils::TYPE::VERTICAL :
            imageUtils::VERIFY::verifyLength( start.y + length, imageUtils::VERIFY::Interval<Height>{0,imageDim.height});
            for ( intmax_t j = start.y; j < ( start.y + length ); ++j ) {
                pixels_.at( static_cast<size_t>(( imageDim.width * j ) + start.x) ) = static_cast<Shade>(color);
            }

            break;

        case imageUtils::TYPE::HORIZONTAL :
            imageUtils::VERIFY::verifyLength( start.x + length, imageUtils::VERIFY::Interval<Width>{0,imageDim.width});
            for ( intmax_t i = start.x; i < ( start.x + length ); ++i ) {
                pixels_.at( static_cast<size_t>(( imageDim.width * start.y ) + i) ) = static_cast<Shade>(color);
            }

            break;

        default :
            throw imageUtils::invalidEnumTYPE( "The given type was unknown for this function" );
    }
}

void GrayImage::drawRectangle(
        const imageUtils::Point start, const imageUtils::Dimension<> rectangleDim, const intmax_t color,
        const imageUtils::FILL filled ) {
    const auto& imageDim = dimension;
    imageUtils::VERIFY::verifyPosition( start, VERIFY::Interval<Width>{0,imageDim.width}, VERIFY::Interval<Height>{0,imageDim.height} );

    imageUtils::VERIFY::verifyLength( start.x + rectangleDim.width, imageUtils::VERIFY::Interval<Width>{0,imageDim.width});
    imageUtils::VERIFY::verifyLength( start.y + rectangleDim.height, imageUtils::VERIFY::Interval<Height>{0,imageDim.height});

    imageUtils::VERIFY::verifyShade( color, imageUtils::VERIFY::Interval<Shade>{0,intensity_} );

    switch ( filled ) {
        case imageUtils::FILL::NO :
            drawLine( start, rectangleDim.width, color, imageUtils::TYPE::HORIZONTAL );
            drawLine( imageUtils::Point{ start.x, start.y + 1 }, rectangleDim.height - 2, color,
                      imageUtils::TYPE::VERTICAL );
            drawLine( imageUtils::Point{ ( start.x - 1 ) + rectangleDim.width, start.y + 1 }, rectangleDim.height - 2,
                      color, imageUtils::TYPE::VERTICAL );
            drawLine( imageUtils::Point{ start.x, ( start.y - 1 ) + rectangleDim.height }, rectangleDim.width, color,
                      imageUtils::TYPE::HORIZONTAL );
            break;

        case imageUtils::FILL::YES :
            for ( intmax_t i = start.y; i < ( start.y + rectangleDim.height ); ++i ) {
                drawLine( imageUtils::Point{ start.x, i }, rectangleDim.width, color, imageUtils::TYPE::HORIZONTAL );
            }
            break;

        default :
            throw imageUtils::invalidEnumTYPE( "The given type was unknown for this function" );
    }
}

// Writers
void GrayImage::writePGM( std::ostream& os, const Format::WRITE_IN f ) const {
    using Format = Format::WRITE_IN;
    const auto& imageDim = dimension;

    // Write u=in a fonction verifyFormat( format, goodformat list )
    if ( ( f != Format::ASCII ) && ( f != Format::BINARY ) ) {
        throw invalidFormat( "Unknown image format" );
    }

    imageUtils::ASCII::verifyPixels<Shade>( pixels_, intensity_ );

    if ( f == Format::BINARY ) {
        // TODO Changer identifier par la variable environnement nom utilisateur
        os << "P5\n" << "# Image sauvegardée par " << ::identifier << '\n' << imageDim.width << " " << imageDim.height
           << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';


        os.write( reinterpret_cast<const char*>(pixels_.data()),
                  static_cast<std::streamsize>(imageDim.width * imageDim.height * sizeof( Shade )) );
    }
    else {
        // Full ASCII format
        // TODO a refaire car la sortie n'est pas bonne
        constexpr uint16_t pgm_limit_char = 70;

        os << "P2\n" << "# Image sauvegardée par " << ::identifier << '\n' << imageDim.width << " " << imageDim.height
           << '\n' << static_cast<uint16_t>(intensity_) << '\n';


        for ( uint16_t i = 0; i < ( imageDim.width * imageDim.height ); ++i ) {
            if ( ( 0 != i ) && imageUtils::isEndOfLine( i + 1, imageDim.width, pgm_limit_char ) ) { os << '\n'; }

            os << pixels_[i] << " ";
        }
    }

    os << '\n' << std::flush;
}

// Readers
std::unique_ptr<GrayImage> GrayImage::readPGM_secured( std::istream& is ) {
    std::string type;
    is >> type;

    if ( ( type != "P5" ) && ( type != "P2" ) ) {
        throw invalidType( "Bad format of file" );
    }

    imageUtils::skip_comments( is );
    const auto width = imageUtils::ASCII::readWidth<Width>( is );

    imageUtils::skip_comments( is );
    const auto height = imageUtils::ASCII::readHeight<Height>( is );

    imageUtils::skip_comments( is );
    const auto intensity = imageUtils::ASCII::readIntensity<Shade>( is );

    imageUtils::skip_ONEwhitespace( is );

    std::vector<Shade> pixels( width * height );

    // Attraper l'exception EOF, badRead(bad bit), fail bit
    if ( type == "P5" ) {
        // Expliquer la lecture
        try {
            is.read( reinterpret_cast<char*>(pixels.data()),
                     static_cast<std::streamsize>(width * height * sizeof( Shade )) );
        } catch ( ... ) {
            std::cerr << "Error occurred on stream";
        }
    }
    else {
        // P2 format
        // Utiliser from_chars()
        for ( size_t i = 0; i < ( width * height ); ++i ) {
            is >> pixels.at( i );
        }
    }

    imageUtils::ASCII::verifyPixels( pixels, intensity );
    imageUtils::skip_ONEwhitespace( is );

    imageUtils::verifyStreamContainData( is );

    return createGrayImage( imageUtils::Dimension<>{ width, height }, intensity, std::move( pixels ) );
}


// Scaler
std::unique_ptr<GrayImage> GrayImage::simpleScale( const imageUtils::Dimension<> newDim ) const {
    imageUtils::VERIFY::verifyWidth( newDim.width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( newDim.height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight});

    std::vector<Shade> pixels( static_cast<size_t>(newDim.width * newDim.height) );

    const auto& thisDim = dimension;

    const auto ratioW = static_cast<long double>(thisDim.width) / newDim.width;
    const auto ratioH = static_cast<long double>(thisDim.height) / newDim.height;

    for ( Height y = 0; y < newDim.height; ++y ) {
        for ( Width x = 0; x < newDim.width; ++x ) {
            pixels.at( static_cast<size_t>( ( newDim.width * y ) + x) ) = getPixel(
                    imageUtils::Pixel{ static_cast<intmax_t>(x * ratioW), static_cast<intmax_t>(y * ratioH) } );
        }
    }

    return createGrayImage( newDim, intensity_, std::move( pixels ) );
}

std::unique_ptr<GrayImage> GrayImage::bilinearScale( const imageUtils::Dimension<> newDim ) const {
    // Peut simplifier la valeur de y2 et x2 car la division en bas vaut toujours 1
    // Car y2 = std::ceil(y) ou bien y2 = y1 + 1 = std::floor(y) + 1
    // Donc ratioY = (y -y1) / (y2 - y1) = y - y1
    imageUtils::VERIFY::verifyWidth( newDim.width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( newDim.height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );


    std::vector<Shade> pixels( static_cast<size_t>(newDim.width * newDim.height) );

    const auto& thisDim = dimension;
    // The ratioW and ratioH was scale ratio between the new image and the old image
    const auto ratioW = static_cast<long double>(thisDim.width) / newDim.width;
    const auto ratioH = static_cast<long double>(thisDim.height) / newDim.height;

    // xp and yp was coordinates of pixels in the new image
    for ( Height yp = 0; yp < newDim.height; ++yp ) {
        // We scale the coordinate yp in the scale of the old image
        const auto y = ( ( (ratioH * yp) - 0.5 ) < 0 ? 0 : ( (ratioH * yp) - 0.5 ) );

        // We determine the boundary of the double y, between two nearest integers
        // Such as : y1 <= y <= y2
        const auto y1 = static_cast<Height>( std::floor(y) );
        const auto y2 = static_cast<Height>( std::ceil(y) < thisDim.height ? std::ceil(y) : ( thisDim.height - 1 ) );

        // We determine the ratio of y between y1 and y2
        // In the case where y1 == y2, we have a mathematical error for the division, so the result was 0.0
        const auto ratioY = ( ( y2 != y1 ) ? ( ( y - y1 ) / ( y2 - y1 ) ) : 0.0 );

        // We do the same process of the coordinate yp, for the coordinate xp
        for ( Width xp = 0; xp < newDim.width; ++xp ) {
            const auto x = ( ( ( ratioW * xp ) - 0.5 ) < 0 ? 0 : ( ( ratioW * xp ) - 0.5 ) );
            const auto x1 = static_cast<Width>( std::floor(x) );
            const auto x2 = static_cast<Width>( std::ceil(x) < thisDim.width ? std::ceil(x) : ( thisDim.width - 1 ) );

            const auto ratioX = ( ( x2 != x1 ) ? ( ( x - x1 ) / ( x2 - x1 ) ) : 0.0 );

            // We take the gray shade of old image's pixels, who surround the new image's pixel's coordinate of x and y
            const auto p1 = getPixel( imageUtils::Pixel{ x1, y1 } );
            const auto p2 = getPixel( imageUtils::Pixel{ x1, y2 } );
            const auto p3 = getPixel( imageUtils::Pixel{ x2, y1 } );
            const auto p4 = getPixel( imageUtils::Pixel{ x2, y2 } );

            // We apply the bilinear scale's method to the new image's pixel
            pixels.at( static_cast<size_t>(xp + ( yp * newDim.width )) ) = static_cast<Shade>(std::round(
                    ( ( 1 - ratioX ) * ( ( ( 1 - ratioY ) * p1 ) + ( ratioY * p2 ) ) ) +
                    ( ratioX * ( ( ( 1 - ratioY ) * p3 ) + ( ratioY * p4 ) ) ) ));
        }
    }

    return createGrayImage( newDim, intensity_, std::move( pixels ) );
}


// Definition of ColorImage's methods

// Public builders
ColorImage::ColorImage( const intmax_t width, const intmax_t height ) : ColorImage( width, height, defaultIntensity ) {}

ColorImage::ColorImage( const intmax_t width, const intmax_t height, const intmax_t intensity ) : width_(
        static_cast<Width>(width) ), height_( static_cast<Height>(height) ), intensity_(
        static_cast<Shade>(intensity) ), pixels_( width_ * height_ ) {
    // Verify all preconditions
    imageUtils::VERIFY::verifyWidth( width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );
    imageUtils::VERIFY::verifyIntensity( intensity, imageUtils::VERIFY::Interval<Shade>{0,imageUtils::maxIntensity} );

    // Fill the image with the default Color
    fill( defaultColor );
}


// Private builders
ColorImage::ColorImage(
        const intmax_t width, const intmax_t height, const intmax_t intensity, std::vector<Color>&& pixels ) : width_(
        static_cast<Width>(width) ), height_( static_cast<Height>(height) ), intensity_(
        static_cast<Shade>(intensity) ), pixels_( std::move( pixels ) ) {
    // Verify all preconditions
    imageUtils::VERIFY::verifyWidth( width, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( height, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );
    imageUtils::ASCII::verifySizeArray( pixels_, width_ * height_ );
    imageUtils::VERIFY::verifyIntensity( intensity, imageUtils::VERIFY::Interval<Shade>{0,imageUtils::maxIntensity});
    imageUtils::ASCII::verifyPixels<Color,Shade>( pixels_, intensity_ );
}

// Getter / Setter
Color& ColorImage::pixel( const intmax_t x, const intmax_t y ) {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );

    return pixels_.at( static_cast<size_t>( ( width_ * y ) + x ) );
}

const Color& ColorImage::pixel( const intmax_t x, const intmax_t y ) const {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );

    return pixels_.at( static_cast<size_t>( ( width_ * y ) + x ) );
}

// Filler
void ColorImage::fill( const Color color ) {
    std::fill( pixels_.begin(), pixels_.end(), color );
}

void ColorImage::horizontalLine( const intmax_t x, const intmax_t y, const intmax_t length, const Color color ) {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );
    imageUtils::VERIFY::verifyLength( length, imageUtils::VERIFY::Interval<Width>{0,width_ - x} );
    imageUtils::VERIFY::verifyColor( color, Color( intensity_, intensity_, intensity_ ) );

    for ( intmax_t i = x; i < ( x + length ); ++i ) {
        pixel( i, y ) = color;
    }
}

void ColorImage::verticalLine( const intmax_t x, const intmax_t y, const intmax_t length, const Color color ) {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );
    imageUtils::VERIFY::verifyLength( length, imageUtils::VERIFY::Interval<Height>{0,height_ - y} );
    imageUtils::VERIFY::verifyColor( color, Color( intensity_, intensity_, intensity_ ) );

    for ( intmax_t j = y; j < ( y + length ); ++j ) {
        pixel( x, j ) = color;
    }
}

void ColorImage::rectangle(
        const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height, const Color color ) {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );
    imageUtils::VERIFY::verifyWidth( width, imageUtils::VERIFY::Interval<Width>{0,width_ - x} );
    imageUtils::VERIFY::verifyHeight( height, imageUtils::VERIFY::Interval<Height>{0,height_ - y} );
    imageUtils::VERIFY::verifyColor( color, Color( intensity_, intensity_, intensity_ ) );

    horizontalLine( x, y, width, color );
    horizontalLine( x, ( y - 1 ) + height, width, color );

    verticalLine( x, y + 1, height - 2, color );
    verticalLine( ( x - 1 ) + width, y + 1, height - 2, color );
}

void ColorImage::fillRectangle(
        const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height, const Color color ) {
    VERIFY::verifyPosition( Point{x,y},
                            VERIFY::Interval<Width>{0,width_},
                            VERIFY::Interval<Height>{0,height_} );
    imageUtils::VERIFY::verifyWidth( width, imageUtils::VERIFY::Interval<Width>{0,width_ - x} );
    imageUtils::VERIFY::verifyHeight( height, imageUtils::VERIFY::Interval<Height>{0,height_ - y} );
    imageUtils::VERIFY::verifyColor( color, Color( intensity_, intensity_, intensity_ ) );

    for ( intmax_t i = y; i < ( y + height ); ++i ) {
        horizontalLine( x, i, width, color );
    }
}

// Writers
void ColorImage::writePPM( std::ostream& os, const Format::WRITE_IN f ) const {
    using Format = Format::WRITE_IN;

    if ( ( f != Format::ASCII ) && ( f != Format::BINARY ) ) {
        throw invalidFormat( "Unknown image format" );
    }

    imageUtils::ASCII::verifyPixels<Color>( pixels_, intensity_ );

    if ( f == Format::BINARY ) {
        os << "P6\n" << "# Image sauvegardée par " << ::identifier << '\n' << width_ << " " << height_ << '\n'
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
            if ( ( 0 != i ) && imageUtils::isEndOfLine( i+1, width_, pgm_limit_char ) ) { os << '\n'; }

            os << static_cast<uint16_t>(pixels_[i].r_) << " " << static_cast<uint16_t>(pixels_[i].g_) << " "
               << static_cast<uint16_t>(pixels_[i].b_) << " ";
        }
    }

    os << '\n' << std::flush;
}

void ColorImage::writeTGA( std::ostream& os, const Format::WRITE_IN ) const {
    // oneByte 1 = 0 because no identification information for the image
    os.put( 0 );

    // oneByte 2 = 0 because no color map
    os.put( 0 );

    // oneByte 3 = 2 because RGB no RLE
    os.put( 2 );

    // oneByte 4,5 = 0
    os.put( 0 );
    os.put( 0 );

    // oneByte 6,7 = 0
    os.put( 0 );
    os.put( 0 );

    // oneByte 8 = 0 because no color map
    os.put( 0 );

    // oneByte 9,10 = X origin of image (lo-hi)
    os.put( 0 );
    os.put( 0 );

    // oneByte 11,12 = Y origin of image (lo-hi)
    os.put( 0 );
    os.put( 0 );

    // oneByte 13,14 = Image width
    os.write( reinterpret_cast<const char*>(&width_), 2 );

    // oneByte 15,16 = Image height
    os.write( reinterpret_cast<const char*>(&height_), 2 );

    // oneByte 17 = 24 because with use Targa 24 bits
    os.put( 24 );

    // oneByte 18 = {  bits0-3 = 0000 because 24bits,
    //              bit4 = 0 because reserved,
    //              bit5 = {0,1} its origin of image (lower left, upper left)
    //              bit6-7 = 00 because the written image is progressive
    const uint8_t byte18 = 32; // Because image start top left
    os.put( byte18 );

    // oneByte 18-size in oneByte 1 = Image identification field, but oneByte 1 = 0 so nothing
    // oneByte 18 - nothing, because no color map

    // oneByte 18 - size of Image = pixels
    // Vérifier la taille de la classe Color
    /*os.write( reinterpret_cast<const char*>(pixels_.data()),
              static_cast<std::streamsize>(width_ * height_ * sizeof( Color )) );*/

    for ( size_t i = 0; i != static_cast<size_t>(width_ * height_); ++i ) {
        os.put( pixels_[i].b_ );
        os.put( pixels_[i].g_ );
        os.put( pixels_[i].r_ );
    }

    // No dev or ext fields
    // Maybe add a basic footer
}

// Readers
ColorImage* ColorImage::readPPM( std::istream& is ) {
    std::string type;
    is >> type;

    if ( ( type != "P6" ) && ( type != "P3" ) ) {
        throw invalidType( "Bad format of file" );
    }

    imageUtils::skip_comments( is );
    const auto width = imageUtils::ASCII::readWidth<Width>( is );

    imageUtils::skip_comments( is );
    const auto height = imageUtils::ASCII::readHeight<Height>( is );

    imageUtils::skip_comments( is );
    const auto intensity = imageUtils::ASCII::readIntensity<Shade>( is );

    imageUtils::skip_ONEwhitespace( is );

    std::vector<Color> pixels( width * height );

    // Attraper l'exception EOF, badRead(bad bit), fail bit
    if ( type == "P6" ) {
        // Expliquer la lecture
        try {
            is.read( reinterpret_cast<char*>(pixels.data()),
                     static_cast<std::streamsize>(width * height * sizeof( Color )) );
        } catch ( ... ) {
            std::cerr << "Error occurred on stream";
        }
    }
    else {
        // type P3
        for ( size_t i = 0; i < ( width * height ); ++i ) {
            is >> pixels.at( i ).r_ >> pixels.at( i ).g_ >> pixels.at( i ).b_;
        }
    }

    imageUtils::ASCII::verifyPixels( pixels, intensity );
    imageUtils::skip_ONEwhitespace( is );

    imageUtils::verifyStreamContainData( is );

    return new ColorImage( width, height, intensity, std::move( pixels ) );
}

struct ColorMap {
    uint16_t firstcolor = 0;
    uint16_t countColor = 0;
    uint8_t nbBitsColor = 0;
};

static ColorMap readColorMap( std::istream& is ) {
    uint16_t firstColor = 0;
    uint16_t countColor = 0;
    uint8_t nbBitsColor = 0;

    is.read( reinterpret_cast<char*>(&firstColor), imageUtils::twoBytes );
    is.read( reinterpret_cast<char*>(&countColor), imageUtils::twoBytes );
    is.read( reinterpret_cast<char*>(&nbBitsColor), imageUtils::oneByte );

    // TODO Vérification de nbBitsColor {15.16.24.32}

    return { firstColor, countColor, nbBitsColor };
}

static bool holdColorMap( std::istream& is ) {
    // Other values than 0 or 1, throw exception
    uint8_t containeMap = 2;

    imageUtils::activateExceptionsOn( is );
    is.read( reinterpret_cast<char*>(&containeMap), imageUtils::oneByte );

    if ( ( containeMap != 0 ) && ( containeMap != 1 ) ) {
        throw invalidColorMapType( "Bad read value of Color Map Type, please verify the given file" );
    }

    return ( 1 == containeMap );
}

enum class TargaFormat {};

ColorImage* ColorImage::readTGA( std::istream& is ) {
    /*
    constexpr size_t lengthFooter = 26;
    const std::string SIGNATURE("TRUEVISION-XFILE");

    // Move to start of footer
    is.seekg( -lengthFooter, std::ios_base::end );

    // Read size of dev field
    intmax_t sizeDevField = -1;
    is.read( reinterpret_cast<char*>(&sizeDevField), 4);
    // Vérifier taille >= 0

    // Read size of ext field
    intmax_t sizeExtField = -1;
    is.read( reinterpret_cast<char*>(&sizeExtField), 4);
    // Vérifier taille >= 0

    // Read Signature in footer
    auto const rawSignature = new char[SIGNATURE.length()];
    is.read( rawSignature, SIGNATURE.length() );

    // Vérification que l'on a le format Original de Targa
    if ( rawSignature == SIGNATURE ) {
        // Récuperer la taille des developpeurs et la taille des extensions
        //throw invalidFormat( "The format TRUEVISION-XFILES is not supported. Only the ORIGINAL format of Targa");
    }


    // Déplacement au début du fichier
    is.seekg(0, std::ios_base::beg);
     */

    imageUtils::activateExceptionsOn( is );
    // 1 Lecture de la taille du champ d'identification
    uint8_t sizeIdentificationField = 0;
    is.read( reinterpret_cast<char*>(&sizeIdentificationField), imageUtils::oneByte );

    // 2 Contient une palette de couleur
    const bool containColormap = holdColorMap( is );

    // 3 Type de l'image
    // Fonction qui retourne un type TargaFormat, qui comportera une liste des formats targa
    uint8_t type = 0;
    is.read( reinterpret_cast<char*>(&type), imageUtils::oneByte );

    // Verification que la type est 2 pour le rgb non compressé
    // 1 pour le color mapped non compressé
    // 10 rgb rle
    // 9 color mapped rle
    if ( type != 2 ) {
        throw invalidFormat( "Only supported format 2 RGB non compressed" );
    }

    // 4 Spec de palette couleur
    const ColorMap colorMap = readColorMap( is );

    if ( containColormap ) {
        //read
    }
    else {
        // jump to
    }


    // 5 Spec Image
    uint16_t originX;
    is.read( reinterpret_cast<char*>(&originX), imageUtils::twoBytes );

    // Verifier l'origine x

    uint16_t originY;
    is.read( reinterpret_cast<char*>(&originY), imageUtils::twoBytes );

    // Verifier l'origine y

    uint16_t width;
    is.read( reinterpret_cast<char*>(&width), imageUtils::twoBytes );

    // Vérifier la largeur de l'image

    uint16_t height;
    is.read( reinterpret_cast<char*>(&height), imageUtils::twoBytes );

    // Vérifier la hauteur de l'image

    uint8_t taille_octet = 0;
    is.read( reinterpret_cast<char*>(&taille_octet), imageUtils::oneByte );

    // Verification de la taille des pixels
    if ( taille_octet != 24 ) {
        throw invalidFormat( "Cette fonction ne prend en charge que des pixels de 24bits" );
    }

    // Verification du byte 17
    // Classe ByteDescTarga avec des fonctions retournant true ou false en fonction des bits
    std::bitset<8> desc;

    uint8_t descByteImage;
    is.read( reinterpret_cast<char*>(&descByteImage), imageUtils::oneByte );

    // Verification de la composition du byte 17
    if ( descByteImage ^ 0x0 ) {
        //
    }


    // Les canaux des pixels sont inversés (b,g,r)


    return createColorImage( 10, 10, 10 ).release();
}

// Scaler
ColorImage* ColorImage::simpleScale( const intmax_t newWidth, const intmax_t newHeight ) const {
    imageUtils::VERIFY::verifyWidth( newWidth, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );
    imageUtils::VERIFY::verifyHeight( newHeight, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );

    auto image = ColorImage::createColorImage( newWidth, newHeight, intensity_ );

    const auto ratioW = static_cast<long double>(width_) / newWidth;
    const auto ratioH = static_cast<long double>(height_) / newHeight;

    for ( uint16_t y = 0; y < newHeight; ++y ) {
        for ( uint16_t x = 0; x < newWidth; ++x ) {
            image->pixel( x, y ) = pixel( static_cast<uint16_t>(x * ratioW), static_cast<uint16_t>(y * ratioH)
            );
        }
    }

    return image.release();
}

ColorImage* ColorImage::bilinearScale( const intmax_t newWidth, const intmax_t newHeight ) const {
    // Peut simplifier la valeur de y2 et x2 car la division en bas vaut toujours 1
    // Car y2 = std::ceil(y) ou bien y2 = y1 + 1 = std::floor(y) + 1
    // Donc ratioY = (y -y1) / (y2 - y1) = y - y1
    imageUtils::VERIFY::verifyWidth( newWidth, imageUtils::VERIFY::Interval<Width>{0,imageUtils::maxWidth} );

    imageUtils::VERIFY::verifyHeight( newHeight, imageUtils::VERIFY::Interval<Height>{0,imageUtils::maxHeight} );

    std::vector<Color> pixels( static_cast<size_t>(newWidth * newHeight) );

    // The ratioW and ratioH was scale ratio between the new image and the old image
    const auto ratioW = static_cast<long double>(width_) / newWidth;
    const auto ratioH = static_cast<long double>(height_) / newHeight;

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
            pixels.at( static_cast<size_t>(xp + ( yp * newWidth )) ) =
                    ( ( 1 - ratioY ) * ( ( ( 1 - ratioX ) * p1 ) + ( ratioX * p3 ) ) ) +
                    ( ratioY * ( ( ( 1 - ratioX ) * p2 ) + ( ratioX * p4 ) ) );
        }
    }

    return new ColorImage(newWidth, newHeight, intensity_, std::move(pixels));
}
