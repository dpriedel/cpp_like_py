/*
 * =====================================================================================
 *
 *       Filename:  py_vector.h
 *
 *    Description:  python-like list for C++17
 *
 *        Version:  1.0
 *        Created:  07/16/2018 12:49:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David P. Riedel (), driedel@cox.net
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  py_vector_INC
#define  py_vector_INC

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

#include <boost/hana.hpp>

namespace hana = boost::hana;

// let's try some template metaprogramming using boost Hana.
// we need to be able to tell whether or not type list B
// is a proper subset of type list A so we know whether or not we
// can safely do py_vector copy ctors, assigments and other operation when the 
// py_vectors differ in the types they can hold.

// the approach chosen is to use set operations
// if the intersection of type sets A and B equals type set B then
// we know all the elements of the 'other' py_vector can be held by this py_vector.


/*
 * =====================================================================================
 *        Class:  py_vector
 *  Description:  provides a Python-like list class for C++
 * =====================================================================================
 */

// from: http://filipjaniszewski.com/2017/01/27/c17-variants
// comes this syntax which let's me construct a new variant of
// the proper type.
//
// using T = std::remove_cv_t<std::remove_reference_t<decltype(val)>>;

template<typename ...Ts>
class py_vector
{
    private:

        static constexpr inline auto types_list_ = hana::to_tuple(hana::tuple_t<Ts...>);
        static constexpr inline auto types_set_ = hana::make_set(hana::type_c<Ts>...);
        
        const int MAX_LEN_FOR_QUICK_COMPARE = 20;

    public:

        using value_type = std::variant<Ts...>;
        using pylist_t = std::vector<value_type>;

        /* ====================  LIFECYCLE     ======================================= */
        py_vector () { }                                                  /* constructor */
        template<typename ...Args>
        py_vector (Args ...args) : the_list_{{args} ...} { }              /* constructor */

        py_vector(const py_vector& rhs) : the_list_{rhs.the_list_} { }
        py_vector(py_vector&& rhs) : the_list_{std::move(rhs.the_list_)} { }

        // now, let's try some metaprogramming....
        // NOTE: per "C++ Templates the Complete Guide, 2nd ed." (pp.102,103), this is necessary to force use
        // of customized copy ctor.
        
        py_vector(py_vector const volatile& rhs) = delete;

        // now, make sure the we have all the types the class we are copying from does.
        
        template<class U,
            typename = std::enable_if_t<hana::equal(hana::intersection(types_set_, U::types_set_), U::types_set_)>>
        py_vector(const U& rhs)
        {
            // we can not simply copy the variant elements from the rhs vector, we need to use
            // our own type of variant elements which, thanks to check above, we know can handle
            // all the types possible in rhs.
           
            auto copy_value([this](auto&& e)
            {
                using X = std::remove_cv_t<std::remove_reference_t<decltype(e)>>;
                value_type new_elem{std::in_place_type<X>, e};
                this->the_list_.push_back(new_elem);
             }); 

            for (const auto& r_element : rhs.the_list_)
            {
                std::visit(copy_value, r_element);
             
            }
        }
        template<typename ...Us> friend class py_vector;

        /* ====================  ACCESSORS     ======================================= */

        auto size(void) const { return the_list_.size(); }
        auto begin() const { return the_list_.begin(); }
        auto cbegin() const { return the_list_.cbegin(); }
        auto end() const { return the_list_.end(); }
        auto cend() const { return the_list_.cend(); }
        auto empty() const { return  the_list_.empty(); }

        void print_list(std::ostream& out) const
        {
            auto print_item([&out](const auto& e) { out << e << ", "; });
            auto print_just_item([&out](const auto& e) { out << e ; });
            //TODO: fix this so it doesn't have a trailing ','.
            
            out << '[';

            int x{1};
            for (const auto e : the_list_)
            {
                if (x >= the_list_.size())
                {
                    break;
                }
                std::visit(print_item, e);
                ++x;
            }

            if (! the_list_.empty())
            {
                std::visit(print_just_item, the_list_.back());
            }

            out << "]\n";
        }

        std::string to_string() const
        {
            std::ostringstream out;
            print_list(out);
            return out.str();
        }

        // half open range
        py_vector slice(int lower_bound, int upper_bound) const
        {
            auto end = upper_bound > the_list_.size() ? the_list_.size() : upper_bound;
            auto start = lower_bound < 0 ? 0 : lower_bound;

            py_vector result;

            std::copy(&the_list_[start], &the_list_[end], std::back_inserter(result.the_list_));
            return result;
        }

