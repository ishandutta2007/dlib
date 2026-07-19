// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_SOCKStREAMBUF_Hh_
#define DLIB_SOCKStREAMBUF_Hh_

#include <iosfwd>
#include <limits>
#include <memory>
#include <streambuf>
#include "../assert.h"
#include "../sockets.h"
#include "sockstreambuf_abstract.h"
#include "sockstreambuf_unbuffered.h"

namespace dlib
{

// ---------------------------------------------------------------------------------------- 

    class sockstreambuf : public std::streambuf
    {
        /*!
            INITIAL VALUE
                - con == a connection
                - in_buffer == an array of in_buffer_size bytes
                - out_buffer == an array of out_buffer_size bytes

            CONVENTION
                - in_buffer == the input buffer used by this streambuf
                - out_buffer == the output buffer used by this streambuf
                - max_putback == the maximum number of chars to have in the put back buffer.
        !*/

    public:

        // These typedefs are here for backwards compatibility with previous versions of
        // dlib.
        typedef sockstreambuf_unbuffered kernel_1a;
        typedef sockstreambuf kernel_2a;

        sockstreambuf (
            connection* con_,
            std::streamsize out_buffer_size_ = 10000,
            std::streamsize in_buffer_size_ = 10000,
            std::streamsize max_putback_ = 4
        ) :
            con(*con_),
            out_buffer_size(out_buffer_size_),
            in_buffer_size(in_buffer_size_),
            max_putback(max_putback_),
            autoflush(false)
        {
            DLIB_ASSERT(0 < out_buffer_size && out_buffer_size <= std::numeric_limits<int>::max(),
                "out_buffer_size must be in the range (0, INT_MAX]");
            DLIB_ASSERT(0 < in_buffer_size && in_buffer_size <= std::numeric_limits<int>::max(),
                "in_buffer_size must be in the range (0, INT_MAX]");
            DLIB_ASSERT(0 <= max_putback && max_putback < in_buffer_size,
                "max_putback must be in the range [0, in_buffer_size)");

            out_buffer.reset(new char[static_cast<size_t>(out_buffer_size)]);
            in_buffer.reset(new char[static_cast<size_t>(in_buffer_size)]);
            init();
        }

        sockstreambuf (
            const std::unique_ptr<connection>& con_,
            std::streamsize out_buffer_size_ = 10000,
            std::streamsize in_buffer_size_ = 10000,
            std::streamsize max_putback_ = 4
        ) : sockstreambuf(
                con_.get(),
                out_buffer_size_,
                in_buffer_size_,
                max_putback_
            )
        {}

        virtual ~sockstreambuf (
        )
        {
            sync();
        }

        connection* get_connection (
        ) { return &con; }

        void flush_output_on_read()
        {
            autoflush = true;
        }

        bool flushes_output_on_read() const
        {
            return autoflush;
        }

        void do_not_flush_output_on_read()
        {
            autoflush = false;
        }

    protected:

        void init (
        )
        {
            setp(out_buffer.get(), out_buffer.get() + (out_buffer_size-1));
            setg(in_buffer.get()+max_putback,
                 in_buffer.get()+max_putback,
                 in_buffer.get()+max_putback);
        }

        int flush_out_buffer (
        )
        {
            int num = static_cast<int>(pptr()-pbase());
            if (con.write(out_buffer.get(),num) != num)
            {
                // the write was not successful so return EOF 
                return EOF;
            } 
            pbump(-num);
            return num;
        }

        // output functions
        int_type overflow (
            int_type c
        );

        int sync (
        )
        {
            if (flush_out_buffer() == EOF)
            {
                // an error occurred
                return -1;
            }
            return 0;
        }

        std::streamsize xsputn (
            const char* s,
            std::streamsize num
        );

        // input functions
        int_type underflow( 
        );

        std::streamsize xsgetn (
            char_type* s, 
            std::streamsize n
        );

    private:

        // member data
        connection&  con;
        const std::streamsize out_buffer_size;
        const std::streamsize in_buffer_size;
        const std::streamsize max_putback;
        std::unique_ptr<char[]> out_buffer;
        std::unique_ptr<char[]> in_buffer;
        bool autoflush;
    
    };

// ---------------------------------------------------------------------------------------- 

}

#ifdef NO_MAKEFILE
#include "sockstreambuf.cpp"
#endif

#endif // DLIB_SOCKStREAMBUF_Hh_

