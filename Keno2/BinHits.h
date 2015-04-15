//
//  BinHits.h
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#ifndef __Keno2__BinHits__
#define __Keno2__BinHits__
#include <vector>
#include "util.h"

//#include <utility>
//using BinCountsAndSizes =std::vector<std::pair<int,int>>;
//    BinCountsAndSizes _binCountsAndSizes;

using CountDistributions = std::vector<int>;
using BinDistributions = std::vector<CountDistributions>;

class BinHits
{
public:
    BinHits(const BinDistributions&);
    BigInt numWays() const;
    BinDistributions getBinDistributions() const;
private:
    BinDistributions _binDistributions;
    BigInt _numWays;
};

#endif /* defined(__Keno2__BinHits__) */
