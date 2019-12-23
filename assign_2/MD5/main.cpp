#include "myMD5.hpp"

string input_msg = "www.sysu.edu.cn";

int main()
{
    calculate_digest(input_msg);
    print_ans(input_msg);
    return 0;
}