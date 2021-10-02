#ifndef DAVID_A_IMAGE_HPP
#define DAVID_A_IMAGE_HPP

// Options de correction
#define CORR_PGMASCII //P2
#define CORR_PPMASCII //P3
// #define CORR_READCOLORJPEG
// #define CORR_WRITECOLORJPEG
// #define CORR_READCOLORTGARLE
// #define CORR_BRESENHAM
// #define CORR_TEMPLATE

#include <iostream>
#include <vector>
#include <limits>

const char* const identifier = "david_a";
const char* const informations = "";

using Width = std::uint16_t;
using Height = std::uint16_t;
using Shade = std::uint8_t;

using invalidColor = std::invalid_argument;


/// Represents the format of the read or written image with Px format
enum class Format : bool {ASCII = false , BINARY = true};

/// Can create or build a gray Image in 2D format, where the intensity or depth is coded on one byte
/// \warning The maximum of : width = max of uint16_t and height = max of uint16_t
/// \warning The maximum of : intensity = max of uint8_t
class GrayImage {
    /// The color black in shade of gray
    static constexpr Shade black = 0;

    /// The default color for grayImage is black
    static constexpr Shade defaultColor = black;

    /// The default intensity
    static constexpr Shade defaultIntensity = std::numeric_limits<Shade>::max();

    /// The default maximum of shades
    static constexpr Shade defaultMaxShades = defaultIntensity;


public:
    /// \warning Can't build a grayImage without giving width and height
    GrayImage() = delete;

    /// Build a grayImage with the given width, height and the default intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \post getWidth == given width
    /// \post getHeight == given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception std::bad_alloc if the memory allocation fails
    GrayImage( std::intmax_t width, std::intmax_t height );

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
    GrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity );

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

    static GrayImage* createGrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity );



    /// \return A constant reference of the width of Image
    const Width& getWidth() const noexcept;

    /// \return A constant reference of the height of Image
    const Height& getHeight() const noexcept;



    /// \return A reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    /// \warning Don't verify the value affected in the selected pixel
    Shade& pixel( std::intmax_t x, std::intmax_t y );

    /// \return A constant reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    const Shade& pixel( std::intmax_t x, std::intmax_t y ) const;



    /// Clear the image, so the image was fill by the default Color
    void clear();

    /// Clear the image, so fill the image with the given color
    /// \deprecated Please use the method fill, it has more sense
    /// \pre The given color needs to be in gray's shades in [0, maxIntensity]
    /// \post The image fills with the given color
    [[ deprecated ("Please use the method fill, it has more sense") ]]
    void clear( std::intmax_t color );

    /// Fill the image with the given color
    /// \pre Needs a color in gray's shades so in [0, maxIntensity]
    /// \post The same image but fill with the given color
    void fill( std::intmax_t color );



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
    void rectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height );

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
    void rectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height, std::intmax_t color );

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
    void fillRectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height );

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
    void fillRectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height, std::intmax_t color );



    /// Draw a 1 pixel of thickness horizontal line, in default Color, with the given length, and it left point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's width - x[
    /// \post The same image with the desired horizontal line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's width - x[
    void horizontalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length );

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
    void horizontalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length, std::intmax_t color );

    /// Draw a 1 pixel of thickness vertical line, in default Color, with the given length, and its top point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's height - y[
    /// \post The same image with the desired vertical line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's height - y[
    void verticalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length );

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
    void verticalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length, std::intmax_t color );



    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of simple scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    GrayImage* simpleScale( std::intmax_t newWidth, std::intmax_t newHeight ) const;

    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of bilinear scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    GrayImage* bilinearScale( std::intmax_t newWidth, std::intmax_t newHeight ) const;



    /// Write in the given output stream the called image in the P5 format
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePGM( std::ostream& os ) const;

    /// Write in the given output stream the called image in the given format {ASCII or BINARY}
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writePGM( std::ostream& os, Format f ) const;

    /// Write in the given output stream the called image in the format TARGA
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeTGA( std::ostream& os ) const;

    /// Write in the given output stream the called image in the format JPEG
    /// \note Check the representation of JPEG format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeJPEG( std::ostream& os ) const;



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
    static GrayImage* readPGM( std::istream& is );

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
    static GrayImage* readTGA( std::istream& is );

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
    static GrayImage* readJPEG( std::istream& is );



private:
    const Width width_;
    const Height height_;
    const Shade intensity_{ defaultMaxShades };

    std::vector<Shade> pixels_;

    /// Build a gray Image with the given width, height, intensity and the vector of shades
    /// \warning This builder build a copy of the given vector
    /// \deprecated Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the same vector, and same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    [[ deprecated ("Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient") ]]
    GrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, const std::vector<Shade>& pixels );

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
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    GrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, std::vector<Shade>&& pixels );
};


