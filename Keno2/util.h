//
//  util.h
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#ifndef __Keno2__util__
#define __Keno2__util__

#define USE_BOOST
#ifdef USE_BOOST
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include </usr/local/include/boost/multiprecision/cpp_int.hpp>
#pragma clang diagnostic pop

using namespace boost::multiprecision;
using BigInt = cpp_int;
#else
using BigInt = unsigned long long;
#endif

BigInt nChoosek( unsigned n, unsigned k );

BigInt ipower(BigInt a, BigInt n);

#endif /* defined(__Keno2__util__) */
