#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/foreach.hpp>

#include "logging.hpp"

#include "lisp.hpp"
#include "function.hpp"
#include "forms.hpp"

namespace lisp {
    namespace {
        // The unique nil object. Is returned by nil().
        object_ptr_t _nil_object;
        object_ptr_t _t_object;

        environment _global_env;
        bool _global_env_initialized = false;
    }


    const object_ptr_t nil()
    {
        if(!_nil_object)
            _nil_object = object_ptr_t(new nil_object);

        return _nil_object;
    }

    const object_ptr_t t()
    {
        if(!_t_object)
            _t_object = object_ptr_t(new t_object);

        return _t_object;
    }

    environment* global_env()
    {
        if(!_global_env_initialized) {

            // Register default forms.
            _global_env.get_symbol("lambda")->set_function(
                object_ptr_t(new lambda_form()));
            _global_env.get_symbol("if")->set_function(
                object_ptr_t(new if_form()));
            _global_env.get_symbol("or")->set_function(
                object_ptr_t(new or_form()));
            _global_env.get_symbol("and")->set_function(
                object_ptr_t(new and_form()));
            _global_env.get_symbol("print")->set_function(
                object_ptr_t(new print_function()));
            _global_env.get_symbol("fset")->set_function(
                object_ptr_t(new fset_form()));
            _global_env.get_symbol("setf")->set_function(
                object_ptr_t(new setf_form()));
            _global_env.get_symbol("setq")->set_function(
                object_ptr_t(new setq_form()));
            _global_env.get_symbol("defun")->set_function(
                object_ptr_t(new defun_form()));
            _global_env.get_symbol("equal")->set_function(
                object_ptr_t(new equal_form()));
            _global_env.get_symbol("+")->set_function(
                object_ptr_t(new plus_form()));

            _global_env_initialized = true;
        }

        return &_global_env;
    }


    namespace {

        // Helper functions to make it more convienient to
        // get access to the pointer or the reference count of a object from
        // an entry.

        inline symbol* entry_pointer(environment::symbol_table_t::iterator iter)
        {
            return iter->second.first;
        }

        inline int& entry_refcount(environment::symbol_table_t::iterator iter)
        {
            return iter->second.second;
        }
    }  // Anonymous namespace

    void signal(symbol_ptr_t err_sym, const std::string& what)
    {
        std::cerr << err_sym->name() << " " << what << std::endl;

        /*
          FIXME: Add a real catchable exception. To build a system
          comparable to emacs try-catch mechanism.
        */
        throw;
    }

    cons_cell::cons_cell(object_ptr_t car, object_ptr_t cdr)
        : object(),
          m_car(car),
          m_cdr(cdr)
    {
        assert(car && cdr);
    }

    object_ptr_t cons_cell::car() const
    {
        return m_car;
    }

    object_ptr_t cons_cell::cdr() const
    {
        return m_cdr;
    }

    bool cons_cell::empty() const
    {
        return m_car == nil() && m_cdr == nil();
    }

    bool cons_cell::is_cons_cell() const
    {
        return true;
    }

    std::string cons_cell::str() const
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
        else if(m_cdr != nil())
            os << " . " << m_cdr->str();

        os << ")";

