// Copyright (C) 2003  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_SOCKSTREAMBUF_ABSTRACT_
#ifdef DLIB_SOCKSTREAMBUF_ABSTRACT_

#include <iosfwd>
#include <memory>
#include <streambuf>

#include "../sockets/sockets_kernel_abstract.h"

namespace dlib
{

// ---------------------------------------------------------------------------------------- 

    class sockstreambuf : public std::streambuf
    {
        /*!
            WHAT THIS OBJECT REPRESENTS
                This object represents a stream buffer capable of writing to and
                reading from TCP connections.

            NOTE:
                For a sockstreambuf EOF is when the connection has closed or otherwise
                returned some kind of error.

                Also note that any data written to the streambuf may be buffered 
                internally.  So if you need to ensure that data is actually sent then you 
                should flush the stream.  

                A read operation is guaranteed to block until the number of bytes
                requested has arrived on the connection.  It will never keep blocking
                once enough data has arrived.

            THREADING
                Generally speaking, this object has the same kind of threading
                restrictions as a connection object.  those being:
                - Do not try to write to a sockstreambuf from more than one thread.
                - Do not try to read from a sockstreambuf from more than one thread.
                - You may call shutdown() on the connection object and this will
                  cause any reading or writing calls to end.  To the sockstreambuf it
                  will appear the same as hitting EOF.  (note that EOF for a sockstreambuf
                  means that the connection has closed)
                - It is safe to read from and write to the sockstreambuf at the same time
                  from different threads so long as flushes_output_on_read()==false.
                - It is not safe to try to putback a char and read from the stream from
                  different threads
        !*/
    public:
        sockstreambuf (
            connection* con,
            std::streamsize out_buffer_size = 10000,
            std::streamsize in_buffer_size = 10000,
            std::streamsize max_putback = 4
        );
        /*!
            requires
                - con == a valid connection object
                - out_buffer_size > 0
                - out_buffer_size <= std::numeric_limits<int>::max()
                - in_buffer_size > 0
                - in_buffer_size <= std::numeric_limits<int>::max()
                - max_putback >= 0
                - max_putback < in_buffer_size
            ensures
                - *this will read from and write to con
                - #flushes_output_on_read() == false
                - out_buffer_size bytes are allocated for the internal output buffer
                - in_buffer_size bytes are used to buffer input, with max_putback of those
                  bytes reserved for putback characters
            throws
                - std::bad_alloc
        !*/

        sockstreambuf (
            const std::unique_ptr<connection>& con,
            std::streamsize out_buffer_size = 10000,
            std::streamsize in_buffer_size = 10000,
            std::streamsize max_putback = 4
        );
        /*!
            requires
                - con == a valid connection object
                - out_buffer_size > 0
                - out_buffer_size <= std::numeric_limits<int>::max()
                - in_buffer_size > 0
                - in_buffer_size <= std::numeric_limits<int>::max()
                - max_putback >= 0
                - max_putback < in_buffer_size
            ensures
                - *this will read from and write to con
                - #flushes_output_on_read() == false
                - out_buffer_size bytes are allocated for the internal output buffer
                - in_buffer_size bytes are used to buffer input, with max_putback of those
                  bytes reserved for putback characters
            throws
                - std::bad_alloc
        !*/

        ~sockstreambuf (
        );
        /*!
            requires
                - get_connection() object has not been deleted
            ensures
                - sockstream buffer is destructed but the connection object will 
                  NOT be closed.  
                - Any buffered data is flushed to the connection. 
        !*/

        connection* get_connection (
        );
        /*!
            ensures
                - returns a pointer to the connection object which this buffer
                  reads from and writes to
        !*/

        void flush_output_on_read (
        );
        /*!
            ensures
                - #flushes_output_on_read() == true
        !*/

        bool flushes_output_on_read (
        ) const;
        /*!
            ensures
                - This function returns true if this object will flush its output buffer
                  to the network socket before performing any network read.   
                - if (flushes_output_on_read() == true)
                    - It is not safe to make concurrent read and write calls to this object.
        !*/

        void do_not_flush_output_on_read (
        );
        /*!
            ensures
                - #flushes_output_on_read() == false
        !*/

    };

// ---------------------------------------------------------------------------------------- 

}

#endif // DLIB_SOCKSTREAMBUF_ABSTRACT_

