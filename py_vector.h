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
#include <iostream>
#include <iterator>
#include <variant>
#include <vector>
#include <functional>

#include <boost/hana.hpp>

namespace hana = boost::hana;

/* #include <boost/hana/experimental/printable.hpp> */


// let's try some template metaprogramming using boost Hana.
// we need a metafunction to tell us whether or not list B
// is a proper subset of list A so we know whether or not we
// can safely do py_vector copy ctors and assigments when the 
// py_vectors differ in the types they can hold.

// the approach chosen is to use set operations
// if the length of the intersection of the sets representing the type lists
// used by each py_vector is equal to the length of the 'other' py_vector
// then the members of the 'other' py_vector can be safely used by this py_vector.

//https://lists.boost.org/boost-users/2007/06/28577.php
//Than you for the quick reply :) Now I reimplemented 
//intersect by converting the type sequences to mpl::set 
//and do a mpl::has_key instead of mpl::contains: 
//template <typename seq1, typename seq2> 
//struct intersect 
//{ 
//  typedef typename copy<seq2 
//     , inserter< set<>, insert< _1, _2 > > 
//>::type set_seq2; 
//  typedef typename copy_if<seq1 
//     , has_key <set_seq2, _1> 
//     , back_inserter < vector<> > 
//>::type type; 
//}; 


/*
 * =====================================================================================
 *        Class:  py_vector
 *  Description:  provides a Python-like list class for C++
 * =====================================================================================
 */

template<typename ...Ts>
class py_vector
{
    public:

        static constexpr inline auto types_list_ = hana::to_tuple(hana::tuple_t<Ts...>);
        static constexpr inline auto types_set_ = hana::make_set(hana::type_c<Ts>...);
        
        using value_type = std::variant<Ts...>;
        using pylist_t = std::vector<value_type>;

//        template <std::size_t N>
//        using v_type = typename std::variant_alternative<N, std::variant<Ts...>>::type;

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
           
            value_type elem;
            auto get_value([this, &elem](const auto& e){ elem = e ; }); 

            for (const auto& r_element : rhs.the_list_)
            {
                std::visit(get_value, r_element);
                this->the_list_.push_back(elem);
            }
        }
        template<typename ...Us> friend class py_vector;

        /* ====================  ACCESSORS     ======================================= */

        auto len(void) const { return the_list_.size(); }
        auto begin() const { return the_list_.begin(); }
        auto cbegin() const { return the_list_.cbegin(); }
        auto end() const { return the_list_.end(); }
        auto cend() const { return the_list_.cend(); }

        void print_list(std::ostream& out) const
        {
            auto print_item([&out](const auto& e) { out << e << ", "; });
            //TODO: fix this so it doesn't have a trailing ','.
            
            out << '[';

            for (const auto& e : the_list_)
                std::visit(print_item, e);

            out << "]\n";
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

        py_vector& erase(std::size_t which)
        {
            the_list_.erase(&the_list_[which], &the_list_[which + 1]);
            return *this;
        }

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
            value_type elem;
            auto get_value([&new_values, &elem](const auto& e){ elem = e ; }); 

            for (const auto& r_element : rhs.the_list_)
            {
                std::visit(get_value, r_element);
                new_values.push_back(elem);
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

    protected:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */

    private:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */
        pylist_t the_list_;

}; /* ----------  end of template class py_vector  ---------- */

#endif   /* ----- #ifndef py_vector_INC  ----- */
