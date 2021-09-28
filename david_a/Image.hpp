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
using GrayShade = std::uint8_t;

/// Represents the format of the read or written image with Px format
enum class Format : bool {ASCII = false , BINARY = true};

/// Can create or build a gray Image in 2D format, where the intensity or depth is coded on one byte
/// \warning The maximum of : width = max of uint16_t and height = max of uint16_t
/// \warning The maximum of : intensity = max of uint8_t
class GrayImage {
    /// The color black in shade of gray
    static constexpr GrayShade black = 0;

    /// The default color for grayImage is black
    static constexpr GrayShade defaultColor = black;

    /// The default intensity
    static constexpr GrayShade defaultIntensity = std::numeric_limits<GrayShade>::max();

    /// The default maximum of shades
    static constexpr GrayShade defaultMaxShades = defaultIntensity;


public:
    /// \warning Can't build a grayImage without giving width and height
    GrayImage() = delete;

    /// Build a grayImage with the given width, height and the default intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less than maximum of the maximum width
    /// \pre The given height needs to be greater than 0 and less than maximum of the maximum height
    /// \post getWidth == given width
    /// \post getHeight == given height
    /// \exception invalidWidth if the given width is outside ]0; maxWidth[
    /// \exception invalidHeight if the given height is outside ]0;maxHeight[
    /// \exception std::bad_alloc if the memory allocation fails
    GrayImage( std::intmax_t width, std::intmax_t height );

    /// Build a grayImage with the given width, height and intensity
    /// The built image was colored with the default Color
    /// \pre The given width needs to be greater than 0 and less than maximum of the maximum width
    /// \pre The given height needs to be greater than 0 and less than maximum of the maximum height
    /// \pre The given intensity needs to be greater than 0 and less than maximum of the maximum intensity
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \exception invalidWidth if the given width is outside ]0; maxWidth[
    /// \exception invalidHeight if the given height is outside ]0;maxHeight[
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

    /// \return A constant reference of the width of Image
    const uint16_t& getWidth() const noexcept;

    /// \return A constant reference of the height of Image
    const uint16_t& getHeight() const noexcept;

    /// \return A reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x >= width_ or y >= height_
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    /// \warning Don't verify the value affected in the selected pixel
    uint8_t& pixel( std::intmax_t x, std::intmax_t y );

    /// \return A constant reference of the pixel at the position x,y
    /// \param[in] position x,y
    /// \exception invalidPosition if x >= width_ or y >= height_
    /// \exception std::out_of_range if ( width_ * y + x ) >= width_ * height_
    const uint8_t& pixel( std::intmax_t x, std::intmax_t y ) const;


    // Continuer la documentation
    void clear();
    void clear( GrayShade color );

    void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height );
    void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, GrayShade color );
    void fillRectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height );
    void fillRectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, GrayShade color );

    void horizontalLine( uint16_t x, uint16_t y, uint16_t length );
    void horizontalLine( uint16_t x, uint16_t y, uint16_t length, GrayShade color );
    void verticalLine( uint16_t x, uint16_t y, uint16_t length );
    void verticalLine( uint16_t x, uint16_t y, uint16_t length, GrayShade color );

    GrayImage* simpleScale( uint16_t newWidth, uint16_t newHeight ) const;
    GrayImage* bilinearScale( uint16_t newWidth, uint16_t newHeight ) const;

    // PGM a 2 formats le P2 et le P5, il faut donc différencier les deux
    void writePGM( std::ostream& os ) const;
    void writePGM( std::ostream& os, Format f ) const;
    void writeTGA( std::ostream& os ) const;
    void writeJPEG( std::ostream& os ) const;

    /// Read the given input stream and create a gray Image
    /// \note Check the representation of Px format : https://en.wikipedia.org/wiki/Netpbm
    /// \note Or : http://netpbm.sourceforge.net/doc/pgm.html
    /// \note All comments are skipped
    /// \note We support only one image per file, in any format
    /// \warning You have the responsibility to manage and delete the created image
    /// \warning After we read value of pixels, the stream don't contain any data
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


    static GrayImage* readTGA( std::istream& is );
    static GrayImage* readJPEG( std::istream& is );

