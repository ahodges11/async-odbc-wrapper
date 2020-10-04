//
// Created by ahodges on 03/10/2020.
//

#include "aodbc.hpp"

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

        auto &env = aodbc::environment::get_singleton();
        env.init();

        {
            aodbc::log_debug("creating connection..");
            auto connection = aodbc::connection(env.get_env(), std::string(argv[1]));
            connection.connect();
            assert(connection.connected());
            connection.disconnect();
        }
        aodbc::log_debug("connection closed and deleted");



    }
    catch (std::exception & ex)
    {
        aodbc::log_fatal(ex.what());
    }


    return 0;
}
