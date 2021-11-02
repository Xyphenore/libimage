#ifndef DAVID_A_IMAGE_HPP
#define DAVID_A_IMAGE_HPP

// Options de correction
#define CORR_PGMASCII //P2
#define CORR_PPMASCII //P3
#define CORR_READCOLORJPEG
#define CORR_WRITECOLORJPEG
//#define CORR_READCOLORTGARLE
// #define CORR_BRESENHAM
// #define CORR_TEMPLATE

#include <iostream>
#include <vector>
#include <limits>
#include <memory>

extern const char* const identifier;
extern const char* const informations;

using Width = uint16_t;
using Height = uint16_t;
using Shade = uint8_t;


namespace imageUtils {
    enum class BOOLEAN_TYPE {
        YES, NO
    };

    /// Enumeration that indicates whether a shape should be drawn filled or empty
    using FILL = BOOLEAN_TYPE;

    /// Enumeration that indicates if a line should be drawn horizontally or vertically
    enum class TYPE {
        HORIZONTAL, VERTICAL
    };

    /// Represents a point with two coordinates
    struct Point {
        intmax_t x;
        intmax_t y;
    };

    /// A Pixel is a Point with two coordinates
    using Pixel = Point;

    /// Represents a dimension of an image, where the width and the height can be of two different types
    template <typename TWidth = intmax_t, typename THeight = intmax_t>
    struct Dimension {
        TWidth width;
        THeight height;
    };
}

namespace Format {
    /// Represents the format of the written image
    /// \warning ASCII and BINARY is to write PNM images
    /// \warning NO_RLE and RLE is to write TARGA images
    enum class WRITE_IN {
        ASCII, BINARY, NO_RLE, RLE
    };

    // Ce qui suit n'a pas été utilisé, il aurait dû servir à rendre le code plus clean
    /// Represents different PNM formats can be read
    /// More information : http://netpbm.sourceforge.net/doc/pnm.html
    /// \note This list was created in 2021, it can be modified in the future
    /// \warning Only PPM and PGM can be calculated
    enum class PNM {
        PBM_ASCII = 1, PGM_ASCII = 2, PPM_ASCII = 3, PBM_BINARY = 4, PGM_BINARY = 5, PPM_BINARY = 6, PAM = 7
    };

    /// Represents different TARGA formats can be read
    /// More information :
    /// \note This list don't contain all TARGA formats, it can be modified in the future
    /// \warning Only these formats can be written : UNCOMPRESSED_TRUE_COLOR(24bits), RLE_TRUE_COLOR(24bits)
    /// \warning Only these formats can be read : UNCOMPRESSED_TRUE_COLOR(24bits), UNCOMPRESSED_COLOR_MAP(24bits)
    enum class TARGA {
        NO_IMAGE = 0,
        UNCOMPRESSED_COLOR_MAP = 1,
        UNCOMPRESSED_TRUE_COLOR = 2,
        UNCOMPRESSED_BLACK_WHITE = 3,
        RLE_COLOR_MAP = 9,
        RLE_TRUE_COLOR = 10,
        RLE_BLACK_WHITE = 11,
        HUFFMAN_DELTA_RLE_COLOR_MAP = 32,
        HUFFMAN_DELTA_RLE_4PASSTREE_COLOR_MAP = 33,
    };
}


// TODO Mettre à jour les exceptions
/// Can create or build a gray Image in 2D format, where the intensity or depth is coded on one byte
/// \warning The maximum of : width = max of uint16_t and height = max of uint16_t
/// \warning The maximum of : intensity = max of uint8_t
/// \warning GrayImage can only be read or write in PGM
class GrayImage {
public:
    /// \warning Can't build a grayImage without giving width and height
    GrayImage() = delete;

    /// Build a grayImage with the given width, height and the default intensity
    /// The built image was colored with the default Color
    /// \deprecated Please use a constructor with the structure Dimension
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \post getWidth == given width
    /// \post getHeight == given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception std::bad_alloc if the memory allocation fails
    // [[deprecated ("Please use a constructor with the structure Dimension")]]
    GrayImage( intmax_t width, intmax_t height );

    /// Build a grayImage with the given Dimension(width, height) and the default intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \post getWidth == given width
    /// \post getHeight == given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception std::bad_alloc if the memory allocation fails
    explicit GrayImage( imageUtils::Dimension<> dim );

    /// Build a grayImage with the given width, height and intensity
    /// The built image was colored with the default Color
    /// \deprecated Please use a constructor with the structure Dimension
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception std::bad_alloc if the memory allocation fails
    // [[deprecated ("Please use a constructor with the structure Dimension")]]
    GrayImage( intmax_t width, intmax_t height, intmax_t intensity );

