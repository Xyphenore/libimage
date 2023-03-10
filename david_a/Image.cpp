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
#include <type_traits>

extern "C" {
#include <jpeglib.h>
}

const char* const identifier = "david_a";
const char* const informations = "Beaucoup de méthodes dépréciées, car j'ai fait des méthodes soit plus sécurisées,"
                                 "soit avec moins de paramètres et une meilleure couche d'abstraction\n"
                                 "Pas eu assez de temps pour faire plus\n"
                                 "Mon objectif de propreté serait tout le code contenu dans la classe GrayImage,"
                                 " qui est plus sécuritaire et propre\n"
                                 "Choix de conception : utilisation des intmax_t au lieu des uint8_t pour les paramètres,"
                                 " ainsi je permets aux utilisateurs de pouvoir entrer des entrées négatives et ainsi"
                                 " ils reçoivent des exceptions";




using alwaysData = std::runtime_error;
using invalidType = std::invalid_argument;
using invalidWidth = std::invalid_argument;
using invalidHeight = std::invalid_argument;
using invalidIntensity = std::invalid_argument;
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
    __attribute__((unused))
    static void activateExceptionsForFailBitOn( std::ios& ios ) {
        ios.exceptions( std::ios_base::failbit );
    }

    /// Activate the exception's throw for badbit at true, on an istream
    __attribute__((unused))
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

    VERIFY::verifyShade( r, VERIFY::Interval<Shade>{ 0, limit } );
    VERIFY::verifyShade( g, VERIFY::Interval<Shade>{ 0, limit } );
    VERIFY::verifyShade( b, VERIFY::Interval<Shade>{ 0, limit } );
}

// Definitions of Color
static Color operator+( const Color& c1, const Color& c2 ) {
    return { static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.r_, c2.r_)) ),
             static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.g_, c2.g_)) ),
             static_cast<uint8_t>( std::round(imageUtils::meanIfOver255( c1.b_, c2.b_)) )
    };
}
static Color operator*( const long double alpha, const Color& c ) {
    return { static_cast<Shade>(std::round( c.r_ * alpha )), static_cast<Shade>(std::round( c.g_ * alpha )),
             static_cast<Shade>(std::round( c.b_ * alpha )) };
}

static bool operator==( const Color& c1, const Color& c2 ) {
    return ( c1.r_ == c2.r_ ) && ( c1.g_ == c2.g_ ) && ( c1.b_ == c2.b_ );
}