        template<typename T>
        bool contains(const T& item) const
        {
            for (auto i = 0; i < the_list_.size(); ++i)
            {
                if (auto elem = std::get_if<T>(&the_list_[i]); elem)
                {
                    if (*elem == item)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        /* ====================  MUTATORS      ======================================= */
        
        py_vector& append(const py_vector& rhs)
        {
            if (this != & rhs)
            {
                std::copy(rhs.the_list_.cbegin(), rhs.the_list_.cend(), std::back_inserter(the_list_));
            }
            return *this;
        }

        template<typename T>
        py_vector& append(const T& element)
        {
            value_type e(std::in_place_type<T>, element);
            the_list_.push_back(e);
            return *this;
        }

//        py_vector& erase(std::size_t which)
//        {
//            the_list_.erase(&the_list_[which], &the_list_[which + 1]);
//            return *this;
//        }

        /* ====================  OPERATORS     ======================================= */

        py_vector& operator=(const py_vector& rhs)
        {
            if (this != &rhs)
                the_list_ = rhs.the_list_;
            return *this;
        }

        template<class U,
            typename = std::enable_if_t<hana::equal(hana::intersection(types_set_, U::types_set_), U::types_set_)>>
        py_vector& operator=(const U& rhs)
        {
            // we can not simply copy the variant elements from the rhs vector, we need to use
            // our own type of variant elements which, thanks to check above, we know can handle
            // all the types possible in rhs.
           
            // a little bit of exception safety.
            
            pylist_t new_values;
            
            auto copy_value([&new_values](auto&& e)
            {
                using X = std::remove_cv_t<std::remove_reference_t<decltype(e)>>;
                value_type new_elem{std::in_place_type<X>, e};
                new_values.push_back(new_elem);
             }); 

            for (const auto& r_element : rhs.the_list_)
            {
                std::visit(copy_value, r_element);
             
            }

            std::swap(this->the_list_, new_values);
            return *this;
        }

        py_vector& operator=(py_vector&& rhs)
        {
            if (this != &rhs)
                the_list_ = std::move(rhs.the_list_);
            return *this;
        }

        py_vector& operator+=(const py_vector& rhs)
        {
            if (this != &rhs)
                this->append(rhs);
            return *this;
        }

        template<typename T>
        py_vector& operator+=(const T& element)
        {
            value_type e{std::in_place_type<T>, element};
            the_list_.push_back(e);
            return *this;
        }

        value_type& operator[](int index)
        {
            return the_list_[index];
        }

        const value_type& operator[](int index) const
        {
            return the_list_[index];
        }

        bool operator==(const py_vector& rhs) const
        {
            return the_list_ == rhs.the_list_;
        }
        
        template<class U,
            typename = std::enable_if_t<hana::equal(hana::intersection(types_set_, U::types_set_), U::types_set_)>>
        bool operator==(const U& rhs) const
        {
            // since our containers hold different types, we need to do a little more work.
            // I haven't found a way to directly compare the individual elements held by different variant
            // types...it may not be possible.  But, it seems you can write the individual items to a std::stream.
            // This will probably be sssllloooowwww.... for large numbers of elements in the list.
             
            // TODO: look at MP11 generate test cases example and C++17 STL Cookbook tuple zipping as possible
            // approaches to a better comparison.
            
            if (the_list_.size() != rhs.the_list_.size())
            {
                return false;
            }

            if (the_list_.size() < MAX_LEN_FOR_QUICK_COMPARE)
            {
                // this should be faster for shorter strings due ot less ostringstream overhead
                
                auto x{to_string()};
                auto y{rhs.to_string()};

                return x == y;
            }

            auto compare_helper([this](auto&& a, auto&& b)
            {
                std::ostringstream x;
                std::ostringstream y;

                x << a;
                y << b;

                return x.str() == y.str();
            });

            auto compare_elements([&compare_helper](const auto& a, const auto& b)
            {
                return std::visit(compare_helper, a, b);
            });

            return std::equal(the_list_.cbegin(), the_list_.cend(), rhs.the_list_.cbegin(), compare_elements);
        }

    protected:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */

    private:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */
        pylist_t the_list_;

}; /* ----------  end of template class py_vector  ---------- */

#endif   /* ----- #ifndef py_vector_INC  ----- */
