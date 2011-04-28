/*
 * Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
 *
 * This file is part of the agentXcpp library.
 *
 * AgentXcpp is free software: you can redistribute it and/or modify
 * it under the terms of the AgentXcpp library license, version 1, which 
 * consists of the GNU General Public License and some additional 
 * permissions.
 *
 * AgentXcpp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the AgentXcpp library license in the LICENSE file of this package 
 * for more details.
 */

// we link the testsuite dynamically (must come before the #include)
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <cmath>

#include <Integer.h>




BOOST_AUTO_TEST_CASE( init_constructor_and_get_value )
{
    agentxcpp::Integer object1(13);
    BOOST_CHECK_EQUAL( object1.get_value(), 13 );
    
    agentxcpp::Integer object2(0);
    BOOST_CHECK_EQUAL( object2.get_value(), 0 );
    
    long long max = std::pow(2,32)-1; // maximum value for uint32_t

    agentxcpp::Integer object3(max); // -1 should wrap to (2^32)-1
    BOOST_CHECK_EQUAL( object3.get_value(), max);
    
    agentxcpp::Integer object4(-1); // -1 should wrap to max!
    BOOST_CHECK_EQUAL( object4.get_value(), max );
}

BOOST_AUTO_TEST_CASE( serialize )
{
    agentxcpp::Integer object(0x11223344);
    data_t blob = object.serialize();
    
    // We expect big endian!
    BOOST_CHECK_EQUAL(blob.size(), 4);
    BOOST_CHECK_EQUAL(blob[0], 0x11);
    BOOST_CHECK_EQUAL(blob[1], 0x22);
    BOOST_CHECK_EQUAL(blob[2], 0x33);
    BOOST_CHECK_EQUAL(blob[3], 0x44);
}
