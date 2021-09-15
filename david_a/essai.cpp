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
        ifstream ifimage( "../ressources/chat.pgm", ios::binary );

        const GrayImage* const gray = GrayImage::readPGM( ifimage );

        GrayImage imgcopy( *gray );

        imgcopy.clear( 15 );

        GrayImage imgrect( *gray );

        GrayImage imgscale( *gray );

        auto pimg = imgscale.simpleScale( 2 * imgscale.getWidth(), 2 * imgscale.getHeight() );

        imgrect.fillRectangle( 150, 120, 10, 20, 0 );
        imgcopy.fillRectangle( 150, 120, 150, 50, 255 );

        ofstream ofimgrect( "../ressources/chatrect.pgm", ios::binary );
        imgrect.writePGM( ofimgrect );

        ofstream ofimgclear( "../ressources/chatclear.pgm", ios::binary );
        imgcopy.writePGM( ofimgclear );

        std::ofstream of( "../ressources/chatscale.pgm", ios::binary );
        pimg->writePGM( of );


    } // Trois types d'exceptions seront attrapés (les chaines C et C++ ainsi que
        // les std::exception et toutes ses dérivées). N'utilisez pas autre chose !
    catch ( exception& e ) { cerr << "Exception :" << e.what() << endl; } catch ( string& e ) {
        cerr << "Exception string :" << e << endl;
    } catch ( const char* e ) { cerr << "Exception const char* :" << e << endl; }
    return 0;
}
