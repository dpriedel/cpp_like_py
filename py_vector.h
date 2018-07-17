/*
 * =====================================================================================
 *
 *       Filename:  py_list.h
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


#ifndef  py_list_INC
#define  py_list_INC

#include <iosfwd>
#include <vector>
#include <variant>
/*
 * =====================================================================================
 *        Class:  py_list
 *  Description:  provides a Python-like list class for C++
 * =====================================================================================
 */

template <typename ...Ts>
class py_list
{
    public:

        using pylist_t = std::vector<std::variant<Ts...>>;

        /* ====================  LIFECYCLE     ======================================= */
        py_list () { }                                                  /* constructor */
        template<class ...Args>
        py_list (Args ...args) : the_list_{(args) ...} { }              /* constructor */

        py_list(const py_list& rhs) : the_list_{rhs.the_list_} { }
        py_list(py_list&& rhs) : the_list_{std::move(rhs.the_list_)} { }

        /* ====================  ACCESSORS     ======================================= */

        auto begin() { return the_list_.begin(); }
        auto cbegin() { return the_list_.cbegin(); }
        auto end() { return the_list_.end(); }
        auto cend() { return the_list_.cend(); }

        void print_list(std::ostream& out)
        {
            auto print_item([&out](const auto& e) { out << e << ", "; });
            
            out << '[';

            for (const auto& e : the_list_)
                std::visit(print_item, e);

            out << "]\n";
        }

        /* ====================  MUTATORS      ======================================= */

        /* ====================  OPERATORS     ======================================= */

    protected:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */

    private:
        /* ====================  METHODS       ======================================= */

        /* ====================  DATA MEMBERS  ======================================= */
        pylist_t the_list_;

}; /* ----------  end of template class py_list  ---------- */

#endif   /* ----- #ifndef py_list_INC  ----- */
