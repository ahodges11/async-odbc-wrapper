//
// Created by ahodges on 03/11/2020.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <string>
#include <array>

#include "arguments.hpp"


std::string connection_str;

int main( int argc, char* argv[] )
{
    Catch::Session session; // There must be exactly one instance
    std::array<char *, 1> argv_fake = {argv[0]};
    session.applyCommandLine(1,argv_fake.data());

    // get connection string from program arguments
    if (argc < 2)
    {
        std::cout << "connection_str program argument not specified" << std::endl;
        exit(-1);
    }
    connection_str = std::string(argv[1]);

    int numFailed = session.run();
    return ( numFailed < 0xff ? numFailed : 0xff );
}