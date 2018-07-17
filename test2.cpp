/*
 * =====================================================================================
 *
 *       Filename:  test2.cpp
 *
 *    Description:  test bed for pytyon-like list class
 *
 *        Version:  1.0
 *        Created:  07/16/2018 03:41:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David P. Riedel (), driedel@cox.net
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <string>

#include "py_vector.h"

int main(int argc, char *argv[])
{
   /* python has lists which can hold arbitrary types. 
    *
    * a = list('ab', 5, 3.4)
    *
    * */
    
    // how about some c++ now
    // see Bartek's coding blog vor article on std::variant
    
    py_vector<std::string, int, float> like_a_list; 
    py_vector<int, std::string, float> like_a_list2{3, "ab", 5, 3.4f}; 

    like_a_list.print_list(std::cout);
    like_a_list2.print_list(std::cout);
    
    py_vector<int, std::string, float> like_a_list3{std::move(like_a_list2)}; 
    like_a_list2.print_list(std::cout);
    like_a_list3.print_list(std::cout);
     
    py_vector<int, std::string, float> like_a_list4; 
    like_a_list4.print_list(std::cout);

    like_a_list4 = like_a_list3;
    like_a_list4.print_list(std::cout);

    like_a_list3.append(like_a_list4);
    like_a_list3.print_list(std::cout);

    return 0;
}
