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

/*
 * =====================================================================================
 *        Class:  py_vector
 *  Description:  provides a Python-like list class for C++
 * =====================================================================================
 */

template <typename ...Ts>
class py_vector
{
    public:

        using value_type = std::variant<Ts...>;
        using pylist_t = std::vector<value_type>;

        /* ====================  LIFECYCLE     ======================================= */
        py_vector () { }                                                  /* constructor */
        template<class ...Args>
        py_vector (Args ...args) : the_list_{{args} ...} { }              /* constructor */

        py_vector(const py_vector& rhs) : the_list_{rhs.the_list_} { }
        py_vector(py_vector&& rhs) : the_list_{std::move(rhs.the_list_)} { }

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
                std::copy(rhs.the_list_.begin(), rhs.the_list_.end(), std::back_inserter(the_list_));
            }
            return *this;
        }

        template<typename T>
        py_vector& append(const T& element)
        {
            value_type e{std::in_place_type<T>, element};
            the_list_.push_back(e);
            return *this;
        }

        /* ====================  OPERATORS     ======================================= */

        py_vector& operator=(const py_vector& rhs)
        {
            if (this != &rhs)
                the_list_ = rhs.the_list_;
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