    /// Build a grayImage with the given Dimension(width, height) and intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception std::bad_alloc if the memory allocation fails
    explicit GrayImage( imageUtils::Dimension<> dim, intmax_t intensity );

    /// Build a grayImage by copying the given image
    /// \pre The given image needs to be good built
    /// \post The built image was same of the given image
    /// \exception bad_alloc if the memory allocation fails
    GrayImage( const GrayImage& src ) = default;

    /// Build a grayImage by moving the given image
    /// \pre The given image needs to be good built
    /// \post The built image was same of the given image
    GrayImage( GrayImage&& src ) noexcept = default;

    /// Delete the given grayImage
    ~GrayImage() noexcept = default;


    /// \warning Can't affect another image to a built image
    GrayImage& operator=( const GrayImage& ) = delete;

    /// \warning Can't affect another image to a built image
    GrayImage& operator=( GrayImage&& ) noexcept = delete;


    /// \return A constant reference of the width of Image
    /// \warning Please don't assign the return reference to a variable with a larger range
    /// \deprecated If you assign the return reference to a variable with a larger range
    /// \deprecated And the instance died, you have a dead reference to a memory, and a bug occurred
    /// \deprecated So please use width()
    // [[deprecated ("Please use the method width(), it returns a copy of width, it's more secure")]]
    const Width& getWidth() const noexcept;

    /// \return A constant reference of the height of Image
    /// \warning Please don't assign the return reference to a variable with a larger range
    /// \deprecated If you assign the return reference to a variable with a larger range
    /// \deprecated And the instance died, you have a dead reference to a memory, and a bug occurred
    /// \deprecated So please use height()
    // [[deprecated ("Please use the method height(), it returns a copy of height, it's more secure")]]
    const Height& getHeight() const noexcept;

    /// \return A copy of width of Image
    Width width() const noexcept;

    /// \return A copy of height of Image
    Height height() const noexcept;


    /// \return A reference of the pixel at the position x,y
    /// \warning Please don't assign the return reference to a variable with a larger range
    /// \deprecated If you assign the return reference to a variable with a larger range
    /// \deprecated And the instance died, you have a dead reference to a memory, and a bug occurred
    /// \deprecated So please use setPixel( Pixel, Color )
    /// \param[in] position x,y
    /// \pre Coordinate x of Pixel needs to be in the range of [0; width[
    /// \pre Coordinate y of Pixel needs to be in the range of [0; height[
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \warning Don't verify the value affected in the selected pixel
    // [[deprecated ("Please use the method setPixel( pixel, color )")]]
    Shade& pixel( intmax_t x, intmax_t y );

    /// \return A constant reference of the pixel at the position x,y
    /// \warning Please don't assign the return reference to a variable with a larger range
    /// \deprecated If you assign the return reference to a variable with a larger range
    /// \deprecated And the instance died, you have a dead reference to a memory, and a bug occurred
    /// \deprecated So please use getPixel( Pixel )
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    // [[deprecated ("Please use the method getPixel( pixel )")]]
    const Shade& pixel( intmax_t x, intmax_t y ) const;

    /// Modify the pixel with the given color
    /// \param[in] Pixel( x,y )
    /// \pre Coordinate x of Pixel needs to be in the range of [0; width[
    /// \pre Coordinate y of Pixel needs to be in the range of [0; height[
    /// \pre The given color needs to be in the range of [0; intensity]
    /// \post The requested pixel has the given color
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \exception invalidColor if the given color is not in the range of [0; intensity]
    void setPixel( imageUtils::Pixel px, intmax_t color );

    /// \return A value of the pixel at the position x,y
    /// \param[in] Pixel( x,y )
    /// \pre Coordinate x of Pixel needs to be in the range of [0; width[
    /// \pre Coordinate y of Pixel needs to be in the range of [0; height[
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    Shade getPixel( imageUtils::Pixel px ) const;


    /// Clear the image, so the image was fill by the default Color
    void clear();

    /// Clear the image, so fill the image with the given color
    /// \deprecated Please use the method fill, it has more sense
    /// \pre The given color needs to be in gray's shades in [0, maxIntensity]
    /// \post The image fills with the given color
    // [[deprecated ("Please use the method fill, it has more sense")]]
    void clear( intmax_t color );

    /// Fill the image with the given color
    /// \pre Needs a color in gray's shades so in [0, maxIntensity]
    /// \post The same image but fill with the given color
    void fill( intmax_t color );


