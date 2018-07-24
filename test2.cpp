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

using namespace std::string_literals;

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
    py_vector<int, std::string, float> like_a_list2{2, 3, "ab", 5, 3.4f}; 
    /* like_a_list2.append(3).append("ab"s).append(5).append(3.4f); */ 
    like_a_list2.print_list(std::cout);

    auto e = like_a_list2[0];
    /* auto f = like_a_list2::v_type<like_a_list2.index()>; */








    py_vector<double, int> like_a_list3{3, 4, 5, 6.2};
    /* like_a_list3.append(3).append(4).append(5).append(6.2); */
    like_a_list3.print_list(std::cout);

    
    std::cout << hana::length(like_a_list2.types_set_) << '\n';
    std::cout << hana::length(like_a_list3.types_set_) << '\n';
    /* auto common = hana::make_set(hana::intersection(py_vector<double, int>().t_list, py_vector<int, std::string, float>().t_list)); */

    constexpr auto xs = hana::make_set(hana::type_c<int>, hana::type_c<float>, hana::type_c<double>);
    constexpr auto ys = hana::make_set(hana::type_c<int>, hana::type_c<int>,hana::type_c<float>);
    std::cout << hana::length(hana::intersection(xs, ys)) << '\n';

    std::cout << std::boolalpha << (hana::length(hana::intersection(like_a_list3.types_set_, like_a_list2.types_set_))
       == hana::length(like_a_list3.types_set_)) << '\n';
    
    py_vector<float, int> like_a_list4{3, 3, 5, 6.2f};
    /* like_a_list4.append(3).append(4).append(5).append(6.2f); */
    py_vector<int, std::string, float> like_a_list5(like_a_list4);
    like_a_list5.print_list(std::cout);

    /* std::cout << std::boolalpha << a_contains_b(like_a_list2, like_a_list3) << '\n'; */
    /* a_contains_b(like_a_list2, like_a_list3); */
    /* auto x = driver(like_a_list2, like_a_list3); */

    /* like_a_list2 = like_a_list3; */

//    std::cout << "like_a_list.  should be empty.\n";
//    like_a_list.print_list(std::cout);
//
//    std::cout << "like_a_list2.  should be [3, ab, 5, 3.4].\n";
//    like_a_list2.print_list(std::cout);
//    
//    py_vector<int, std::string, float> like_a_list3{std::move(like_a_list2)}; 
//
//    std::cout << "like_a_list2.  should be empty after move ctor.\n";
//    like_a_list2.print_list(std::cout);
//
//    std::cout << "like_a_list3.  should be [3, ab, 5, 3.4].\n";
//    like_a_list3.print_list(std::cout);
//     
//    py_vector<int, std::string, float> like_a_list4; 
//
//    std::cout << "like_a_list4.  should be empty.\n";
//    like_a_list4.print_list(std::cout);
//
//    like_a_list4 = like_a_list3;
//
//    std::cout << "like_a_list4.  should be [3, ab, 5, 3.4] after assignment.\n";
//    like_a_list4.print_list(std::cout);
//
//    like_a_list3.append(like_a_list4);
//
//    std::cout << "like_a_list3.  should be [3, ab, 5, 3.4, 3, ab, 5, 3.4] after append.\n";
//    like_a_list3.print_list(std::cout);
//
//    py_vector<int, std::string, float> like_a_list5; 
//
//    std::cout << "like_a_list5.  should be empty.\n";
//    like_a_list5.print_list(std::cout);
//
//    like_a_list5 += like_a_list4;
//    std::cout << "like_a_list5.  should be [3, ab, 5, 3.4] after +=.\n";
//    like_a_list5.print_list(std::cout);
//
////    like_a_list5.append('c');
//// should fail to compile -- type conversion restriced here.
//    like_a_list5.append("c"s);       // now, this should work
//    std::cout << "like_a_list5.  should be [3, ab, 5, 3.4, c] after append.\n";
//    like_a_list5.print_list(std::cout);
//
//    auto like_a_list6 = like_a_list3.slice(3, 6);
//    std::cout << "like_a_list6.  should be [3.4, 3, ab] after slice.\n";
//    like_a_list6.print_list(std::cout);
//
//    like_a_list6[2] = 99;
//    std::cout << "like_a_list6.  should be [3.4, 3, 99] after element assignment.\n";
//    like_a_list6.print_list(std::cout);
//
//    std::cout << "test the 'contains' function.  should report 'false'\n";
//    std::cout << std::boolalpha << like_a_list6.contains("abcd"s) << '\n';
//
//    std::cout << "test the 'contains' function.  should report 'true'\n";
//    std::cout << std::boolalpha << like_a_list6.contains(3.4f) << '\n';

    return 0;
}
