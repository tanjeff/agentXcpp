#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <istream>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/**
 * \brief A type with exactly 8 bits
 *
 * The char types in C++ (char, unsigned char, signed char) are defined to have 
 * <em>at least</em> 8 bits. Although these types probably have 8 bits on 
 * virtually every platform, I'm a perfectionist here and define my own type, 
 * so that it can be redefined in one central place.
 */
typedef unsigned char byte_t; // for machines where unsigned char has 8bits

/**
 * \brief input stream for connection to master agent
 *
 * This type is used as input stream which is used to receive protocol messages 
 * from the master agent.
 */
typedef std::basic_istream<byte_t> input_stream;

// A type representing a contigous byte stream (depricated!)
typedef std::basic_string<byte_t> data_t;

#endif