    /// Draw a rectangle with the given width, height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \deprecated Please use the method drawRectangle with two objects Point and Dimension
    /// \deprecated Please use drawRectangle(Point, Dimension)
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : (given x + given width) needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : (given y + given height) needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect precondition : (given x + given width) needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect precondition : (given y + given height) needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    // [[deprecated ("Please use drawRectangle(Point, Dimension)")]]
    void rectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height );


    /// Draw a rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the given color
    /// \deprecated Please use the method drawRectangle with two objects Point and Dimension
    /// \deprecated Please use drawRectangle(Point, Dimension, color)
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height[
    /// \pre The given color needs to be a gray's shade in [0; image's intensity]
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect this : given x + given width needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect this : given y + given height needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    // [[deprecated ("Please use drawRectangle(Point, Dimension, color)")]]
    void rectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height, intmax_t color );


    /// Draw a filled rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \deprecated Please use the method drawRectangle with two objects Point and Dimension
    /// \deprecated Please use drawRectangle(Point, Dimension, imageUtils::TYPE::FILLED)
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect this : given x + given width needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect this : given y + given height needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    // [[deprecated ("Please use drawRectangle(Point, Dimension, imageUtils::TYPE::FILLED)")]]
    void fillRectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height );


    /// Draw a filled rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the given color
    /// \deprecated Please use the method drawRectangle with two objects Point and Dimension
    /// \deprecated Please use drawRectangle(Point, Dimension, color, imageUtils::TYPE::FILLED)
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height[
    /// \pre The given color needs to be a gray's shade in [0; image's intensity]
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect this : given x + given width needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect this : given y + given height needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    // [[deprecated ("Please use drawRectangle(Point, Dimension, color, imageUtils::TYPE::FILLED")]]
    void fillRectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height, intmax_t color );

    /// Draw a rectangle with the given Dimension(width, height) and with it top left corner Point(x,y)
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : (given x + given width) needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : (given y + given height) needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect precondition : (given x + given width) needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect precondition : (given y + given height) needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    void drawRectangle( imageUtils::Point start, imageUtils::Dimension<> rectangleDim );

    /// Draw a rectangle with the given Dimension(width, height) and with it top left corner Point(x,y)
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : (given x + given width) needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : (given y + given height) needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect precondition : (given x + given width) needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect precondition : (given y + given height) needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    void drawRectangle( imageUtils::Point start, imageUtils::Dimension<> rectangleDim, intmax_t color );

    // TODO MAJ
    /// Draw a rectangle with the given Dimension(width, height) and with it top left corner Point(x,y)
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \warning Draw a filled rectangle if TYPE::FILLED is specified
    /// \warning Draw an empty rectangle if TYPE::EMPTY is specified
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : (given x + given width) needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : (given y + given height) needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect precondition : (given x + given width) needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect precondition : (given y + given height) needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception imageUtils::invalidEnumTYPE if the given TYPE was different from [TYPE::FILLED/TYPE::EMPTY]
    void drawRectangle( imageUtils::Point start, imageUtils::Dimension<> rectangleDim, imageUtils::FILL filled );

    // TODO MAJ
    /// Draw a rectangle with the given Dimension(width, height) and with it top left corner Point(x,y)
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \warning Draw a filled rectangle if TYPE::FILLED is specified
    /// \warning Draw an empty rectangle if TYPE::EMPTY is specified
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : (given x + given width) needs to be in ]0; image's width[
    /// \pre The given height needs to respect this : (given y + given height) needs to be in ]0; image's height[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not respect precondition : (given x + given width) needs to be in ]0; image's width[
    /// \exception invalidHeight if height does not respect precondition : (given y + given height) needs to be in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    /// \exception imageUtils::invalidEnumTYPE if the given TYPE was different from [TYPE::FILLED/TYPE::EMPTY]
    void drawRectangle( imageUtils::Point start, imageUtils::Dimension<> rectangleDim,
                        intmax_t color, imageUtils::FILL filled );


    /// Draw a 1 pixel of thickness line, in default Color, with the given length, and it left point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's width - x[
    /// \pre type needs to be TYPE::HORIZONTAL or TYPE::VERTICAL
    /// \post The same image with the desired line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's width - x[
    /// \exception imageUtils::invalidEnumTYPE if the type is different from TYPE::HORIZONTAL or TYPE::VERTICAL
    void drawLine( imageUtils::Point start, intmax_t length, imageUtils::TYPE type );

    /// Draw a 1 pixel of thickness line, in the given Color, with the given length, and its top point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's height - y[
    /// \pre color needs to be a gray's shade in [0, image's intensity]
    /// \pre type needs to be TYPE::HORIZONTAL or TYPE::VERTICAL
    /// \post The same image with the desired line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's height - y[
    /// \exception invalidColor if color does not in [0, image's intensity]
    /// \exception imageUtils::invalidEnumTYPE if the type is different from TYPE::HORIZONTAL or TYPE::VERTICAL
    void drawLine( imageUtils::Point start, intmax_t length, intmax_t color, imageUtils::TYPE type );


    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of simple scale
    /// \warning You have the responsibility of the created image
    /// \deprecated Please use simpleScale( Dimension ), is more safe because the function return a unique_ptr
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    /// \exception std::bad_alloc if the new image is too large to store
    // [[deprecated ("Please use simpleScale( Dimension ), is more safe because the function return a unique_ptr")]]
    GrayImage* simpleScale( intmax_t newWidth, intmax_t newHeight ) const;

    /// Created the same image of called image, but scale to Dimension(newWidth,newHeight), with the algorithm of simple scale
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    /// \exception std::bad_alloc if the new image is too large to store
    std::unique_ptr<GrayImage> simpleScale( imageUtils::Dimension<> newDim ) const;


    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of bilinear scale
    /// \warning You have the responsibility of the created image
    /// \deprecated Please use bilinearScale( Dimension ), is more safe because the function return a unique_ptr
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    /// \exception std::bad_alloc if the new image is too large to store
    // [[deprecated ("Please use bilinearScale( Dimension ), is more safe because the function return a unique_ptr")]]
    GrayImage* bilinearScale( intmax_t newWidth, intmax_t newHeight ) const;

    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of bilinear scale
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    /// \exception std::bad_alloc if the new image is too large to store
    std::unique_ptr<GrayImage> bilinearScale( imageUtils::Dimension<> newDim ) const;


    // TODO MAJ DESC
    /// Write in the given output stream the called image in the P5 format
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePGM( std::ostream& os ) const;

    // TODO MAJ DESC
    /// Write in the given output stream the called image in the given format {ASCII or BINARY}
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePGM( std::ostream& os, Format::WRITE_IN f ) const;

    // TODO MAJ DESC
    /// Read the given input stream and create a gray Image
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \warning You have the responsibility to manage and delete the created image
    /// \warning After we read value of pixels, the stream needs to be empty
    /// \return A raw pointe to the built image
    /// \pre The given stream needs to respect the Px format and this case the P2 or P5
    /// \pre The width in the input stream needs to be in ]0; maxWidth]
    /// \pre The height in the input stream needs to be in ]0; maxHeight]
    /// \pre The intensity in the input stream needs to be in ]0; maxIntensity]
    /// \pre The value of pixels needs to be in the range of [0,intensity]
    /// \pre The representation of pixels needs to have a length equals to width * height
    /// \post An image sized to width on height, with depth equals to intensity, and pixels equals to value of pixels in the stream
    /// \exception invalidType if the type of format don't match with "P2" or "P5"
    /// \exception invalidWidth if the width in the stream was not in [0, maxWidth]
    /// \exception invalidHeight if the height in the stream was not in ]0; maxHeight]
    /// \exception invalidIntensity if the intensity in the stream was not in [0, maxIntensity]
    /// \exception invalidPixels if any of read pixels is not in the range of [0, intensity]
    /// \exception std::bad_alloc if the memory allocation failed
    /// \exception alwaysData if the stream always contains data
    /// \exception invalidSizeRepresentationPixel if the END OF STREAM was encountered before the reach width * height pixels
    // [[deprecated ("Please use the method readPGM_secured who return an unique_ptr")]]
    static GrayImage* readPGM( std::istream& is );

    /// Read the given input stream and create a gray Image
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \warning After we read value of pixels, the stream needs to be empty
    /// \returns Return a unique_ptr to the created GrayImage
    /// \pre The given stream needs to respect the Px format and this case the P2 or P5
    /// \pre The width in the input stream needs to be in ]0; maxWidth]
    /// \pre The height in the input stream needs to be in ]0; maxHeight]
    /// \pre The intensity in the input stream needs to be in ]0; maxIntensity]
    /// \pre The value of pixels needs to be in the range of [0,intensity]
    /// \pre The representation of pixels needs to have a length equals to width * height
    /// \post An image sized to width on height, with depth equals to intensity, and pixels equals to value of pixels in the stream
    /// \exception invalidType if the type of format don't match with "P2" or "P5"
    /// \exception invalidWidth if the width in the stream was not in [0, maxWidth]
    /// \exception invalidHeight if the height in the stream was not in ]0; maxHeight]
    /// \exception invalidIntensity if the intensity in the stream was not in [0, maxIntensity]
    /// \exception invalidPixels if any of read pixels is not in the range of [0, intensity]
    /// \exception std::bad_alloc if the memory allocation failed
    /// \exception alwaysData if the stream always contains data
    /// \exception invalidSizeRepresentationPixel if the END OF STREAM was encountered before the reach width * height pixels
    static std::unique_ptr<GrayImage> readPGM_secured( std::istream& is );


