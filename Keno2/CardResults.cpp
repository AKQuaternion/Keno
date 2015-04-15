//
//  CardResults.cpp
//  Keno2
//
//  Created by Chris Hartman on 4/14/15.
//  Copyright (c) 2015 Chris Hartman. All rights reserved.
//

#include "CardResults.h"
#include <cassert>
#include <vector>
using std::vector;
#include <numeric>
using std::accumulate;
#include <iostream>
using std::cout;
using std::endl;
#include <iomanip>
using std::setw;
using std::setprecision;

const std::vector<std::vector<double>> PayoutResults::_payouts {
//   0 1 2 3   4        5     6       7      8      9
    {},                                                      //0
    {},                                                      //1
    {},                                                      //2
    {},                                                      //3
    {0,0,0,0, 400.0},                                        //4
    {0,0,0,0,   2.50, 1700},                                 //5
    {0,0,0,0,   1.25,   65, 3'500},                          //6
    {0,0,0,0,   0   ,   10,   325, 15'000},                  //7
    {0,0,0,0,   0   ,    4,    80,  1'480, 48'000},          //8
    {0,0,0,0,   0   ,    1,    30,    325,  6'000, 90'000},  //9
};

const std::vector<double> PayoutResults::_costs {0,0,0,0,2,2,1.5,1.5,1.5,1.5};

/*
 4spot: $2, 4/4 pays jackpot $400+ (653)
 5spot: $2, 4/5 $2.50, 5/5 $1700+ (3050)
 6spot: $1.50, 4/6 $1.25, 5/6 $65, 6/6 $3500+ (9800)
 7spot: $1.50, 5/7 $10, 6/7 $325, 7/7 $15000+ (48200)
 8spot: $1.50, 5/8 $4, 6/8 $80, 7/8 $1480, 8/8 $48000+ (230k)
 9spot: $1.50, 5/9 $1, 6/9 $30, 7/9 $325, 8/9 $6000, 9/9 $90000+ (1.25M)
 */

PayoutResults::PayoutResults(int spots, double progressive, const std::vector<BinHits> &possibleBinHits):_progressive(progressive),_possibleBinHits(possibleBinHits)
{
//    auto totalWays = accumulate(begin(_possibleBinHits),end(_possibleBinHits),0,[](auto sum, BinHits bh){return sum+bh.numWays();});
    auto firstCard = _possibleBinHits.front();
    vector<unsigned long> binSizes;
    vector<unsigned long> numBins;
    unsigned long totalOfBinSizes=0;
    for (auto binSet : firstCard.getBinDistributions()) {
        binSizes.push_back(binSet.size()-1);
        numBins.push_back(accumulate(begin(binSet),end(binSet),0));
        totalOfBinSizes += binSizes.back()*numBins.back();
        }
    assert (totalOfBinSizes==80);
    
    if (spots == binSizes.front()) { //One type of bins, fill one bin for progressive
        assert(binSizes.size()==1 || binSizes.size()==2 && binSizes.back()==1);
        _costPerCard = nChoosek((int)numBins[0], 2) * _costs[spots];
        unsigned long long waysHit=0;
        unsigned long long waysMiss=0;
        double totalPayHit = 0;
        double totalPayMiss = 0;
        for (auto card : _possibleBinHits) {
            double payout=0;
            auto bh = card.getBinDistributions()[0];
            for (int first = 0;first<bh.size();++first)
                for(int second = first; second<bh.size();++second) {
                    if (first == second)
                        payout += nChoosek(bh[first],2)*_payouts[spots][first+second];
                    else
                        payout += bh[first]*bh[second]*_payouts[spots][first+second];
                }
            if(bh.back()>=1) {
                payout += progressive-_payouts[spots][spots];
                waysHit += card.numWays();
                totalPayHit += payout * card.numWays();
            }
            else {
                waysMiss += card.numWays();
                totalPayMiss += payout * card.numWays();
            }
            _payoutForCard.push_back(payout);
        }
        assert(waysHit+waysMiss == 3535316142212174320); //80c20
        _EVGivenHitProgressive = (double)totalPayHit/waysHit;
        _EVGivenMissProgressive = (double) totalPayMiss/waysMiss;
        _EV = (double) (totalPayMiss+totalPayHit)/(waysHit+waysMiss);
        _pProgressive = (double) waysHit / (waysHit+waysMiss);
        
    }
    else if (spots == binSizes.front()*2) {//One type of bins, fill two bins for progressive
        assert(binSizes.size()==1 || binSizes.size()==2 && binSizes.back()==1);
        _costPerCard = nChoosek((int)numBins[0], 2) * _costs[spots];
        unsigned long long waysHit=0;
        unsigned long long waysMiss=0;
        double totalPayHit = 0;
        double totalPayMiss = 0;
        for (auto card : _possibleBinHits) {
            double payout=0;
            auto bh = card.getBinDistributions()[0];
            for (int first = 0;first<bh.size();++first)
                for(int second = first; second<bh.size();++second) {
                    if (first == second)
                        payout += nChoosek(bh[first],2)*_payouts[spots][first+second];
                    else
                        payout += bh[first]*bh[second]*_payouts[spots][first+second];
                }
            if(bh.back()>=2) {
                payout += progressive-_payouts[spots][spots];
                waysHit += card.numWays();
                totalPayHit += payout * card.numWays();
            }
            else {
                waysMiss += card.numWays();
                totalPayMiss += payout * card.numWays();
            }
            _payoutForCard.push_back(payout);
//            cout << setw(5) << ++count << "    ";
//            for(auto bd : card.getBinDistributions()) {
//                for(auto i : bd)
//                    cout << setw(4) << i << " ";
//                cout << "    ";
//            }
//            cout << "   " << setw(18) << card.numWays();
//            cout << "   " << payout << endl;
        }
        assert(waysHit+waysMiss == 3535316142212174320); //80c20
        _EVGivenHitProgressive = (double)totalPayHit/waysHit;
        _EVGivenMissProgressive = (double) totalPayMiss/waysMiss;
        _EV = (double) (totalPayMiss+totalPayHit)/(waysHit+waysMiss);
        _pProgressive = (double) waysHit / (waysHit+waysMiss);
    }
    else { //Two types of bins, fill one of each type for progressive.
        assert(binSizes.size()==2 || binSizes.size()==3 && binSizes.back()==1);
        assert(0); //write this code        
    }
}

void PayoutResults::showStats() const
{
        cout << "EV given hit progressive: $" << _EVGivenHitProgressive << endl;
        cout << "EV given miss progressive: $" <<  _EVGivenMissProgressive << endl;
        cout << "EV: $" <<  _EV << endl;
        cout << "Probability to hit progressive: " <<  _pProgressive << endl;
        cout << "Cost per card: $" <<  _costPerCard << endl;
}