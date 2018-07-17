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

#include "test.h"

int main(int argc, char *argv[])
{
   /* python has lists which can hold arbitrary types. 
    *
    * a = list('ab', 5, 3.4)
    *
    * */
    
    // how about some c++ now
    // see Bartek's coding blog vor article on std::variant
    
    py_list<std::string, int, float> like_a_list; 
    py_list<int, std::string, float> like_a_list2{3, "ab", 5, 3.4f}; 

    // lambda visitor
    
    like_a_list.print_list(std::cout);
    like_a_list2.print_list(std::cout);
    
    py_list<int, std::string, float> like_a_list3{std::move(like_a_list2)}; 
    like_a_list2.print_list(std::cout);
    like_a_list3.print_list(std::cout);
     
    return 0;
}
