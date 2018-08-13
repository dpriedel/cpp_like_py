# cpp_like_py

This project is a proof-of-concept at bringing some Python type features to C++ using C++17 facilities. I
believe this type of thing is possible with Boost libraries as well (fusion, I think).  In fact, I need to use
Boost Hana and MP11 for a couple of things as well.

The first attempt is to build a py_vector modeled on Python's list data structure.

The notable feature of Python's list is that it is heterogeneous -- it can hold different types.
In Python, one can say:

  x = list(3, 4, "ab", 6, 8.2) resulting in a list with 5 elements.
  
  x[2] = 9.4
  


With py_vector and C++17's std::variant, one can now say in C++:

  py_vector<int, std::string, d> x{3, 4, "ab", 6, 8.2}; resulting in a list with 5 elements. 
  
  x[2] = 9.4;

As in Python, lists can be modified by adding, updating or removing elements. Lists
can be appended.  Lists can be copied. Lists of different types can interact as long as their type lists are
compatible.

It turns out to be not-straight-forward to get the actual value out of a std::variant. So, I have supplied methods to visit all elements of a given type contained in the py_vector.

This is a work in progress. Basic features are working using GCC 8.1 compiler. Similar functions could be provided for other C++ containers such as sets and maps.