static bool operator!=( const Color& c1, const Color& c2 ) {
    return !operator==( c1, c2 );
}
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
    imageUtils::VERIFY::verifyWidth( dim.width, imageUtils::VERIFY::Interval<Width>{ 0, imageUtils::maxWidth } );
    imageUtils::VERIFY::verifyHeight( dim.height, imageUtils::VERIFY::Interval<Height>{ 0, imageUtils::maxHeight } );

    imageUtils::VERIFY::verifyIntensity( intensity,
                                         imageUtils::VERIFY::Interval<Shade>{ 0, imageUtils::maxIntensity } );

    imageUtils::VERIFY::verifySizeArray( pixels_, dimension.width * dimension.height );

    for ( const auto pixel: pixels_ ) {
        VERIFY::verifyShade( pixel, VERIFY::Interval<Shade>{ 0, intensity_ } );
    }
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
            // On soustrait deux car on trace déjà deux pixels de la ligne suivante
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
// TODO Non opérationnel, impossible d'écrire en FULL ASCII
void GrayImage::writePGM( std::ostream& os, const Format::WRITE_IN f ) const {
    using Format = Format::WRITE_IN;
    const auto& imageDim = dimension;

    // Write u=in a fonction verifyFormat( format, goodformat list )
    if ( ( f != Format::ASCII ) && ( f != Format::BINARY ) ) {
        throw invalidFormat( "Unknown image format" );
    }

    for ( const auto pixel: pixels_ ) {
        VERIFY::verifyShade( pixel, VERIFY::Interval<Shade>{ 0, intensity_ } );
    }

    if ( f == Format::BINARY ) {
        // TODO Changer identifier par la variable environnement nom utilisateur
        os << "P5\n" << "# Image sauvegardée par " << ::identifier << '\n' << imageDim.width << '\n' << imageDim.height
           << '\n' << static_cast<uint16_t>(intensity_) << '\n';


        os.write( reinterpret_cast<const char*>(pixels_.data()),
                  static_cast<std::streamsize>(imageDim.width * imageDim.height * sizeof( Shade )) );
    }
    else {
        throw std::runtime_error("Le format full ascii n'est pas encore pris en charge");
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
    // Lecture du nombre magique pour identifier le type d'image
    auto p = new char[2];
    is.read( p, 2 );
    const std::string type( p );

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
        // Utiliser from_chars() en C++17

        for ( size_t i = 0; i < ( width * height ); ++i ) {
            auto tmp = 0;
            is >> tmp;
            pixels.at( i ) = static_cast<Shade>(tmp);
        }
    }


    for ( const auto pixel: pixels ) {
        VERIFY::verifyShade( pixel, VERIFY::Interval<Shade>{ 0, intensity } );
    }

    imageUtils::skip_ONEwhitespace( is );

    //imageUtils::verifyStreamContainData( is );

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
    imageUtils::VERIFY::verifyWidth( width, imageUtils::VERIFY::Interval<Width>{ 0, imageUtils::maxWidth } );
    imageUtils::VERIFY::verifyHeight( height, imageUtils::VERIFY::Interval<Height>{ 0, imageUtils::maxHeight } );
    imageUtils::ASCII::verifySizeArray( pixels_, width_ * height_ );
    imageUtils::VERIFY::verifyIntensity( intensity,
                                         imageUtils::VERIFY::Interval<Shade>{ 0, imageUtils::maxIntensity } );

    for ( const auto color: pixels_ ) {
        VERIFY::verifyColor( color, Color{ intensity_, intensity_, intensity_ } );
    }
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

    // Pas utile de vérifier le max, lors de la création de couleur on vérifie déjà
//    for ( const auto color : pixels_ ) {
//        VERIFY::verifyColor( color, Color{intensity_, intensity_, intensity_});
//    }

    if ( f == Format::BINARY ) {
        os << "P6\n" << "# Image sauvegardée par " << ::identifier << '\n' << width_ << " " << height_ << '\n'
           << static_cast<uint16_t>(intensity_) << '\n';

        os.write( reinterpret_cast<const char*>(pixels_.data()),
                  static_cast<std::streamsize>(width_ * height_ * sizeof( Color )) );
    }
    else {
        // Full ASCII format
        // TODO Corriger l'écriture en FULL ASCII
        throw std::runtime_error("La fonction ne permet pas d'écrire en full ascii");
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

    os << '\n';
}

void ColorImage::writeTGA( std::ostream& os, const Format::WRITE_IN f ) const {
    // oneByte 1 = 0 because no identification information for the image
    os.put( 0 );

    // oneByte 2 = 0 because no color map
    os.put( 0 );

    using Format = Format::WRITE_IN;
    if ( ( f != Format::NO_RLE ) && ( f != Format::RLE ) ) {
        throw invalidEnumTYPE( "Error the given format was unknown for this function" );
    }

    if ( f == Format::NO_RLE ) {
        // oneByte 3 = 2 because RGB no RLE
        os.put( 2 );
    }
    else {
        os.put( 10 );
    }

    // Colormap
    // oneByte 4,5 = 0
    os.put( 0 ).put( 0 );

    // oneByte 6,7 = 0
    os.put( 0 ).put( 0 );

    // oneByte 8 = 0 because no color map
    os.put( 0 );

    // Spec Image

    // oneByte 9,10 = X origin of image (lo-hi)
    os.put( 0 ).put( 0 );

    // oneByte 11,12 = Y origin of image (lo-hi)
    os.put( 0 ).put( 0 );

    // oneByte 13,14 = Image width
    os.write( reinterpret_cast<const char*>(&width_), 2 );

    // oneByte 15,16 = Image height
    os.write( reinterpret_cast<const char*>(&height_), 2 );

    // oneByte 17 = 24 because with use Targa 24 bits
    os.put( 24 );

    // Mettre la valeur à 0 pour que l'origine soit en bas à gauche
    // oneByte 18 = {  bits0-3 = 0000 because 24bits,
    //              bit4 = 0 because the origin is left
    //              bit5 = {0,1} its origin of image (lower left, upper left)
    //              bit6-7 = 00 because the written image is progressive
    const uint8_t byte18 = 0; // Because image start top left
    os.put( byte18 );

    // oneByte 18-size in oneByte 1 = Image identification field, but oneByte 1 = 0 so nothing
    // oneByte 18 - nothing, because no color map

    // oneByte 18 - size of Image = pixels
    // Vérifier la taille de la classe Color

    if ( f == Format::NO_RLE ) {
        std::vector<Color> swappedPixels( width_ * height_ );

        for ( size_t y = 0; y < height_; ++y ) {
            for ( size_t x = 0; x < width_; ++x ) {
                swappedPixels.at( ( y * width_ ) + x ) = pixels_.at( ( ( height_ - 1 - y ) * width_ ) + x );
            }
        }

        for ( size_t i = 0; i != static_cast<size_t>(width_ * height_); ++i ) {
            os.put( static_cast<char>(swappedPixels[i].b_) );
            os.put( static_cast<char>(swappedPixels[i].g_) );
            os.put( static_cast<char>(swappedPixels[i].r_) );
        }
    }
    else {
        // On swap les lignes pour pouvoir bien affiché un fichier commençant avec l'origine en bas à gauche
        std::vector<Color> swappedPixels( width_ * height_ );

        for ( size_t y = 0; y < height_; ++y ) {
            for ( size_t x = 0; x < width_; ++x ) {
                swappedPixels.at( ( y * width_ ) + x ) = pixels_.at( ( ( height_ - 1 - y ) * width_ ) + x );
            }
        }

        const size_t maxLength = width_ * height_;
        size_t index = 0;
        Color color;
        //Color currChunk[128];
        std::array<Color, 128> currChunk;

        while ( index < maxLength ) {
            bool isRle = false;
            color = swappedPixels[index];
            currChunk[0] = color;

            size_t rle = 1;

            while ( (index + rle) < maxLength ) {
                if ( (color != swappedPixels[index + rle]) || ( 128 == rle )) {
                    isRle = (rle > 1);
                    break;
                }
                ++rle;
            }

            if ( isRle ) {
                os.put(static_cast<char>(128 | (rle -1)) );
                os.put( static_cast<char>(color.b_) );
                os.put( static_cast<char>(color.g_) );
                os.put( static_cast<char>(color.r_) );
            }
            else {
                rle = 1;

                while ( (index + rle) < maxLength ) {
                    if ( ((color != swappedPixels[index + rle]) && (rle < 128)) || (rle < 3)) {
                        color = swappedPixels[index + rle];
                        currChunk[rle] = color;
                    }
                    else {
                        if ( color == swappedPixels[index + rle]) {
                            rle -= 2;
                        }
                        break;
                    }
                    ++rle;
                }

                os.put(static_cast<char>(rle - 1) );
                for ( size_t i =0; i < rle; ++i) {
                    color = currChunk[i];
                    os.put( static_cast<char>(color.b_) );
                    os.put( static_cast<char>(color.g_) );
                    os.put( static_cast<char>(color.r_) );
                }
            }

            index += rle;
        }

        os.flush();
    }


    // No dev or ext fields
    // Maybe add a basic footer
}

void ColorImage::writeJPEG( const char* output, const int quality ) const {
    // TODO L'écriture est figée, on ne gère très peu d'exceptions, et c'est l'exemple donnée dans les fichiers du tp
    std::vector<Shade> buffer(width_ * height_ * 3);
    for ( size_t i =0; i < (width_ * height_); ++i ) {
        auto x = i *3;
        buffer[x] = pixels_[i].r_;
        buffer[x+1] = pixels_[i].g_;
        buffer[x+2] = pixels_[i].b_;
    }

    FILE* outfile;
    if ( ( outfile = fopen( output, "wb" ) ) == nullptr ) {
        throw std::runtime_error( "Erreur dans l'ouverture du fichier de sorti" );
    }

    jpeg_compress_struct cinfo;

    // Activation de l'attrapeur des erreurs
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error( &jerr );

    // Initialisation de l'objet compression
    jpeg_create_compress( &cinfo );
    jpeg_stdio_dest( &cinfo, outfile );

    cinfo.image_width = width_;
    cinfo.image_height = height_;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults( &cinfo );

    jpeg_set_quality( &cinfo, quality, true );

    jpeg_start_compress( &cinfo, true );

    auto row_stride = width_ * 3;
    JSAMPROW row_pointer;

    while ( cinfo.next_scanline < cinfo.image_height ) {
        row_pointer = (JSAMPROW)&buffer[cinfo.next_scanline * row_stride];

        jpeg_write_scanlines( &cinfo, &row_pointer, 1 );
    }

    jpeg_finish_compress( &cinfo );

    // Fermeture du fichier de sortie
    fclose(outfile);

    jpeg_destroy_compress( &cinfo );
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
            auto r = 0;
            auto g = 0;
            auto b = 0;
            is >> r >> g >> b;

            pixels.at( i ).r_ = static_cast<Shade>(r);
            pixels.at( i ).g_ = static_cast<Shade>(g);
            pixels.at( i ).b_ = static_cast<Shade>(b);
        }
    }

    for ( const auto color: pixels ) {
        VERIFY::verifyColor( color, Color{ intensity, intensity, intensity } );
    }

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

    if ( ( nbBitsColor != 24 ) && ( nbBitsColor != 0 ) ) {
        throw invalidColor( "Just manage color with 24 bits" );
    }

    return { firstColor, countColor, nbBitsColor };
}

