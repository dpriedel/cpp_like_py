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

#include <gmock/gmock.h>

using namespace testing;

#include "py_vector.h"

using namespace std::string_literals;

class Variants : public Test
{
};

TEST_F(Variants, DefaultConstructedVariantsMayBeEqual)
{
    std::variant<int, std::string, float>x;
    std::variant<float, int>y;
    std::variant<int, float, std::string>z;

    EXPECT_FALSE(cpp_like_py::operator==(x, y));
    ASSERT_TRUE(cpp_like_py::operator==(x, z));
};

TEST_F(Variants, EqualContentsShouldBeEqual)
{
    std::variant<int, std::string, float>x{3};
    std::variant<float, int>y{3};
    std::variant<int, float, std::string>z{3};

    EXPECT_TRUE(cpp_like_py::operator==(x, y));
    ASSERT_TRUE(cpp_like_py::operator==(x, z));
};

TEST_F(Variants, DifferentContentsShouldNotBeEqual)
{
    std::variant<int, std::string, float>x{3};
    std::variant<float, int>y{3.0F};
    std::variant<int, float, std::string>z{std::string{"Hello World"}};

    EXPECT_FALSE(cpp_like_py::operator==(x, y));
    ASSERT_FALSE(cpp_like_py::operator==(x, z));
};

class Constructors : public Test
{

};

TEST_F(Constructors, DefaultCtor)
{
    py_vector<std::string, int, float> like_a_list; 
    like_a_list.print_list(std::cout);

    ASSERT_TRUE(like_a_list.empty());
}

TEST_F(Constructors, InitializerListCtor)
{
    py_vector<std::string, int, float> like_a_list{3, "ab", 5, 3.4F}; 
    like_a_list.print_list(std::cout);

    ASSERT_EQ(like_a_list.size(), 4);
}

