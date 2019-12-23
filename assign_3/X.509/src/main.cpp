#include "ans.hpp"

string file_name = "./CER/apple.com.cer";

int main()
{   
    parseX509(read_from_file(file_name));
    return 0;
}

