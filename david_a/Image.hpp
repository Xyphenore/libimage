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

  uint8_t *pixels{nullptr};
};

inline const uint16_t& GrayImage::getWidth() const { return width_; }
inline const uint16_t& GrayImage::getHeight() const { return height_; }



class ColorImage {
public:
  void readPPM(std::istream &is);

  ColorImage *writePPM(std::ostream &os) const;

  void readTGA(std::istream &is);

  ColorImage *writeTGA(std::ostream &os) const;
};

class Color {
public:
  Color(int r, int g, int b);

private:
  int r;
  int g;
  int b;
};

#endif // IMAGE_HPP