TEST_F(Constructors, CopyCtorSameTypes)
{
    py_vector<std::string, int, float> like_a_list{3, "ab", 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    py_vector<std::string, int, float> like_a_list2{like_a_list}; 
    like_a_list2.print_list(std::cout);

    ASSERT_TRUE(like_a_list == like_a_list2);
}

TEST_F(Constructors, CopyCtorDifferentTypes)
{
    py_vector<int, float> like_a_list{3, 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    py_vector<float, std::string, int> like_a_list2{like_a_list}; 
    like_a_list2.print_list(std::cout);

    ASSERT_TRUE(like_a_list2 == like_a_list);
}

TEST_F(Constructors, DISABLED_CopyCtorIncompatibleTypes)
{
    py_vector<int, float> like_a_list{3, 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    
    // need to comment out the code below so the program will compile.
    // even though test is dsabled, it still needs to compile.
    //
//    py_vector<std::string, int> like_a_list2{like_a_list}; 
//    like_a_list2.print_list(std::cout);
//
//    ASSERT_EQ((like_a_list2 == like_a_list), true);
}

class Operators : public Test
{

};

TEST_F(Operators, AppendToEmptyList)
{
    py_vector<int, float> like_a_list; 
    like_a_list.print_list(std::cout);
    like_a_list.append({3, 5, 3.4F}); 
    like_a_list.print_list(std::cout);

    ASSERT_TRUE((like_a_list == py_vector<int, float>{3, 5, 3.4F}));
}

TEST_F(Operators, AppendSingleElement)
{
    py_vector<int, float, char> like_a_list{3, 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    like_a_list += 'z';
    like_a_list.print_list(std::cout);

    ASSERT_TRUE((like_a_list == py_vector<int, float, char>{3, 5, 3.4F, 'z'}));
}

TEST_F(Operators, AssignToEmptyListSameTypes)
{
    py_vector<std::string, int, float> like_a_list; 
    like_a_list.print_list(std::cout);
    py_vector<std::string, int, float> like_a_list2{3, "ab", 5, 3.4F}; 
    like_a_list2.print_list(std::cout);

    like_a_list = like_a_list2;
    like_a_list.print_list(std::cout);

    ASSERT_EQ(like_a_list, like_a_list2);
}

TEST_F(Operators, AssignToEmptyListCompatibleTypes)
{
    py_vector<std::string, int, float> like_a_list; 
    like_a_list.print_list(std::cout);
    py_vector<int, float> like_a_list2{3, 5, 3.4F}; 
    like_a_list2.print_list(std::cout);

    like_a_list = like_a_list2;
    like_a_list.print_list(std::cout);

    ASSERT_EQ(like_a_list, like_a_list2);
}

TEST_F(Operators, AssignToListCompatibleTypes)
{
    py_vector<std::string, int, float> like_a_list{3, "ab", 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    py_vector<int, float> like_a_list2{3, 5, 3.4F}; 
    like_a_list2.print_list(std::cout);

    like_a_list = like_a_list2;
    like_a_list.print_list(std::cout);

    ASSERT_EQ(like_a_list, like_a_list2);
}

TEST_F(Operators, DISABLED_AssignToListIncompatibleTypes)
{
    py_vector<std::string, int, float> like_a_list{3, "ab", 5, 3.4F}; 
    like_a_list.print_list(std::cout);
    py_vector<int, float> like_a_list2{3, 5, 3.4F}; 
    like_a_list2.print_list(std::cout);

//    like_a_list2 = like_a_list;
//    like_a_list.print_list(std::cout);
//
//    ASSERT_EQ(like_a_list, like_a_list2);
}

TEST_F(Operators, IndexOperatorGetAndSet)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, 3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);
    auto x = like_a_list[3];
    EXPECT_TRUE((x == py_vector<int, std::string, float, char>::value_type{'z'}));

    like_a_list[5] = "Good bye";
    like_a_list.print_list(std::cout);
    ASSERT_TRUE((like_a_list == py_vector<int, std::string, float, char>{3, 5, 3.4F, 'z', 8.2F, "Good bye"}));
}

TEST_F(Operators, Slice)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, 3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);

    auto x = like_a_list.slice(2, 5);
    x.print_list(std::cout);
    
    EXPECT_FALSE((x == py_vector<int, std::string, float, char>{3.4F, 'r', 8.2F}));
    ASSERT_TRUE((x == py_vector<int, std::string, float, char>{3.4F, 'z', 8.2F}));
}

TEST_F(Operators, Contains)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, 3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);

    EXPECT_FALSE(like_a_list.contains("Goodbye world"s));
    ASSERT_TRUE(like_a_list.contains('z'));
}

TEST_F(Operators, EraseRange)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, 3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);

    like_a_list.erase(2, 5);
    like_a_list.print_list(std::cout);

    ASSERT_TRUE((like_a_list == py_vector<int, std::string, float, char>{3, 5, "Hello World"}));
}

TEST_F(Operators, VisitAllStrings)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, "Hi, I'm Dave",  3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);

    std::vector<std::string> my_strings;

    auto collect_strings([&my_strings] (const std::string& input) { my_strings.push_back(input); } );

    like_a_list.visit_all<std::string>(collect_strings);

    ASSERT_EQ(my_strings.size(), 2);
}

TEST_F(Operators, MultiplyAllFloats)
{
    py_vector<int, std::string, float, char> like_a_list{3, 5, "Hi, I'm Dave",  3.4F, 'z', 8.2F, "Hello World"}; 
    like_a_list.print_list(std::cout);

    auto multiply_floats([factor = 3.0F] (float& input) { input *= factor; } );

    like_a_list.visit_all<float>(multiply_floats);
    like_a_list.print_list(std::cout);

    ASSERT_TRUE((like_a_list == py_vector<int, std::string, float, char>{3, 5, "Hi, I'm Dave",  (3.4F * 3.0F), 'z', (8.2F * 3.0F), "Hello World"}));
}

int main(int argc, char *argv[])
{
   /* python has lists which can hold arbitrary types. 
    *
    * a = list('ab', 5, 3.4)
    *
    * */
    
    // how about some c++ now
    // see Bartek's coding blog vor article on std::variant
    //
    // py_vector<float, std::string, int> a_list{"ab", 5, 3.4f};
    
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