        return os.str();
    }

    object_ptr_t cons_cell::eval(environment* env)
    {
        object_ptr_t func = m_car;

        if(m_car->is_cons_cell()) {
            cons_cell_ptr_t car_cell = boost::dynamic_pointer_cast<cons_cell>(m_car);

            if(car_cell->car()->is_symbol_ref() &&
               boost::dynamic_pointer_cast<symbol_ref>(car_cell->car())->name() == "lambda")
                func = env->eval(car_cell);
        }

        return env->funcall(func, cons_cell_ptr_t(
                                new cons_cell(m_car, m_cdr)));
    }

    object_ptr_t symbol::value() const
    {
        if(!m_value)
            signal(m_env->get_symbol("void-variable"), m_name);

        return m_value;
    }

    object_ptr_t symbol::function() const
    {
        if(!m_function)
            signal(m_env->get_symbol("void-function"), m_name);

        return m_function;
    }

    bool symbol::is_useless() const
    {
        assert(m_property_list);

        if((!m_value || m_value == nil()) &&
           (!m_function || m_function == nil()) && m_property_list == nil())
           //&& !m_gc_flag)
            return true;

        return false;
    }

    object_ptr_t symbol::operator()(environment* env,
                            const cons_cell_ptr_t args)
    {
        if(m_function && *m_function)
            return env->funcall(m_function, args);
        else
            return object_ptr_t();
    }

    object_ptr_t symbol_ref::eval(environment* env)
    {
        symbol_ptr_t sym = env->get_symbol(m_name);

        return env->eval(sym);
    }

    object_ptr_t symbol_ref::operator()(environment* env,
                                        const cons_cell_ptr_t args)
    {
        symbol_ptr_t sym = env->get_symbol(m_name);

        return (*sym)(env, args);
    }

    void environment::deleter::operator()(symbol* sym)
    {
        if(sym != 0) {
            assert(sym->env());
            sym->env()->del_ref(sym->name());
        }
    }

    environment::environment(environment* parent)
        : m_parent(parent)
    {
    }

    environment::~environment()
    {
        BOOST_FOREACH(symbol_table_t::value_type& c, m_symbols) {
            if(m_parent && c.second.second > 0)
                // Enable closures and append to parent.
                c.second.first->set_env(m_parent);
            else
                // This is the uppermost context and program will
                // exit and the reference count is 0.
                delete c.second.first;
        }
    }

    symbol_ptr_t environment::create_symbol(const std::string& name)
    {
        symbol_table_t::iterator iter = m_symbols.find(name);
        symbol* sym_ptr = 0;

        if(iter == m_symbols.end()) {
            // Allocate memory for a new symbol.
            sym_ptr = new symbol(this, name);

            symbol_entry_t sym_entry(sym_ptr, 1);

            m_symbols.insert(m_symbols.begin(),
                             std::pair<std::string, symbol_entry_t>(name, sym_entry));
        }
        else
            // Invalid usage of the method.
            throw std::logic_error("symbol already exists: " + name);

        symbol_ptr_t new_sym = symbol_ptr_t(sym_ptr, deleter());

        return new_sym;
    }

    symbol_ptr_t environment::get_symbol(const std::string& name)
    {
        symbol_table_t::iterator iter = m_symbols.find(name);
        symbol* sym_ptr = 0;

        if(iter == m_symbols.end()) {
            if(m_parent) {
                // Check parent.

                symbol_ptr_t sym = m_parent->get_symbol(name);

                if(sym)
                    return sym;
            }

            // Allocate memory for a new symbol.
            sym_ptr = new symbol(this, name);

            symbol_entry_t sym_entry(sym_ptr, 1);

            m_symbols.insert(m_symbols.begin(),
                             std::pair<std::string, symbol_entry_t>(name, sym_entry));
        }
        else {
            // Increase ref_count.
            ++entry_refcount(iter);

            sym_ptr = entry_pointer(iter);
        }

        symbol_ptr_t new_sym = symbol_ptr_t(sym_ptr, deleter());

        return new_sym;
    }

    void environment::del_ref(const std::string& name) 
    {
        symbol_table_t::iterator iter = m_symbols.find(name);

        if(iter == m_symbols.end())
            return;

        assert(entry_refcount(iter) > 0);

        --entry_refcount(iter);

        // Refcount is 0 -> Do garbage collection.
        if(entry_refcount(iter) <= 0 && entry_pointer(iter)->is_useless()) {
            delete entry_pointer(iter);

            m_symbols.erase(iter);
        }
    }

    object_ptr_t environment::eval(object_ptr_t obj)
    {
        object_ptr_t r = obj->eval(this);

        if(r)
            return r;
        else
            return obj;
    }

    object_ptr_t environment::funcall(object_ptr_t obj, const cons_cell_ptr_t args)
    {
        object_ptr_t r = (*obj)(this, args);

        if(r)
            return r;
        else {
            signal(get_symbol("invalid-function"),
                   obj->str());

            // Shouldn't be reached.
            return nil();
        }
    }
}
