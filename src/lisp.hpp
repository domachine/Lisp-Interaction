
#ifndef LISP_HPP
#define LISP_HPP

#include <vector>
#include <map>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "logging.hpp"
#include "types.hpp"


namespace lisp {
    /**
       @brief Represents the nil object. Always returns the
       pointer to same object. @c nil is unique.
    */
    const object_ptr_t nil();

    const object_ptr_t t();

    environment* global_env();


    class t_object : public object
    {
    public:
        operator bool() const
            {
                return true;
            }

        std::string str() const
            {
                return "t";
            }

        friend const object_ptr_t t();

    private:
        t_object()
            : object()
            {
            }
    };

    class nil_object : public object
    {
    public:
        operator bool() const
            {
                return false;
            }

        std::string str() const
            {
                return "nil";
            }

        friend const object_ptr_t nil();

    private:
        nil_object()
            : object()
            {
            }
    };

    class cons_cell : public object
    {
    public:
        cons_cell(object_ptr_t car = nil(),
                  object_ptr_t cdr = nil())
            : object(),
              m_car(car),
              m_cdr(cdr)
            {
                assert(car && cdr);
            }

        object_ptr_t car() const
            {
                return m_car;
            }

        object_ptr_t cdr() const
            {
                return m_cdr;
            }

        bool empty() const
            {
                return m_car == nil() && m_cdr == nil();
            }

        bool is_cons_cell() const
            {
                return true;
            }

        std::string str() const
            {
                std::stringstream os;

                os << "(" << m_car->str();

                if(m_cdr->is_cons_cell()) {
                    cons_cell_ptr_t cell = boost::dynamic_pointer_cast<cons_cell>(m_cdr);

                    while(cell && *cell) {
                        os << " " << cell->car()->str();

                        cell = boost::dynamic_pointer_cast<lisp::cons_cell>(cell->cdr());
                    }
                }
                else
                    os << " . " << m_cdr->str();

                os << ")";

                return os.str();
            }

    protected:
        object_ptr_t eval(environment* env);

    private:
        object_ptr_t m_car;
        object_ptr_t m_cdr;
    };

    // class environment;

    class symbol : public object
    {
    public:
        friend class environment;

        const std::string& name() const
            {
                return m_name;
            }

        object_ptr_t value() const
            {
                assert(m_value);
                return m_value;
            }

        object_ptr_t function() const
            {
                assert(m_function);
                return m_function;
            }

        object_ptr_t property_list() const
            {
                assert(m_property_list);
                return m_property_list;
            }

        void set_value(object_ptr_t obj)
            {
                m_value = obj;
            }

        void set_function(object_ptr_t obj)
            {
                m_function = obj;
            }

        /**
           @brief Returns the environment in which the object
           lives.
        */
        environment* env() const
            {
                return m_env;
            }

        void set_env(environment* env)
            {
                m_env = env;
            }

        bool is_symbol() const
            {
                return true;
            }

        /**
           @brief Indicates whether the symbol is useless
           which means that all cells are empty or @c nil.

           If this happens the symbol will be proposed to the
           garbage collector.
        */
        bool is_useless() const;

        std::string str() const
            {
                return m_name;
            }

    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t());

        object_ptr_t eval(environment*)
            {
                return m_value;
            }

    private:
        symbol()
            {
                // This should not happen.
                assert(false);
            }

        symbol(environment* env, const std::string& name)
            : object(),
              m_name(name),
              m_value(nil()),
              m_function(nil()),
              m_property_list(nil()),
              m_env(env)
            {
            }

        // Disable copying
        symbol(const symbol&)
            : object()
            {
                assert(false);
            }

        std::string m_name;
        object_ptr_t m_value;
        object_ptr_t m_function;
        object_ptr_t m_property_list;
        environment* m_env;
    };

    class quote : public object
    {
    public:
        /**
           @brief Constructs a new quote object.

           Simply holds the given object.
        */
        quote(object_ptr_t obj)
            : object(),
              m_object(obj)
            {
                assert(m_object);
            }

    protected:
        /**
           @brief On evaluation simply return the
           object.

           @return A pointer to the hold object.
        */
        object_ptr_t eval(environment*)
            {
                return m_object;
            }

    private:
        object_ptr_t m_object;
    };

    typedef boost::shared_ptr<symbol> symbol_ptr_t;

    void signal(symbol_ptr_t err_sym, const std::string& what);

    /**
       @brief Handles a symbol table and takes care
       that the symbols are destroyed if they aren't needed
       anymore.

       @see symbol::is_useless().
    */
    class environment
    {
        struct deleter
        {
            void operator()(symbol* sym);
        };

    public:
        typedef std::pair<symbol*, int> symbol_entry_t;
        typedef std::map<std::string, symbol_entry_t> symbol_table_t;

        environment(environment* parent = 0);
        ~environment();

        /**
           @brief Creates symbol `name' explicitely without checking
           parent.

           Should be used for creating let-like environments only.
           In other contexts it doesn't make sense. Use get_symbol()
           instead.

           @throws std::logic_error if the symbol already exists.
        */
        symbol_ptr_t create_symbol(const std::string& name);

        /**
           @brief Checks if the named symbol exists otherwise
           creates a new instance and returns a controlled pointer to
           it.
        */
        symbol_ptr_t get_symbol(const std::string& name);

        object_ptr_t eval(object_ptr_t obj);
        object_ptr_t funcall(object_ptr_t obj,
                             const cons_cell_ptr_t args = cons_cell_ptr_t());

    private:
        void del_ref(const std::string& name);

        symbol_table_t m_symbols;

        environment* m_parent;
    };
}

#endif  // LISP_HPP
