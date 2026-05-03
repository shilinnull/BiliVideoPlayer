#include <iostream>
#include <gflags/gflags.h>

DECLARE_string(listen_ip);

void print()
{
    std::cout << FLAGS_listen_ip << std::endl;
}