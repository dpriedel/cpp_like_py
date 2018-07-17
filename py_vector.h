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

        using pylist_t = std::vector<std::variant<Ts...>>;

        /* ====================  LIFECYCLE     ======================================= */
        py_vector () { }                                                  /* constructor */
        template<class ...Args>
        py_vector (Args ...args) : the_list_{(args) ...} { }              /* constructor */

        py_vector(const py_vector& rhs) : the_list_{rhs.the_list_} { }
        py_vector(py_vector&& rhs) : the_list_{std::move(rhs.the_list_)} { }

        /* ====================  ACCESSORS     ======================================= */

        auto begin() { return the_list_.begin(); }
        auto cbegin() { return the_list_.cbegin(); }
        auto end() { return the_list_.end(); }
        auto cend() { return the_list_.cend(); }

        void print_list(std::ostream& out)
        {
            auto print_item([&out](const auto& e) { out << e << ", "; });
            //TODO: fix this so it doesn't have a trailing ','.
            
            out << '[';

            for (const auto& e : the_list_)
                std::visit(print_item, e);

            out << "]\n";
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
            the_list_.emplace_back(element);
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

    protected:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */

    private:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */
        pylist_t the_list_;

}; /* ----------  end of template class py_vector  ---------- */

#endif   /* ----- #ifndef py_vector_INC  ----- */
