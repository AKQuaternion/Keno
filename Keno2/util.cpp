//
//  util.cpp
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#include "util.h"
#include <numeric>

BigInt nChoosek( int n, int k )
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;
    
    BigInt result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

BigInt iPower(BigInt a, unsigned long long n) {
    BigInt res = 1;
    while (n) {
        if (n & 1)
            res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}

BigInt multi(const std::vector<int> &cs)
{
    int sumOfCs = std::accumulate(begin(cs),end(cs),0);
    BigInt m{1};
    for (auto c:cs) {
        m *= nChoosek(sumOfCs, c);
        sumOfCs -= c;
    }
    return m;
}