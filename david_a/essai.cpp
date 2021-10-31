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

        ifstream ifimage( "../ressources/chat.pgm", ios::binary );

        const auto greay = GrayImage::readPGM_secured( ifimage );

        auto pimg = greay->simpleScale( imageUtils::Dimension<>{823, 400} );
        auto pbilinear = greay->bilinearScale( imageUtils::Dimension<>{823, 400} );

        ofstream ot1( "..//ressources/ot1.pgm", ios::binary);
        ofstream ot2( "../ressources/ot2.pgm", ios::binary);

        pimg->writePGM( ot1, Format::WRITE_IN::BINARY );
        pbilinear->writePGM( ot2, Format::WRITE_IN::BINARY );

        auto grey = GrayImage( imageUtils::Dimension<>{10, 10}, 10);
        for ( int i = 00; i < 10; i++ ) {
            grey.drawLine( imageUtils::Point{i,0}, 10, i, imageUtils::TYPE::VERTICAL);
        }

        grey.drawLine( imageUtils::Point{0,0}, 10, 1, imageUtils::TYPE::HORIZONTAL);


        ofstream out("../ressources/grey.pgm", ios::binary);
        grey.writePGM( out );




    } // Trois types d'exceptions seront attrapés (les chaines C et C++ ainsi que
        // les std::exception et toutes ses dérivées). N'utilisez pas autre chose !
    catch ( exception& e ) { cerr << "Exception :" << e.what() << endl; } catch ( string& e ) {
        cerr << "Exception string :" << e << endl;
    } catch ( const char* e ) { cerr << "Exception const char* :" << e << endl; }
    return 0;
}
