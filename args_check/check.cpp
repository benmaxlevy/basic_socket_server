#include "check.h"
#include <string>
#include <iostream>

int check(int argc, char** argv, char** args_to_check, std::string* to_write)
{
    //let's see what args we got passed!
    if(argc > 0)
    {
        for(int i = 1; i < argc; i++) //loop through argv
        {
            if(argv[i] == std::string("--http")) //see if the pointer to a char within the argv array is equal to --http (needs to be of type string, so not comparing pointer to pointer)
            {
                *to_write = "http";
                break;
            }
            else if(argv[i] == std::string("--basic")) //same as previous, just --basic
            {
                *to_write = "basic";
                break;
            }
            //else, they gave a bad flag
            std::cerr << "Invalid flag. Options: --http (for http), --basic (for basic TCP)" << std::endl;
            return -1;
        }
    }
}