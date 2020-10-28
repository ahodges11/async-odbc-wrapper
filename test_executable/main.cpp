//
// Created by ahodges on 03/10/2020.
//



#include "simple_interface_test.hpp"


int main(int argc, char *argv[])
{
    try
    {
        // get connection string from program arguments
        if (argc < 2)
        {
            std::cout << "connection_str program argument not specified" << std::endl;
            exit(-1);
        }

        auto conn_str = std::string(argv[1]);

        auto test1 = simple_interface_test();
        test1.start(conn_str);


    }
    catch (std::exception &ex)
    {
        aodbc::log_fatal(ex.what());
    }

    return 0;
}
