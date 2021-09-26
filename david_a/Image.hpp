#ifndef IMAGE_HPP
#define IMAGE_HPP

// Options de correction
// #define CORR_PGMASCII //P2 //Try
// #define CORR_PPMASCII //P3
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

using grayShade = uint8_t;

enum class Format : bool {ASCII = false , BINARY = true};

class GrayImage {
    static constexpr grayShade black = 0;
    static constexpr grayShade defaultColor = black;

    static constexpr grayShade maxDefaultShades = std::numeric_limits<grayShade>::max();
public:
    GrayImage() = delete;
    GrayImage( uint16_t width, uint16_t height );
    GrayImage( uint16_t width, uint16_t height, uint8_t intensity );
    GrayImage( const GrayImage& src ) = default;
    GrayImage( GrayImage&& src ) noexcept = default;
    ~GrayImage() noexcept = default;

    GrayImage& operator=( const GrayImage& ) = delete;
    GrayImage& operator=( GrayImage&& ) noexcept = delete;

    const uint16_t& getWidth() const noexcept;
    const uint16_t& getHeight() const noexcept;

    uint8_t& pixel( uint16_t x, uint16_t y );
    const uint8_t& pixel( uint16_t x, uint16_t y ) const;

    void clear();
    void clear( grayShade color );

    void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height );
    void rectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, grayShade color );
    void fillRectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height );
    void fillRectangle( uint16_t x, uint16_t y, uint16_t width, uint16_t height, grayShade color );

    void horizontalLine( uint16_t x, uint16_t y, uint16_t length );
    void horizontalLine( uint16_t x, uint16_t y, uint16_t length, grayShade color );
    void verticalLine( uint16_t x, uint16_t y, uint16_t length );
    void verticalLine( uint16_t x, uint16_t y, uint16_t length, grayShade color );

    GrayImage* simpleScale( uint16_t newWidth, uint16_t newHeight ) const;
    GrayImage* bilinearScale( uint16_t newWidth, uint16_t newHeight ) const;

    // PGM a 2 formats le P2 et le P5, il faut donc différencier les deux
    void writePGM( std::ostream& os ) const;
    void writePGM( std::ostream& os, Format f ) const;
    void writeTGA( std::ostream& os ) const;
    void writeJPEG( std::ostream& os ) const;

    static GrayImage* readPGM( std::istream& is );
    static GrayImage* readTGA( std::istream& is );
    static GrayImage* readJPEG( std::istream& is );

private:
    const uint16_t width_;
    const uint16_t height_;
    const grayShade intensity_{ maxDefaultShades };

    std::vector<grayShade> pixels_;

    GrayImage( uint16_t width, uint16_t height, uint8_t intensity, const std::vector<uint8_t>& pixels );
    GrayImage( uint16_t width, uint16_t height, uint8_t intensity, std::vector<uint8_t>&& pixels );
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


#endif // IMAGE_HPP
