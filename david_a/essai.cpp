#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <cstdlib>

using namespace std;

/*
 Toutes vos définitions de classes doivent être regroupées dans l'unique
 fichier "Image.h". Son nom n'est pas modifiable sinon vous empêcherez la
 compilation de l'autre executable servant à la correction.
*/
#include "Image.hpp"

extern const char * const identifier;   // Ne pas modifier ces deux lignes ! Vos changements au sujet
extern const char * const informations; // de ceux deux constantes doivent être faits dans Image.cpp.


int main(int argc,char *argv[])
{
 cout << "Votre identifiant tel que declare dans Image.cpp : " << identifier << endl;
 cout << "Les informations que vous avez decide d'indiquer au correcteur : " << endl << informations << endl;
 try
  {
   ifstream inputana( "../ressources/ombres.ppm", ios::binary);
   auto ana = ColorImage::readPPM(inputana);
   auto anargb = ana->anaglyphe();

   ofstream outputana("../ressources/ombresrgb.ppm", ios::binary);
   anargb->writePPM(outputana);

   ifstream inana ( "../ressources/temple.ppm", ios::binary);
   auto ana2 = ColorImage::readPPM(inana);

   auto ana2rgb = ana2->anaglyphe();

   ofstream outana("../ressources/templergb.ppm", ios::binary);
   ana2rgb->writePPM(outana);

  } // Trois types d'exceptions seront attrapés (les chaines C et C++ ainsi que
    // les std::exception et toutes ses dérivées). N'utilisez pas autre chose !
 catch(exception& e)
  { cerr<< "Exception :"<<e.what()<< endl; }
 catch(string& e)
  { cerr<< "Exception string :"<<e<< endl; }
 catch(const char * e)
  { cerr<< "Exception const char* :"<<e<< endl; }
 return 0;
}
