//
//  CardResults.h
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#ifndef __Keno2__CardResults__
#define __Keno2__CardResults__

#include <vector>

#include "BinHits.h"

class PayoutResults {
public:
    PayoutResults(int spots, double progressive, const std::vector<BinHits> &);
    void showStats() const;
private:
    const std::vector<BinHits> &_possibleBinHits;
    double _progressive;
    double _EVGivenHitProgressive;
    double _EVGivenMissProgressive;
    double _EV;
    double _pProgressive;
    double _costPerCard;
    std::vector<double> _payoutForCard;
    std::vector<bool> _hitProgressive;
    //double _sd;
    //double _var; // !!!Add these stats
    
    const static std::vector<std::vector<double>> _payouts;
    const static std::vector<double> _costs;
};
//!!! Add standard deviation and variance to this class
#endif /* defined(__Keno2__CardResults__) */
