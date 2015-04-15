//
//  BinHits.cpp
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#include "BinHits.h"
#include <string>
#include <numeric>
#include <exception>

BinHits::BinHits(const BinDistributions& binDistributions):_binDistributions(binDistributions)
{
    int totalBinHits = 0;
    int totalBinSizes = 0;
    _numWays = 1;
    for(const auto & bd:_binDistributions) {
        auto maxBinHits = static_cast<int>(bd.size()-1);
        totalBinSizes += maxBinHits*std::accumulate(begin(bd),end(bd),0);
        _numWays *= multi(bd);
        for (int binHits=0;binHits <= maxBinHits;++binHits) {
            totalBinHits += binHits*bd[binHits];
            _numWays *= iPower(nChoosek(maxBinHits, binHits),bd[binHits]);
        }
    }
    if (totalBinHits != 20) {
        throw std::runtime_error("BinHits contructor passed BinDistribution with " + std::to_string(totalBinHits) + " balls, should always be 20");
    }
    if (totalBinSizes != 80) {
        throw std::runtime_error("BinHits contructor passed BinDistribution with " + std::to_string(totalBinSizes) + " total bin sizes, should always be 80");
    }
}

BinDistributions BinHits::getBinDistributions() const
{
    return _binDistributions;
}

BigInt BinHits::numWays() const
{
    return _numWays;
}
