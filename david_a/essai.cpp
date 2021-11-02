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

extern const char* const identifier;   // Ne pas modifier ces deux lignes ! Vos changements au sujet
extern const char* const informations; // de ceux deux constantes doivent être faits dans Image.cpp.


int main( int argc, char* argv[] ) {
    cout << "Votre identifiant tel que declare dans Image.cpp : " << identifier << endl;
    cout << "Les informations que vous avez decide d'indiquer au correcteur : " << endl << informations << endl;
    try {
        /*
        ofstream ottest( "../ressources/test", ios::binary );
        uint16_t test = 1500;
        ottest.write( reinterpret_cast<const char*>(&test), 2 );
        ottest.close();

        uint16_t test1;
        uint8_t test1lo;
        uint8_t test1hi;
        ifstream ittest( "../ressources/test", ios::binary );
        ittest.read( reinterpret_cast<char*>(&test1), 2);
        ittest.seekg(0);
        ittest.read( reinterpret_cast<char*>(&test1lo), 1);
        ittest.read( reinterpret_cast<char*>(&test1hi), 1);

        cout << test1 << " " << (unsigned)test1lo << " " << (unsigned)test1hi << endl;

        if( test == test1 ) cout << "Good\n";
        else {
            cout <<"bad\n";
            cout << test << " " << test1 << endl;
        }*/

        ifstream input( "../ressources/images/chat.tga", ios::binary );
        auto chat = ColorImage::readTGA( input );

        ofstream output( "../ressources/chat_out.tga", ios::binary );
        chat->writeTGA( output, Format::WRITE_IN::NO_RLE );


        ifstream in2( "../ressources/images/palette_bl.tga", ios::binary );
        ifstream in3( "../ressources/images/palette_tl.tga", ios::binary );

        ofstream ou2( "../ressources/palette_bl_ou.tga", ios::binary );
        ofstream ou3( "../ressources/palette_tl_ou.tga", ios::binary );


        auto plbl = ColorImage::readTGA( in2 );
        auto pltl = ColorImage::readTGA( in3 );

        plbl->writeTGA( ou2, Format::WRITE_IN::NO_RLE );
        pltl->writeTGA( ou3, Format::WRITE_IN::NO_RLE );

        signed char c = 10;
        std::cout << ( ( c & 0b1000'0000 ) == 0b1000'0000 ) << '\n';

        ifstream in5( "../ressources/images/chanel.ppm", ios::binary );
        auto colour = ColorImage::readPPM( in5 );
        ofstream ou5( "../ressources/chanel.tga", ios::binary );
        colour->writeTGA( ou5 );

        ofstream ou6( "../ressources/chanel.ppm", ios::binary );
        colour->writePPM( ou6 );


    } // Trois types d'exceptions seront attrapés (les chaines C et C++ ainsi que
        // les std::exception et toutes ses dérivées). N'utilisez pas autre chose !
    catch ( exception& e ) { cerr << "Exception :" << e.what() << endl; } catch ( string& e ) {
        cerr << "Exception string :" << e << endl;
    } catch ( const char* e ) { cerr << "Exception const char* :" << e << endl; }
    return 0;
}