class Color {
public:
    constexpr Color() = default;
    constexpr Color( std::intmax_t r, std::intmax_t g, std::intmax_t b );
    constexpr Color( const Color& ) = default;
    constexpr Color( Color&& ) noexcept = default;
    ~Color() noexcept = default;

    Color& operator=( const Color& ) = default;
    Color& operator=( Color&& ) noexcept = default;

    Shade r_{ 0 };
    Shade g_{ 0 };
    Shade b_{ 0 };
};

// Color's method
constexpr Color::Color( const std::intmax_t r, const std::intmax_t g, const std::intmax_t b )
: r_( static_cast<Shade>(r) ), g_( static_cast<Shade>(g) ), b_( static_cast<Shade>(b) ) {
    constexpr auto limit = std::numeric_limits<Shade>::max();

    if ( ( 0 > r ) || ( limit < r ) ) {
        throw invalidColor( "Bad Color" );
    }

    if ( ( 0 > g ) || ( limit < g ) ) {
        throw invalidColor( "Bad Color" );
    }

    if ( ( 0 > b ) || ( limit < b ) ) {
        throw invalidColor( "Bad Color" );
    }

}


/// Can create or build a gray Image in 2D format, where the intensity or depth is coded on one byte
/// \warning The maximum of : width = max of uint16_t and height = max of uint16_t
/// \warning The maximum of : intensity = max of uint8_t
class ColorImage {
    static constexpr Shade defaultShade = 0;

    /// The color black in shade of gray
    static constexpr Color black = Color(defaultShade,defaultShade,defaultShade);

    /// The default color for grayImage is black
    static constexpr Color defaultColor = black;

    /// The default intensity
    static constexpr Shade defaultIntensity = std::numeric_limits<Shade>::max();

    /// The default maximum of shades
    static constexpr Shade defaultMaxShades = defaultIntensity;


public:
    /// \warning Can't build a grayImage without giving width and height
    ColorImage() = delete;

    /// Build a grayImage with the given width, height and the default intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \post getWidth == given width
    /// \post getHeight == given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception std::bad_alloc if the memory allocation fails
    ColorImage( std::intmax_t width, std::intmax_t height );

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
    ColorImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity );

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

    static ColorImage* createColorImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity );



    /// \return A constant reference of the width of Image
    const Width& getWidth() const noexcept;

    /// \return A constant reference of the height of Image
    const Height& getHeight() const noexcept;



    /// \return A reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    /// \warning Don't verify the value affected in the selected pixel
    Color& pixel( std::intmax_t x, std::intmax_t y );

    /// \return A constant reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x is not in [0; width[
    /// \exception invalidPosition if y is not int [0; height[
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    const Color& pixel( std::intmax_t x, std::intmax_t y ) const;



    /// Clear the image, so the image was fill by the default Color
    void clear();

    /// Clear the image, so fill the image with the given color
    /// \deprecated Please use the method fill, it has more sense
    /// \pre The given color needs to be in gray's shades in [0, maxIntensity]
    /// \post The image fills with the given color
    [[ deprecated ("Please use the method fill, it has more sense") ]]
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
    void rectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height );

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
    void rectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height, Color color );

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
    void fillRectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height );

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
    void fillRectangle( std::intmax_t x, std::intmax_t y, std::intmax_t width, std::intmax_t height, Color color );



    /// Draw a 1 pixel of thickness horizontal line, in default Color, with the given length, and it left point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's width - x[
    /// \post The same image with the desired horizontal line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's width - x[
    void horizontalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length );

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
    void horizontalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length, Color color );

    /// Draw a 1 pixel of thickness vertical line, in default Color, with the given length, and its top point at the given position x,y
    /// \pre x needs to be in [0, image's width[
    /// \pre y needs to be in [0; image's height[
    /// \pre length needs to be in ]0; image's height - y[
    /// \post The same image with the desired vertical line
    /// \exception invalidCoordinateX if x does not in [0; image's width[
    /// \exception invalidCoordinateY if y does not in [0; image's height[
    /// \exception invalidLength if length does not in ]0; image's height - y[
    void verticalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length );

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
    void verticalLine( std::intmax_t x, std::intmax_t y, std::intmax_t length, Color color );



    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of simple scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    ColorImage* simpleScale( std::intmax_t newWidth, std::intmax_t newHeight ) const;

    /// Created the same image of called image, but scale to newWidth and newHeight, with the algorithm of bilinear scale
    /// \warning You have the responsibility of the created image
    /// \pre newWidth needs to be in ]0; maxWidth]
    /// \pre newHeight needs to be in ]0; maxHeight]
    /// \post The same image of called image with the dimension of newWidth and newHeight
    /// \exception invalidWidth if newWidth does not in ]0; maxWidth]
    /// \exception invalidHeight if newHeight does not in ]0; maxHeight]
    ColorImage* bilinearScale( std::intmax_t newWidth, std::intmax_t newHeight ) const;



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
    void writePPM( std::ostream& os, Format f ) const;

    /// Write in the given output stream the called image in the format TARGA
    /// \note Check the representation of TARGA format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeTGA( std::ostream& os, bool compressed ) const;

    /// Write in the given output stream the called image in the format JPEG
    /// \note Check the representation of JPEG format : URL
    /// \note Or : URL
    /// \pre A good output stream
    /// \post The called image was output in the given the stream
    void writeJPEG( std::ostream& os ) const;



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
    static ColorImage* readJPEG( std::istream& is );



