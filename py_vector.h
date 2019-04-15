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


#ifndef  _PY_VECTOR_INC_
#define  _PY_VECTOR_INC_

#include <algorithm>
#include <any>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>

namespace mp11 = boost::mp11;

// I need to be able to compare 2 variants of different types.
// The problem is that just using '==' leads to combinatorial 
// explosion by the compiler generating lots of options that
// aren't needed (and don't compile).  So the need is to focus on generating smaller
// sets of possible comparisons.
// to avoid confusion, let's put this in a namespace.

namespace cpp_like_py
{
    template<typename ...Ts, typename ...Us>
    bool operator==(const std::variant<Ts ...>& lhs, const std::variant<Us ...>& rhs)
    {
        bool result{false};

        mp11::mp_with_index<sizeof...(Ts)>(lhs.index(), [&](auto I)
        {
            using X = std::variant_alternative_t<I, std::variant<Ts ...>>;
            X x = std::get<I>(lhs);

            mp11::mp_with_index<sizeof...(Us)>(rhs.index(), [&](auto J)
            {
                using Y = std::variant_alternative_t<J, std::variant<Us ...>>;
                Y y = std::get<J>(rhs);

                if constexpr(! std::is_same_v<X, Y>)
                {
                    result = false;
                }
                else
                {
                    result = (x == y);
                }
            });
        });
        return result;
    }
}		/* -----  end of namespace cpp_like_py  ----- */

// let's try some template metaprogramming using boost MP11.
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
        using new_types_set_ = typename mp11::mp_unique<mp11::mp_list<Ts...>>;

template<typename ...Ts>
class py_vector
{
    public:

        using value_type = std::variant<Ts...>;
        using pylist_t = std::vector<value_type>;

        /* ====================  LIFECYCLE     ======================================= */
        py_vector () = default;                                              /* constructor */
        ~py_vector () = default;

        template<typename ...Args>
        explicit py_vector (Args ...args) : the_list_{{args} ...} { }              /* constructor */
        py_vector (std::initializer_list<value_type> values) : the_list_{values} {}

        py_vector(const py_vector& rhs) : the_list_{rhs.the_list_} { }
        py_vector(py_vector&& rhs) noexcept : the_list_{std::move(rhs.the_list_)} { }

        // now, let's try some metaprogramming....
        // NOTE: per "C++ Templates the Complete Guide, 2nd ed." (pp.102,103), this is necessary to force use
        // of customized copy ctor.
        
        py_vector(py_vector const volatile& rhs) = delete;

