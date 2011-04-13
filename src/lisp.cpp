
#include <sstream>
#include <stdexcept>

#include <boost/foreach.hpp>

#include "logging.hpp"

#include "lisp.hpp"
#include "function.hpp"

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
            _global_env.get_symbol("defun")->set_function(
                object_ptr_t(new defun_form()));

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
    }

    object_ptr_t cons_cell::eval(environment* env)
    {
        return env->funcall(m_car, cons_cell_ptr_t(
                                new cons_cell(m_car, m_cdr)));
    }

    bool symbol::is_useless() const
    {
        assert(m_value && m_function && m_property_list);

        if(m_value == nil() && m_function == nil() && m_property_list == nil())
            return true;

        return false;
    }

    object_ptr_t symbol::operator()(environment* env,
                            const cons_cell_ptr_t args)
    {
        return env->funcall(m_function, args);
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
