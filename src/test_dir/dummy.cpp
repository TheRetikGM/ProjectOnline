#include "test_dir/dummy.h"
#include <iostream>

void Logger::Log(std::string msg)
{
    std::cout << "[MESSAGE]: " << msg << std::endl;
}