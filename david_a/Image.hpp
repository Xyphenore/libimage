#ifndef IMAGE_HPP
#define IMAGE_HPP

const char * const identifier = "david_a";
// TODO
const char * const informations = "C'est de la merde";

#include <iostream>

class ColorImage {
public :
  void readPPM( std::istream& is );

  ColorImage* writePPM( std::ostream& os ) const;

  void readTGA( std::istream& is );

  ColorImage* writeTGA( std::ostream& os, bool ) const;


};

class Color {
public :
  Color( int r, int g, int b );

private :
  int r;
  int g;
  int b;
};


#endif //IMAGE_HPP