private:
    const imageUtils::Dimension<Width, Height> dimension;
    const Shade intensity_{ defaultIntensity };

    std::vector<Shade> pixels_;


    /// The color black in shade of gray
    static const Shade black;

    /// The default color for grayImage is black
    static const Shade& defaultColor;

    /// The default intensity
    static const Shade defaultIntensity;

    /// Build a gray Image with the given width and height, intensity and the vector of shades
    /// \warning This builder move the given vector
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the given vector, so same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidSizeArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    /// \exception std::bad_alloc if the memory allocation fails
    explicit GrayImage( imageUtils::Dimension<> dim, intmax_t intensity, std::vector<Shade>&& pixels );

    /// Build a gray Image with the given Dimension(width,height), intensity and the vector of shades
    /// \warning This builder move the given vector
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the given vector, so same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidSizeArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    /// \exception std::bad_alloc if the memory allocation fails
    static std::unique_ptr<GrayImage>
    createGrayImage( imageUtils::Dimension<> dim, intmax_t intensity, std::vector<Shade>&& pixels );
};

/// This class is for create a rgb color
class Color {
public:
    /// Build a default color, so a black color (0,0,0)
    Color() = default;

    /// Build a color with the given red, green and blue shade
    /// \thorw invalidColor if one shade is above the maxIntensity aka 255, and if one shade is below 0
    Color( intmax_t r, intmax_t g, intmax_t b );