private:
    const Width width_;
    const Height height_;
    const GrayShade intensity_{ defaultMaxShades };

    std::vector<GrayShade> pixels_;

    /// Build a gray Image with the given width, height, intensity and the vector of shades
    /// \warning This builder build a copy of the given vector
    /// \deprecated Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient
    /// \pre The given width needs to be greater than 0 and less than maximum of the maximum width
    /// \pre The given height needs to be greater than 0 and less than maximum of the maximum height
    /// \pre The given intensity needs to be greater than 0 and less than maximum of the maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the same vector, and same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth[
    /// \exception invalidHeight if the given height is outside ]0;maxHeight[
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    [[ deprecated ("Because this builder copy the vector of shades, please use the builder who move the vector, it is more efficient") ]]
    GrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, const std::vector<GrayShade>& pixels );

    /// Build a gray Image with the given width and height, intensity and the vector of shades
    /// \warning This builder move the given vector
    /// \pre The given width needs to be greater than 0 and less than maximum of the maximum width
    /// \pre The given height needs to be greater than 0 and less than maximum of the maximum height
    /// \pre The given intensity needs to be greater than 0 and less than maximum of the maximum intensity
    /// \pre The given vector of shades needs to have it size equal to width * height
    /// \pre The given vector of shades needs to have all of it pixels to be in [0;intensity]
    /// \post getWidth was equal to the given width
    /// \post getHeight was equal to the given height
    /// \post The intensity of Image was equal to the given intensity
    /// \post The built image contains the given vector, so same representation of the pixels
    /// \exception invalidWidth if the given width is outside ]0; maxWidth[
    /// \exception invalidHeight if the given height is outside ]0;maxHeight[
    /// \exception invalidIntensity if the given intensity is outside [0;maxShades]
    /// \exception invalidArray if the given vector don't have the good size like vector_size == width * height
    /// \exception badValuePixel if a pixel in the given vector, have a value above the intensity
    GrayImage( std::intmax_t width, std::intmax_t height, std::intmax_t intensity, std::vector<GrayShade>&& pixels );
};

inline const uint16_t& GrayImage::getWidth() const noexcept { return width_; }
inline const uint16_t& GrayImage::getHeight() const noexcept { return height_; }

inline void GrayImage::writePGM( std::ostream& os ) const { writePGM(os, Format::BINARY); }

inline void GrayImage::clear() { clear(defaultColor); }
inline void GrayImage::horizontalLine( const uint16_t x, const uint16_t y, const uint16_t length ) {
    horizontalLine(x,y,length,defaultColor);
}
inline void GrayImage::verticalLine( const uint16_t x, const uint16_t y, const uint16_t length ) {
    verticalLine(x,y,length,defaultColor);
}
inline void GrayImage::rectangle( const uint16_t x, const uint16_t y,
                                  const uint16_t width, const uint16_t height ) {
    rectangle(x,y,width,height,defaultColor);
}
inline void GrayImage::fillRectangle( const uint16_t x, const uint16_t y,
                                      const uint16_t width, const uint16_t height ) {
    fillRectangle(x,y,width,height, defaultColor);
}



class Color {
public:
    Color() = default;
    Color( uint8_t r, uint8_t g, uint8_t b );

    uint8_t r_{ 0 };
    uint8_t g_{ 0 };
    uint8_t b_{ 0 };
};


class ColorImage {
public:
    ColorImage() = delete;

    ColorImage( uint16_t width, uint16_t height );

    ColorImage( uint16_t width, uint16_t height, uint8_t intensity );

    ColorImage( const ColorImage& src );

    ~ColorImage();

    ColorImage& operator=( const ColorImage& src ) = delete;

    const uint16_t& getWidth() const;

    const uint16_t& getHeight() const;

    Color& pixel( uint16_t x, uint16_t y );

    const Color& pixel( uint16_t x, uint16_t y ) const;

    void clear( Color color );

    void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color = Color());

    void fillRectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color = Color());

    ColorImage* simpleScale( uint16_t width, uint16_t height ) const;

    ColorImage* bilinearScale( uint16_t width, uint16_t height ) const {
        return new ColorImage( 0, 0 );
    };

    static ColorImage* readPPM( std::istream& is );

    static ColorImage* readTGA( std::istream& is ) {
        return new ColorImage( 0, 0 );
    };

    void writePPM( std::ostream& os ) const;

    void writeTGA( std::ostream& os, bool compressed ) const {
        return;
    };

private :
    uint16_t width_;
    uint16_t height_;
    uint8_t intensity_;

    Color* pixels{ nullptr };
};

inline const uint16_t& ColorImage::getWidth() const { return width_; }
inline const uint16_t& ColorImage::getHeight() const { return height_; }


#endif // DAVID_A_IMAGE_HPP
