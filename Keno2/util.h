//
//  util.h
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#ifndef __Keno2__util__
#define __Keno2__util__
#include <vector>

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

unsigned long long nChoosek( int n, int k );

unsigned long long iPower(BigInt a, unsigned long long n);

unsigned long long multi(const std::vector<int> &cs);

#endif /* defined(__Keno2__util__) */