    Color( const Color& ) = default;

    Color( Color&& ) noexcept = default;

    ~Color() noexcept = default;

    Color& operator=( const Color& ) = default;

    Color& operator=( Color&& ) noexcept = default;

    Shade r_{ 0 };
    Shade g_{ 0 };
    Shade b_{ 0 };
};


/// Can create or build a gray Image in 2D format, where the intensity or depth is coded on one byte
/// \warning The maximum of : width = max of uint16_t and height = max of uint16_t
/// \warning The maximum of : intensity = max of uint8_t
class ColorImage {
public:
    /// \warning Can't build a grayImage without giving width and height
    ColorImage() = delete;

    /// Build a grayImage with the given width, height and the default intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception std::bad_alloc if the memory allocation fails
    ColorImage( intmax_t width, intmax_t height );

    /// Build a grayImage with the given width, height and intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception std::bad_alloc if the memory allocation fails
    ColorImage( intmax_t width, intmax_t height, intmax_t intensity );

    /// Build a grayImage by copying the given image
    /// \pre The given image needs to be good built
    /// \post The built image was same of the given image
    /// \exception bad_alloc if the memory allocation fails
    ColorImage( const ColorImage& src ) = default;

    /// Build a grayImage by moving the given image
    /// \pre The given image needs to be good built
    /// \post The built image was same of the given image
    ColorImage( ColorImage&& src ) noexcept = default;

    /// Delete the given grayImage
    ~ColorImage() noexcept = default;


    /// \warning Can't affect another image to a built image
    ColorImage& operator=( const ColorImage& ) = delete;

    /// \warning Can't affect another image to a built image
    ColorImage& operator=( ColorImage&& ) noexcept = delete;


    /// \return A constant reference of the width of Image
    const Width& getWidth() const noexcept;

    /// \return A constant reference of the height of Image
    const Height& getHeight() const noexcept;


    /// \return A reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \warning Don't verify the value affected in the selected pixel
    Color& pixel( intmax_t x, intmax_t y );

    /// \return A constant reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    const Color& pixel( intmax_t x, intmax_t y ) const;



    /// Clear the image, so the image was fill by the default Color
    void clear();

    /// Clear the image, so fill the image with the given color
    /// \deprecated Please use the method fill, it has more sense
    /// \pre The given color needs to be in gray's shades in [0, maxIntensity]
    /// \post The image fills with the given color
    // [[ deprecated ("Please use the method fill, it has more sense") ]]
    void clear( Color color );

    /// Fill the image with the given color
    /// \pre Needs a color in gray's shades so in [0, maxIntensity]
    /// \post The same image but fill with the given color
    void fill( Color color );



    /// Draw a rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width - given x[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height - given y[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not in ]0; image's width[
    /// \exception invalidHeight if height does not in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    void rectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height );

