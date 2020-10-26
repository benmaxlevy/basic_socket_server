#ifndef SOCKET_SERVER_CHECK_H
#define SOCKET_SERVER_CHECK_H

#include <string>
#include <iostream>

int check(int, char**, char**, int*);

int check(int argc, char** argv, char** args_to_check, int* to_write) {
    int count = 0; //we need to figure out how many char*s are in args_to_check
    //let's see what args we got passed!
    if (argc > 0) {
        for (int i = 1; i < argc; i++) //loop through argv
        {
            while(args_to_check[count] != nullptr) //check if the pointer to the char isn't nullptr
            {
                count++; //increment the count by 1
            }

            for(int a = 0; a < count; a++) //loop thru args_to_check
            {

                if(strcmp(argv[i], args_to_check[a]) == 0) //check the arg passed in this instance of the for loop is equal to the args to check
                {
                    *to_write = a; //set the to_write variable to a (to indicate which args_to_check was "true")
                    return 0;
                }
            }

            //else, they gave a bad flag
            std::cerr << "Invalid flag. Options: --http (for http), --basic (for basic TCP)" << std::endl;
            return -1;
        }
    }
    return -1;
}

#endif //SOCKET_SERVER_CHECK_H