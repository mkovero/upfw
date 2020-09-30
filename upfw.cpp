//
// Created by Markus on 23.9.2020.
//
#include <iostream>
#include "upfw.h"
#include <CLI/CLI.hpp>


int main(int argc, char *argv[], char *envp[]) {
    std::string hostname = "";
    CLI::App app("UpCloud FW refreshner");
    std::cout << "See usage with -h. Auth details are supposed to be given "
    << "via UPC_HOST UPC_USER and UPC_PWD env variables." << std::endl;
    CLI::Option *hopt = app.add_option("-H,--hostname", hostname, "VM hostname");
    CLI11_PARSE(app, argc, argv);

    if (hostname.size() > 0) {
        UpFW manager(hostname);
    }
    return 0;

}