    /// Draw a rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the given color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width - given x[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height - y[
    /// \pre The given color needs to be a gray's shade in [0; image's intensity]
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not in ]0; image's width[
    /// \exception invalidHeight if height does not in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    void rectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height, Color color );

    /// Draw a filled rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the default Color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width - given x[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height - y[
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not in ]0; image's width[
    /// \exception invalidHeight if height does not in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    void fillRectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height );

    /// Draw a filled rectangle with the given width and height and with it top left corner at the position x,y
    /// The drawn rectangle has a thickness of 1 pixel and the given color
    /// \pre The given coordinate x needs to be in [0, image's width[
    /// \pre The given coordinate y needs to be in [0, image's height[
    /// \pre The given width needs to respect this : given x + given width needs to be in ]0; image's width - given x[
    /// \pre The given height needs to respect this : given y + given height needs to be in ]0; image's height - y[
    /// \pre The given color needs to be a gray's shade in [0; image's intensity]
    /// \post The same given image with the desired rectangle
    /// \exception invalidWidth if width does not in ]0; image's width[
    /// \exception invalidHeight if height does not in ]0; image's height[
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidColor if color does not in [0; image's intensity]
    void fillRectangle( intmax_t x, intmax_t y, intmax_t width, intmax_t height, Color color );


    /// Draw a 1 pixel of thickness horizontal line, in default Color, with the given length, and it left point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's width - x[
    /// \post The same image with the desired horizontal line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's width - x[
    void horizontalLine( intmax_t x, intmax_t y, intmax_t length );

    /// Draw a 1 pixel of thickness horizontal line, in the given color, with the given length, and it left point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's width - x[
    /// \pre color needs to be a gray's shade in [0, image's intensity]
    /// \post The same image with the desired horizontal line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's width - x[
    /// \exception invalidColor if color does not in [0, image's intensity]
    void horizontalLine( intmax_t x, intmax_t y, intmax_t length, Color color );

    /// Draw a 1 pixel of thickness vertical line, in default Color, with the given length, and its top point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's height - y[
    /// \post The same image with the desired vertical line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's height - y[
    void verticalLine( intmax_t x, intmax_t y, intmax_t length );

    /// Draw a 1 pixel of thickness vertical line, in the given Color, with the given length, and its top point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's height - y[
    /// \pre color needs to be a gray's shade in [0, image's intensity]
    /// \post The same image with the desired vertical line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's height - y[
    /// \exception invalidColor if color does not in [0, image's intensity]
    void verticalLine( intmax_t x, intmax_t y, intmax_t length, Color color );


    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of simple scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    ColorImage* simpleScale( intmax_t newWidth, intmax_t newHeight ) const;

    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of bilinear scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    ColorImage* bilinearScale( intmax_t newWidth, intmax_t newHeight ) const;



    /// Write in the given output stream the called image in the P5 format
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePPM( std::ostream& os ) const;

    /// Write in the given output stream the called image in the given format {ASCII or BINARY}
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePPM( std::ostream& os, Format::WRITE_IN f ) const;

    /// Write in the given output stream the called image in the format TARGA
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeTGA( std::ostream& os ) const;

    /// Write in the given output stream the called image in the format TARGA
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    /// \deprecated Use writeTGA( std::ostream& os, Format::WRITE_IN f ), have more sense
    // [[deprecated ("Use writeTGA( std::ostream& os, Format::WRITE_IN f ), have more sense")]]
    void writeTGA( std::ostream& os, bool rle ) const;

    /// Write in the given output stream the called image in the format TARGA
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeTGA( std::ostream& os, Format::WRITE_IN f ) const;

    /// Write in the given output stream the called image in the format JPEG
    /// \note Check the representation of JPEG format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeJPEG( const char* output ) const;

    // JPEG_LIB Attend que quality soit un int
    /// Write in the given output stream the called image in the format JPEG
    /// \note Check the representation of JPEG format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeJPEG( const char* output, int quality ) const;


    /// Read the given input stream and create a gray Image
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \warning You have the responsibility to manage and delete the created image
    /// \warning After we read value of pixels, the stream needs to contain any data
    /// \return A raw pointe to the built image
    /// \pre The given stream needs to respect the Px format and this case the P2 or P5
    /// \pre The width in the input stream needs to be in ]0; maxWidth]
    /// \pre The height in the input stream needs to be in ]0; maxHeight]
    /// \pre The intensity in the input stream needs to be in ]0; maxIntensity]
    /// \pre The value of pixels needs to be in the range of [0,intensity]
    /// \pre The representation of pixels needs to have a length equals to width * height
    /// \post An image sized to width on height, with depth equals to intensity, and pixels equals to value of pixels in the stream
    /// \exception invalidType if the type of format don't match with "P2" or "P5"
    /// \exception invalidWidth if the width in the stream was not in [0, maxWidth]
    /// \exception invalidHeight if the height in the stream was not in ]0; maxHeight]
    /// \exception invalidIntensity if the intensity in the stream was not in [0, maxIntensity]
    /// \exception invalidPixels if any of read pixels is not in the range of [0, intensity]
    /// \exception std::bad_alloc if the memory allocation failed
    /// \exception alwaysData if the stream always contains data
    /// \exception invalidSizeRepresentationPixel if the END OF STREAM was encountered before the reach width * height pixels
    static ColorImage* readPPM( std::istream& is );

    /// Read the given input stream and create a gray Image
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \warning You have the responsibility to manage and delete the created image
    /// \warning After we read value of pixels, the stream needs to contain any data
    /// \return A raw pointe to the built image
    /// \pre The given stream needs to respect the TARGA format
    /// \pre The width in the input stream needs to be in ]0; maxWidth]
    /// \pre The height in the input stream needs to be in ]0; maxHeight]
    /// \pre The intensity in the input stream needs to be in ]0; maxIntensity]
    /// \pre The value of pixels needs to be in the range of [0,intensity]
    /// \pre The representation of pixels needs to have a length equals to width * height
    /// \post An image sized to width on height, with depth equals to intensity, and pixels equals to value of pixels in the stream
    /// \exception invalidType if the type of format don't match with TARGA format
    /// \exception invalidWidth if the width in the stream was not in [0, maxWidth]
    /// \exception invalidHeight if the height in the stream was not in ]0; maxHeight]
    /// \exception invalidIntensity if the intensity in the stream was not in [0, maxIntensity]
    /// \exception invalidPixels if any of read pixels is not in the range of [0, intensity]
    /// \exception std::bad_alloc if the memory allocation failed
    /// \exception alwaysData if the stream always contains data
    /// \exception invalidSizeRepresentationPixel if the END OF STREAM was encountered before the reach width * height pixels
    static ColorImage* readTGA( std::istream& is );

    /// Read the given input stream and create a gray Image
    /// \note Check the representation of JPEG format : URL
    /// \note Or : URL
    /// \warning You have the responsibility to manage and delete the created image
    /// \warning After we read value of pixels, the stream needs to contain any data
    /// \return A raw pointe to the built image
    /// \pre The given stream needs to respect the JPEG format
    /// \pre The width in the input stream needs to be in ]0; maxWidth]
    /// \pre The height in the input stream needs to be in ]0; maxHeight]
    /// \pre The intensity in the input stream needs to be in ]0; maxIntensity]
    /// \pre The value of pixels needs to be in the range of [0,intensity]
    /// \pre The representation of pixels needs to have a length equals to width * height
    /// \post An image sized to width on height, with depth equals to intensity, and pixels equals to value of pixels in the stream
    /// \exception invalidType if the type of format don't match with JPEG format
    /// \exception invalidWidth if the width in the stream was not in [0, maxWidth]
    /// \exception invalidHeight if the height in the stream was not in ]0; maxHeight]
    /// \exception invalidIntensity if the intensity in the stream was not in [0, maxIntensity]
    /// \exception invalidPixels if any of read pixels is not in the range of [0, intensity]
    /// \exception std::bad_alloc if the memory allocation failed
    /// \exception alwaysData if the stream always contains data
    /// \exception invalidSizeRepresentationPixel if the END OF STREAM was encountered before the reach width * height pixels
    static ColorImage* readJPEG( const char* input );

    // TODO Faire une méthode qui retourne un unique ptr
    /// This method create a ColorImage with the format Maison2
    /// \warning You have the responsibility to manage the return pointer
    static ColorImage* readMaison2( std::istream& is );

    // TODO REtourner une pointeur safe
    /// This method create an anaglyphe
    /// \warning You have the responsiblity to manage the returned pointer
    ColorImage* anaglyphe() const;

    /// Not done
    /// Give two point and one color, and this function draw a right line from point 1 to point 2
    void line( intmax_t x1, intmax_t y1, intmax_t x2, intmax_t y2, Color color );


private:
    const Width width_;
    const Height height_;
    const Shade intensity_{ defaultIntensity };

    std::vector<Color> pixels_;


    /// The color black in shade of gray
    static const Color black;

    /// The default color for grayImage is black
    static const Color& defaultColor;

    /// The default intensity so default number of shades
    static const Shade defaultIntensity;

    /// Default quality for JPEG
    static constexpr auto defaultJPEGQuality = 75;


    /// Build a gray Image with the given width and height, intensity and the vector of shades
    /// \warning This builder move the given vector
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the given vector, so same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidSizeArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    ColorImage( intmax_t width, intmax_t height, intmax_t intensity, std::vector<Color>&& pixels );

    /// Build a ColorImage with the given width, height and intensity
    /// The built image was colored with the default Color
    /// \warning You have responsibility to manage the returned image
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception std::bad_alloc if the memory allocation fails
    static std::unique_ptr<ColorImage> createColorImage( intmax_t width, intmax_t height, intmax_t intensity );

    /// This method is use in the method line
    /// Just apply Behensam to draw line
    void Behensem2Octants( intmax_t x1, intmax_t y1, intmax_t x2, intmax_t y2, Color color );
};

