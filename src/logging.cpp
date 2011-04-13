/* This program listens on a directory for changes and applies them
 * to another location, too.
 * Copyright (C) 2010  Dominik Burgdörfer <dominik.burgdoerfer@googlemail.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */


#include "logging.hpp"

#include <boost/shared_ptr.hpp>

//#include "exceptions.hpp"


namespace logging {
    using boost::shared_ptr;
    
    class DummyStream : public std::ostream
    {
    public:
        DummyStream()
            : std::ostream(0)
            {
            }
    };

    std::ostream* __log_stream;
    std::ostream* __dbg_stream;

    void init(std::ostream& log_stream)
    {
        __log_stream = &log_stream;

#ifdef NDEBUG
        __dbg_stream = new DummyStream;
#else
        __dbg_stream = __log_stream;
#endif
    }

    void close()
    {
#ifdef NDEBUG
        delete __dbg_stream;
#endif
    }

    std::ostream& log(Level level)
    {
        using boost::shared_ptr;

        assert(level == DEBUG ||
                  level == INFO ||
                  level == WARNING ||
                  level == ERROR ||
                  level == CRITICAL);
        assert(__log_stream);
        assert(__dbg_stream);

        switch(level) {
        case DEBUG:
            *__dbg_stream << "[DEBUG]   ";
            break;
        case INFO:
            *__log_stream << "[INFO]    ";
            break;
        case WARNING:
            *__log_stream << "[WARNING]  ";
            break;
        case ERROR:
            *__log_stream << "[ERROR]    ";
            break;
        case CRITICAL:
            *__log_stream << "[CRITICAL] ";
            break;
        }

        if(level == DEBUG)
            return *__dbg_stream;

        return *__log_stream;
    }
}
