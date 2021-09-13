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

const char *const identifier = "david_a";

const char *const informations = "";

class GrayImage {
public:
    GrayImage() = delete;
    GrayImage(uint16_t width, uint16_t height);
    GrayImage(const GrayImage &src);
    ~GrayImage();

    GrayImage &operator=(const GrayImage &) = delete;

    const uint16_t& getWidth() const;
    const uint16_t& getHeight() const;

    uint8_t &pixel(uint16_t x, uint16_t y);
    const uint8_t &pixel(uint16_t x, uint16_t y) const;

    void clear(uint8_t color);

    void rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);

    GrayImage* simpleScale(uint16_t width, uint16_t height) const;
    GrayImage* bilinearScale(uint16_t width, uint16_t height) const;

    // PGM a 2 formats le P2 et le P5, il faut donc différencier les deux
    void writePGM(std::ostream &os) const;
    void writeTGA(std::ostream &os) const;
    void writeJPEG(std::ostream &os) const;

    static GrayImage *readPGM(std::istream &is);
    static GrayImage *readTGA(std::istream &is);
    static GrayImage *readJPEG(std::istream &is);

private:
    uint16_t width_;
    uint16_t height_;
    uint8_t intensity_;

    uint8_t* pixels{nullptr};
};

inline const uint16_t& GrayImage::getWidth() const { return width_; }
inline const uint16_t& GrayImage::getHeight() const { return height_; }



class Color {
public:
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b);

    uint8_t r_{ 0};
    uint8_t g_{ 0};
    uint8_t b_{ 0};
};

Color operator+( const Color& c1, const Color& c2 ) {
    return { static_cast<uint8_t>(c1.r_ + c2.r_),
             static_cast<uint8_t>(c1.g_ + c2.g_),
             static_cast<uint8_t>(c1.b_ + c2.b_)
    };
}
Color operator*( const double alpha, const Color& c ) {
    return { static_cast<uint8_t>(c.r_ * alpha),
             static_cast<uint8_t>(c.g_ * alpha),
             static_cast<uint8_t>(c.b_ * alpha)
    };
}



class ColorImage {
public:
    ColorImage() = delete;
    ColorImage( uint16_t width, uint16_t height );
    ColorImage(const ColorImage& src);
    ~ColorImage();
    ColorImage& operator=(const ColorImage& src) = delete;

    const uint16_t& getWidth() const;
    const uint16_t& getHeight() const;

    Color& pixel(uint16_t x, uint16_t y);
    const Color& pixel(uint16_t x, uint16_t y) const;

    void clear(Color color);

    void rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

    ColorImage* simpleScale(uint16_t width, uint16_t height) const;
    ColorImage* bilinearScale(uint16_t width, uint16_t height) const;

    static ColorImage* readPPM(std::istream &is);
    static ColorImage* readTGA(std::istream &is);

    void writePPM(std::ostream &os) const;
    void writeTGA(std::ostream &os) const;

private :
    uint16_t width_;
    uint16_t height_;
    uint8_t intensity_;

    Color* pixels {nullptr};
};

inline const uint16_t& ColorImage::getWidth() const { return width_; }
inline const uint16_t& ColorImage::getHeight() const { return height_; }






#endif // IMAGE_HPP