// BETA
inline void ColorImage::line( const intmax_t x1, const intmax_t y1,
                              const intmax_t x2, const intmax_t y2, const Color color ) {
    Behensem2Octants(x1,y1,x2,y2,color);
}

// Inline methods

// GrayImage's methods

// LEGACY methods

// Getters
inline const Width& GrayImage::getWidth() const noexcept { return dimension.width; }
inline const Height& GrayImage::getHeight() const noexcept { return dimension.height; }

// Fillers
inline void GrayImage::clear( const intmax_t color ) { fill( color ); }

inline void GrayImage::rectangle( const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height ) {
    drawRectangle( imageUtils::Point{ x, y }, imageUtils::Dimension<>{ width, height }, defaultColor,
                   imageUtils::FILL::NO );
}

inline void GrayImage::rectangle(
        const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height, const intmax_t color ) {
    drawRectangle( imageUtils::Point{ x, y }, imageUtils::Dimension<>{ width, height }, color, imageUtils::FILL::NO );
}

inline void
GrayImage::fillRectangle( const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height ) {
    drawRectangle( imageUtils::Point{ x, y }, imageUtils::Dimension<>{ width, height }, defaultColor,
                   imageUtils::FILL::YES );
}

inline void GrayImage::fillRectangle(
        const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height, const intmax_t color ) {
    drawRectangle( imageUtils::Point{ x, y }, imageUtils::Dimension<>{ width, height }, color, imageUtils::FILL::YES );
}

