#include "Image.hpp"

#include <iostream>


static void skip_comments( std::istream& is) {
    while ( '#' == is.peek() ) {
        is.ignore();
    }
}