static bool holdColorMap( std::istream& is ) {
    // Other values than 0 or 1, throw exception
    char containeMap = 2;

    imageUtils::activateExceptionsOn( is );
    is.get( containeMap );

    if ( ( containeMap != 0 ) && ( containeMap != 1 ) ) {
        throw invalidColorMapType( "Bad read value of Color Map Type, please verify the given file" );
    }

    return ( 1 == containeMap );
}

ColorImage* ColorImage::readTGA( std::istream& is ) {
    // TODO Factoriser le code
    // TODO Rendre expressif le code avec des appels de fonctions -> abstraction

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
    if ( ( type != 2 ) && ( type != 1 ) ) {
        throw invalidFormat( "Only supported formats : 1,2" );
    }

    // 4 Spec de palette couleur
    const ColorMap colorMap = readColorMap( is );

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
    if ( ( taille_octet != 24 ) && ( taille_octet != 8 ) ) {
        throw invalidFormat( "Cette fonction ne prend en charge que des pixels de 24bits, de 8bits" );
    }

    // Verification du byte 17

    uint8_t descByteImage;
    is.read( reinterpret_cast<char*>(&descByteImage), imageUtils::oneByte );

    // Verification de la composition du byte 17
    if ( ( descByteImage & 0b0000'1111 ) != 0 ) {
        // throw exception only no alpha in 24bits
    }
    if ( ( descByteImage & 0b0001'0000 ) != 0 ) {
        // throw exception only left origin
    }
    if ( ( descByteImage & 0b1100'0000 ) != 0 ) {
        // throw exception on ne gère pas l'entrelacement
    }


    // Lecture du champ d'id
    std::string idField;
    if ( sizeIdentificationField > 0 ) {
        auto idFieldRAW = new char[sizeIdentificationField];
        is.read( idFieldRAW, sizeIdentificationField );

        idField = std::string( idFieldRAW );
        delete[] idFieldRAW;
    }


    std::vector<Color> tabColor( colorMap.countColor );
    if ( containColormap && ( 1 == type ) ) {
        // lecture de la colormap
        // Hard fixed to 24bits color
        is.read( reinterpret_cast<char*>(tabColor.data()),
                 static_cast<std::streamsize>(colorMap.countColor * sizeof( Color ) ) );
    }

    ColorImage* pimg = nullptr;

    std::vector<Color> pixels( width * height );
    if ( type == 2 ) {
        is.read( reinterpret_cast<char*>(pixels.data()),
                 static_cast<std::streamsize>(width * height * sizeof( Color )) );
        // Origin en haut
        if ( ( descByteImage & 0b0010'0000 ) == 0b0010'0000 ) {
            // on swap les lignes
            std::vector<Color> swappedPixels( width * height );

            for ( size_t y = 0; y < height; ++y ) {
                for ( size_t x = 0; x < width; ++x ) {
                    swappedPixels.at( ( y * width ) + x ) = pixels.at( ( ( height - 1 - y ) * width ) + x );
                }
            }

            pimg = new ColorImage( width, height, maxIntensity, std::move( swappedPixels ) );
        }
        else {
            pimg = new ColorImage( width, height, maxIntensity, std::move( pixels ) );
        }
    }
    else if ( type == 1 ) {
        if ( tabColor.empty() ) {
            throw invalidColorMapType( "Error tab Color is nullptr" );
        }

        for ( size_t i = 0; i < ( width * height ); ++i ) {
            uint8_t c = 0;
            is.read( reinterpret_cast<char*>(&c), 1 );

            pixels.at( i ) = tabColor[static_cast<size_t>(c)];
        }

        if ( ( descByteImage & 0b0010'0000 ) == 0b0010'0000 ) {
            // on swap les lignes
            std::vector<Color> swappedPixels( width * height );

            for ( size_t y = 0; y < height; ++y ) {
                for ( size_t x = 0; x < width; ++x ) {
                    swappedPixels.at( ( y * width ) + x ) = pixels.at( ( ( height - 1 - y ) * width ) + x );
                }
            }

            pimg = new ColorImage( width, height, maxIntensity, std::move( swappedPixels ) );
        }
        else {
            pimg = new ColorImage( width, height, maxIntensity, std::move( pixels ) );
        }
    }

    return pimg;
}

ColorImage* ColorImage::readJPEG( const char* input ) {
    FILE* inputFile;
    if ( (inputFile = fopen(input, "rb")) == nullptr ) {
        throw std::runtime_error("Erreur lors de l'ouverture du fichier input");
    }

    struct jpeg_decompress_struct cinfo;

    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inputFile);

    jpeg_read_header(&cinfo, true);

    jpeg_start_decompress(&cinfo);

    auto rowstride = cinfo.output_width * cinfo.output_components;

    std::vector<Shade> buffer( cinfo.image_height * rowstride);
    JSAMPROW row_pointer;

    while ( cinfo.output_scanline < cinfo.output_height ) {
        row_pointer = (JSAMPROW)&buffer[cinfo.output_scanline * rowstride];

        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);

    std::vector<Color> array(cinfo.image_width * cinfo.image_height);
    for ( size_t i = 0; i < (cinfo.image_height * cinfo.image_width); ++i) {
        auto x = i*3;

        array[i] = Color(buffer[x], buffer[x+1], buffer[x+2]);
    }


    auto image = new ColorImage( cinfo.image_width, cinfo.image_height, 255, std::move(array));

    jpeg_destroy_decompress(&cinfo);
    fclose(inputFile);

    return image;
}

ColorImage* ColorImage::readMaison2( std::istream& is ) {
    // Vérification du type
    auto p = std::make_unique<char>(7);
    is.read( p.get(), 7);
    const std::string type(p.release());

    if ( type != "Maison2" ) {
        throw invalidType("The given file is not of type Maison2");
    }

    // Lecture de la taille de la zone commentaire
    uint8_t sizeComments = 0;
    is.read( reinterpret_cast<char*>(&sizeComments), oneByte);

    // Lecture de la hauteur
    uint8_t phHeight = 0;
    // Lecture du poids fort
    is.read( reinterpret_cast<char*>(&phHeight), oneByte);

    // Lecture du poids faible
    uint8_t pfHeight = 0;
    is.read( reinterpret_cast<char*>(&pfHeight), oneByte);

    // Merci https://stackoverflow.com/questions/6090561/how-to-use-high-and-low-bytes/6090641
    const uint16_t height = (pfHeight | (phHeight << 8));

    // Lecture de la largeur
    uint8_t phWidth = 0;
    is.read( reinterpret_cast<char*>(&phWidth), oneByte);

    uint8_t pfWidth = 0;
    is.read( reinterpret_cast<char*>(&pfWidth), oneByte);

    const uint16_t width = (pfWidth | (phWidth << 8));

    // Récupération du commentaire et affichage
    auto str = new char[sizeComments];
    if ( sizeComments > 0 ) {
        is.read( str, sizeComments );
        const std::string comment( str );

        std::cout << comment << '\n';
    }
    delete[] str;

    // La suite est des pixels
    std::vector<Color> pixels(width * height);

    std::cout << "green\n";

    // Lecture des pixels verts
    for ( size_t i = 0; i < (width * height); ++i ) {
        uint8_t green = 0;
        is.read( reinterpret_cast<char*>(&green), oneByte);
        pixels.at(i).g_ = green;
    }

    std::cout << "blue\n";

    // Lecture des pixels bleus
    for ( size_t i = 0; i < (width * height); ++i ) {
        uint8_t blue = 0;
        is.read( reinterpret_cast<char*>(&blue), oneByte);
        pixels.at(i).b_ = blue;
    }

    std::cout << "red\n";

    // Lecture des pixels rouge
    for ( size_t i = 0; i < (width * height); ++i ) {
        uint8_t red = 0;
        is.read( reinterpret_cast<char*>(&red), oneByte);
        pixels.at(i).r_ = red;
    }

    std::cout << "fin\n";

    return new ColorImage(width, height, maxIntensity, std::move(pixels));
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

void ColorImage::Behensem2Octants( intmax_t x1, intmax_t y1, intmax_t x2, intmax_t y2, Color color ) {
    auto x = x1;
    auto y = y1;

    auto longX = x2 - x1;
    auto longY = y2 - y1;

    if ( longY < longX ) { // 1er Octant
        const auto c1 = 2 * ( longY - longX );
        const auto c2 = 2 * longY;

        auto critere = c2 - longX;

        while ( x <= x2 ) {
            pixel( x, y ) = color;
            if ( critere >= 0 ) { // changement de ligne horizontale
                y++;
                critere = critere + c1;
            }
            else
                // toujours la même ligne horizontale
                critere = critere + c2;

            x++; // ligne suivante, et recommence
        }
    }
    else { // 2eme Octant
        const auto c1 = 2 * ( longX - longY );
        const auto c2 = 2 * longX;

        auto critere = c2 - longY;

        while ( y <= y2 ) {
            pixel( x, y ) = color;
            if ( critere >= 0 ) { // changement de ligne verticale
                x++;
                critere = critere + c1;
            }
            else
                // toujours la même ligne verticale
                critere = critere + c2;
            y++; // ligne suivante, et recommence
        }

    }
}

ColorImage* ColorImage::anaglyphe() const {
    const size_t demiWidth = width_ / 2;

    std::vector<Color> pixels(demiWidth * height_);

    for ( size_t y = 0; y < height_; ++y ) {
        // On navigue dans la partie gauche de l'image
        for ( size_t x = 0; x < demiWidth; ++x ) {
            // On supprime le canal rouge
            Color color = pixel( static_cast<intmax_t>(x),
                                 static_cast<intmax_t>(y));
            color.r_ = 0;
            pixels.at( (y * demiWidth) + x) = color;
        }

        // On navigue dans la partie droite de l'image
        for ( size_t x = demiWidth; x < width_; ++x ) {
            // On supprime les canaux vert et bleu
            Color color = pixel( static_cast<intmax_t>(x),
                                 static_cast<intmax_t>(y));
            color.g_ = 0;
            color.b_ = 0;
            pixels.at( (y * demiWidth) + (x-demiWidth)) = pixels.at( (y * demiWidth) + (x-demiWidth)) + color;
        }
    }

    return new ColorImage(demiWidth, height_, maxIntensity, std::move(pixels));
}
