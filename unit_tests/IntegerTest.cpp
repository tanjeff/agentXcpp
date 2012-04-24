/*
 * Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

#include <Integer.hpp>




BOOST_AUTO_TEST_CASE( init_constructor_and_get_value )
{
    agentxcpp::Integer object_1(13);
    BOOST_CHECK_EQUAL( object_1.get_value(), 13 );
    
    agentxcpp::Integer object_2(0);
    BOOST_CHECK_EQUAL( object_2.get_value(), 0 );
    
    long long max = std::pow(2,32)-1; // maximum value for uint32_t

    agentxcpp::Integer object_3(max);
    BOOST_CHECK_EQUAL( object_3.get_value(), max);
    
    agentxcpp::Integer object_4(-1); // -1 should wrap to max!
    BOOST_CHECK_EQUAL( object_4.get_value(), max );
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

BOOST_AUTO_TEST_CASE( parse_constructor )
{
    // Create some serialized data
    data_t blob;
    blob.push_back(0x11);
    blob.push_back(0x22);
    blob.push_back(0x33);
    blob.push_back(0x44);
    blob.push_back(0x55);
    blob.push_back(0x66);
    blob.push_back(0x77);

    data_t::const_iterator begin;
    data_t::const_iterator end   = blob.end();
    
    // Create Integer from beginning, little endian
    begin = blob.begin();
    agentxcpp::Integer object_1(begin, end, false);
    BOOST_CHECK_EQUAL(object_1.get_value(), 0x44332211);
    BOOST_CHECK_EQUAL(*begin, *(blob.begin()+4));
    
    // Create Integer from beginning, big endian
    begin = blob.begin();
    agentxcpp::Integer object_2(begin, end);
    BOOST_CHECK_EQUAL(object_2.get_value(), 0x11223344);
    BOOST_CHECK_EQUAL(*begin, *(blob.begin()+4));
    
    // Create Integer starting at a position where offset%4 != 0
    begin = blob.begin() + 1;
    agentxcpp::Integer object_3(begin, end);
    BOOST_CHECK_EQUAL(object_3.get_value(), 0x22334455);
    BOOST_CHECK_EQUAL(*begin, *(blob.begin()+5));
    
    // Create Integer starting at the last possible position
    begin = blob.begin() + 3;
    agentxcpp::Integer object_4(begin, end);
    BOOST_CHECK_EQUAL(object_4.get_value(), 0x44556677);
    BOOST_CHECK_EQUAL(*begin, *(blob.begin()+7));
    
    // Create Integer starting near the end, so less than 4 bytes are available 
    // for parsing. The constructor should throw a parse_error excection.
    begin = blob.begin() + 5;
    BOOST_CHECK_THROW( agentxcpp::Integer object_5(begin, end),
		       parse_error )
}

BOOST_AUTO_TEST_CASE( set_value )
{
    // We use this object for the tests
    agentxcpp::Integer object(0);

    object.set_value(13);
    BOOST_CHECK_EQUAL( object.get_value(), 13 );
    
    object.set_value(0);
    BOOST_CHECK_EQUAL( object.get_value(), 0 );
    
    long long max = std::pow(2,32)-1; // maximum value for uint32_t

    object.set_value(max);
    BOOST_CHECK_EQUAL( object.get_value(), max);
    
    object.set_value(-1); // -1 should wrap to max!
    BOOST_CHECK_EQUAL( object.get_value(), max );
}