// Scaler
inline GrayImage* GrayImage::simpleScale( const intmax_t newWidth, const intmax_t newHeight ) const {
    return simpleScale( imageUtils::Dimension<>{ newWidth, newHeight } ).release();
}
inline GrayImage* GrayImage::bilinearScale( const intmax_t newWidth, const intmax_t newHeight ) const {
    return bilinearScale( imageUtils::Dimension<>{ newWidth, newHeight } ).release();
}

// Reader
inline GrayImage* GrayImage::readPGM( std::istream& is ) {
    return readPGM_secured( is ).release();
}

// END LEGACY

// Getters
inline Width GrayImage::width() const noexcept { return dimension.width; }
inline Height GrayImage::height() const noexcept { return dimension.height; }


// Fillers
inline void GrayImage::clear() { fill( defaultColor ); }

inline void GrayImage::drawLine( const imageUtils::Point start, const intmax_t length, const imageUtils::TYPE type ) {
    drawLine( start, length, defaultColor, type );
}

inline void GrayImage::drawRectangle( const imageUtils::Point start, const imageUtils::Dimension<> rectangleDim ) {
    drawRectangle( start, rectangleDim, defaultColor, imageUtils::FILL::NO );
}

inline void GrayImage::drawRectangle(
        const imageUtils::Point start, const imageUtils::Dimension<> rectangleDim, const intmax_t color ) {
    drawRectangle( start, rectangleDim, color, imageUtils::FILL::NO );
}

inline void GrayImage::drawRectangle(
        const imageUtils::Point start, const imageUtils::Dimension<> rectangleDim, const imageUtils::FILL filled ) {
    drawRectangle( start, rectangleDim, defaultColor, filled );
}

// Writers
inline void GrayImage::writePGM( std::ostream& os ) const { writePGM( os, Format::WRITE_IN::BINARY ); }

inline void ColorImage::writeJPEG( const char* const output ) const { writeJPEG( output, defaultJPEGQuality ); }



// ColorImage's methods

// Getters
inline const Width& ColorImage::getWidth() const noexcept { return width_; }
inline const Height& ColorImage::getHeight() const noexcept { return height_; }

// Fillers
inline void ColorImage::clear() { fill( defaultColor ); }

inline void ColorImage::clear( const Color color ) { fill( color ); }

inline void ColorImage::horizontalLine( const intmax_t x, const intmax_t y, const intmax_t length ) {
    horizontalLine( x, y, length, defaultColor );
}

inline void ColorImage::verticalLine( const intmax_t x, const intmax_t y, const intmax_t length ) {
    verticalLine( x, y, length, defaultColor );
}

inline void ColorImage::rectangle( const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height ) {
    rectangle( x, y, width, height, defaultColor );
}

inline void
ColorImage::fillRectangle( const intmax_t x, const intmax_t y, const intmax_t width, const intmax_t height ) {
    fillRectangle( x, y, width, height, defaultColor );
}

// Writer
inline void ColorImage::writePPM( std::ostream& os ) const { writePPM( os, Format::WRITE_IN::BINARY ); }

inline void ColorImage::writeTGA( std::ostream& os ) const { writeTGA( os, Format::WRITE_IN::RLE ); }

inline void ColorImage::writeTGA( std::ostream& os, const bool rle ) const {
    if ( rle ) { writeTGA( os, Format::WRITE_IN::RLE ); }
    else { writeTGA( os, Format::WRITE_IN::NO_RLE ); }
}

// Builder
inline std::unique_ptr<ColorImage>
ColorImage::createColorImage( const intmax_t width, const intmax_t height, const intmax_t intensity ) {
    return std::make_unique<ColorImage>( width, height, intensity );
}


#endif // DAVID_A_IMAGE_HPP