        template<typename ... Us>
        explicit py_vector(const py_vector<Us...>& rhs)
        {
            // now, make sure the we have all the types the class we are copying from does.
            //
            if constexpr(std::is_same_v<mp11::mp_size<mp11::mp_set_intersection<new_types_set_<Ts...>,
                    new_types_set_<Us...>>>, mp11::mp_size<new_types_set_<Us...>>>)
            {
                // we can not simply copy the variant elements from the rhs vector, we need to use
                // our own type of variant elements which, thanks to check above, we know can handle
                // all the types possible in rhs.
               
                auto copy_value([this](const auto& e)
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
            else
            {
                // just use this to get a more understandable compile error if our type signatures at not compatible.
               
                static_assert(std::is_same_v<std::true_type, std::false_type>,
                        "rhs py_vector type signature must be proper subset to copy construct.");
            }

        }

        template<typename ...Us> friend class py_vector;

        /* ====================  ACCESSORS     ======================================= */

        auto size() const { return the_list_.size(); }
        auto begin() const { return the_list_.begin(); }
        auto cbegin() const { return the_list_.cbegin(); }
        auto end() const { return the_list_.end(); }
        auto cend() const { return the_list_.cend(); }
        auto empty() const { return  the_list_.empty(); }

        void print_list(std::ostream& out) const
        {
            auto print_item([&out](const auto& e) { out << ", " << e ; });
            auto print_first_item([&out](const auto& e) { out << e ; });
            
            out << '[';

            if (! the_list_.empty())
            {
                auto it = the_list_.cbegin();
                std::visit(print_first_item, *it);
                for(++it; it != the_list_.cend(); ++it)
                {
                    std::visit(print_item, *it);
                }
            }
            out << "]\n";
        }

        [[nodiscard]] std::string to_string() const
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

        template<typename Y>
        bool contains(const Y& item) const
        {
            for (const auto& elem : the_list_)
            {
                bool result{false};
                mp11::mp_with_index<sizeof...(Ts)>(elem.index(), [&](auto I)
                {
                    using X = std::variant_alternative_t<I, std::variant<Ts ...>>;
                    if constexpr(std::is_same_v<X, Y>)
                    {
                        const X& x = std::get<I>(elem);
                        result = (x == item);
                    }
                });
                if (result)
                {
                    return true;
                }
            }
            return false;
        }

        // this method will apply the supplied function to all list elements
        // of the specified type.
        
        template<typename T, class F>
        void visit_all(F& func)
        {
            using good_type = mp11::mp_contains<new_types_set_<Ts...>, T>;
            static_assert(std::is_same_v<good_type, mp11::mp_true>, "Type T must be in type signature of py_vector.");

            auto apply_func([func](auto& elem)
            {
                // we the index here rather than type since we can have multiple
                // instances of a type in our type signature.
                
                mp11::mp_with_index<sizeof...(Ts)>(elem.index(), [&](auto I)
                {
                    using X = std::variant_alternative_t<I, std::variant<Ts ...>>;
                    if constexpr (std::is_same_v<T, X>)
                    {
                        // we take a reference here because we may
                        // modify the values in our list.
                        
                        X& x = std::get<I>(elem);
                        func(x);
                    }
                });
            });
            std::for_each(the_list_.begin(), the_list_.end(), apply_func);
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

        py_vector& append(std::initializer_list<value_type> new_values)
        {
            std::copy(new_values.begin(), new_values.end(), std::back_inserter(the_list_));
            return *this;
        }

        template<typename T>
        py_vector& append(const T& element)
        {
            value_type e(std::in_place_type<T>, element);
            the_list_.push_back(e);
            return *this;
        }

        // half open range
        
        py_vector& erase(std::size_t from, std::size_t to)
        {
            the_list_.erase(the_list_.begin() + from, the_list_.begin() + to);
            return *this;
        }

        /* ====================  OPERATORS     ======================================= */

        py_vector& operator=(const py_vector& rhs)
        {
            if (this != &rhs)
            {
                the_list_ = rhs.the_list_;
            }
            return *this;
        }

        template<typename ... Us>
        py_vector& operator=(const py_vector<Us...>& rhs)
        {
            if constexpr(std::is_same_v<mp11::mp_size<mp11::mp_set_intersection<new_types_set_<Ts...>,
                    new_types_set_<Us...>>>, mp11::mp_size<new_types_set_<Us...>>>)
            {
                // we can not simply copy the variant elements from the rhs vector, we need to use
                // our own type of variant elements which, thanks to check above, we know can handle
                // all the types possible in rhs.
               
                // a little bit of exception safety.
                
                pylist_t new_values;
                
                auto copy_value([&new_values](const auto& e)
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
            else
            {
                // just use this to get a more understandable compile error if our type signatures at not compatible.
               
                static_assert(std::is_same_v<std::true_type, std::false_type>,
                        "rhs py_vector type signature must be proper subset to copy assign.");
            }
        }

        py_vector& operator=(py_vector&& rhs) noexcept
        {
            if (this != &rhs)
            {
                the_list_ = std::move(rhs.the_list_);
            }
            return *this;
        }

        py_vector& operator+=(const py_vector& rhs)
        {
            if (this != &rhs)
            {
                this->append(rhs);
            }
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
        
        template<typename ... Us>
        bool operator==(const py_vector<Us...>& rhs) const
        {
            if constexpr(std::is_same_v<mp11::mp_size<mp11::mp_set_intersection<new_types_set_<Ts...>,
                    new_types_set_<Us...>>>, mp11::mp_size<new_types_set_<Us...>>>)
            {
                // It appears I now have a proper way to compare two variants with different type signatures.

                auto compare_elements([](const auto& a, const auto& b)
                {
                    return cpp_like_py::operator==(a, b);
                });
                return std::equal(the_list_.cbegin(), the_list_.cend(), rhs.the_list_.cbegin(), compare_elements);
            }
            else
            {
                // just use this to get a more understandable compile error if our type signatures at not compatible.
               
                static_assert(std::is_same_v<std::true_type, std::false_type>,
                        "rhs py_vector type signature must be proper subset to test equivalence.");

                return false;
            }
        }

    protected:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */

    private:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */
        pylist_t the_list_;

}; /* ----------  end of template class py_vector  ---------- */

#endif   /* ----- #ifndef PY_VECTOR_INC  ----- */