private:
    const Width width_;
    const Height height_;
    const Shade intensity_{ defaultMaxShades };

    std::vector<Color> pixels_;

    /// Build a gray Image with the given width, height, intensity and the vector of shades
    /// \warning This builder build a copy of the given vector
    /// \deprecated Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient
    /// \pre The given width needs to be greater than 0 and less or equal than maximum width
    /// \pre The given height needs to be greater than 0 and less or equal than maximum height
    /// \pre The given intensity needs to be greater or equal than 0 and less or equal than maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the same vector, and same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth]
    /// \exception invalidHeight if the given height is outside ]0;maxHeight]
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    [[ deprecated ("Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient") ]]
    ColorImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, const std::vector<Color>& pixels );

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
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    ColorImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, std::vector<Color>&& pixels );
};

// Beta
inline void GrayImage::writeTGA( std::ostream& os ) const {
    // Do nothing
}

inline void GrayImage::writeJPEG( std::ostream& os ) const {
    // Do nothing
}

inline GrayImage* GrayImage::readTGA( std::istream& is ) {
    // Do nothing
    return createGrayImage(11, 11, 11);
}

inline GrayImage* GrayImage::readJPEG( std::istream& is ) {
    // Do nothing
    return createGrayImage(11, 11, 11);
}

inline void ColorImage::writeTGA( std::ostream&, bool ) const {
    // Do nothing
}

inline void ColorImage::writeJPEG( std::ostream& ) const {
    // Do nothing
}

inline ColorImage* ColorImage::readTGA( std::istream& is ) {
    // Do nothing
    return createColorImage( 10, 10, 10);
}

inline ColorImage* ColorImage::readJPEG( std::istream& is ) {
    // Do nothing
    return createColorImage( 10, 10, 10);
}

// Inline methods

//GrayImage's methods

// Getters
inline const Width& GrayImage::getWidth() const noexcept { return width_; }
inline const Height& GrayImage::getHeight() const noexcept { return height_; }

// Fillers
inline void GrayImage::clear() { fill(defaultColor); }
inline void GrayImage::clear( const std::intmax_t color ) { fill( color ); }
inline void GrayImage::horizontalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length ) {
    horizontalLine(x,y,length,defaultColor);
}
inline void GrayImage::verticalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length ) {
    verticalLine(x,y,length,defaultColor);
}
inline void GrayImage::rectangle( const std::intmax_t x, const std::intmax_t y,
                                  const std::intmax_t width, const std::intmax_t height ) {
    rectangle(x,y,width,height,defaultColor);
}
inline void GrayImage::fillRectangle( const std::intmax_t x, const std::intmax_t y,
                                      const std::intmax_t width, const std::intmax_t height ) {
    fillRectangle(x,y,width,height, defaultColor);
}

// Writers
inline void GrayImage::writePGM( std::ostream& os ) const { writePGM(os, Format::BINARY); }

// Builder
inline GrayImage* GrayImage::createGrayImage(const std::intmax_t width, const std::intmax_t height, const std::intmax_t intensity) {
    return new GrayImage(width, height, intensity);
}


// ColorImage's methods

// Getters
inline const Width& ColorImage::getWidth() const noexcept { return width_; }
inline const Height& ColorImage::getHeight() const noexcept { return height_; }

// Fillers
inline void ColorImage::clear() { fill(defaultColor); }
inline void ColorImage::clear( const Color color ) { fill(color); }
inline void ColorImage::horizontalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length ) {
    horizontalLine(x,y,length,defaultColor);
}
inline void ColorImage::verticalLine( const std::intmax_t x, const std::intmax_t y, const std::intmax_t length ) {
    verticalLine(x,y,length,defaultColor);
}
inline void ColorImage::rectangle( const std::intmax_t x, const std::intmax_t y,
                                  const std::intmax_t width, const std::intmax_t height ) {
    rectangle(x,y,width,height,defaultColor);
}
inline void ColorImage::fillRectangle( const std::intmax_t x, const std::intmax_t y,
                                      const std::intmax_t width, const std::intmax_t height ) {
    fillRectangle(x,y,width,height, defaultColor);
}

// Writer
inline void ColorImage::writePPM( std::ostream& os ) const { writePPM(os, Format::BINARY); }

// Builder
inline ColorImage* ColorImage::createColorImage( const std::intmax_t width, const std::intmax_t height, const std::intmax_t intensity) {
    return new ColorImage( width, height, intensity);
}


#endif // DAVID_A_IMAGE_HPP
