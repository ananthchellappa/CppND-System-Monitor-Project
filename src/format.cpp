#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int d;
    int h;
    int m;
    int s;
    d = seconds / 60 / 60 / 24;
    h = ( seconds / 60 / 60 ) % 24;
    m = ( seconds / 60  ) % 60;
    s = seconds % 60;
    return std::to_string( d ) + "d " + std::to_string( h ) +
             ":" + std::to_string( m ) + ":" + std::to_string(s);
}