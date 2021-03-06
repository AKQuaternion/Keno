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
#include <sstream>
using std::ostringstream;
#include <memory>
using std::unique_ptr;
using std::make_unique;
#include <string>
using std::string;

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

unique_ptr<PayoutResults> PayoutResults::PayoutResultsFactory(int spots, double progressive, const std::vector<BinHits> &bh)
{
//    auto firstCard = _possibleBinHits.front();
//    unsigned long totalOfBinSizes=0;
//    for (auto binSet : firstCard.getBinDistributions()) {
//        _binSizes.push_back(binSet.size()-1);
//        _numBins.push_back(accumulate(begin(binSet),end(binSet),0));
//        totalOfBinSizes += _binSizes.back()*_numBins.back();
//    }
    unique_ptr<PayoutResults> pr;
    if (spots == bh.front().getBinDistributions().front().size()-1) //One type of bins, fill one bin for progressive
         pr = make_unique<OneBinPayoutResults>(spots,progressive,bh);
    else if (spots == (bh.front().getBinDistributions().front().size()-1)*2) //One type of bins, fill two bins for progressive
        pr = make_unique<OneBinDoubledPayoutResults>(spots,progressive,bh);
    else //Two types of bins, fill one of each type for progressive.
        pr = make_unique<TwoBinsPayoutResults>(spots,progressive,bh);
    pr->calculateStats();
    return pr;
}

const std::vector<unsigned long> & PayoutResults::getBinSizes() const
{
    return _binSizes;
}

const std::vector<unsigned long> & PayoutResults::getNumBins() const
{
    return _numBins;
}

void OneBinPayoutResults::checkBinSizes() const
{
    assert(getBinSizes().size()==1 || getBinSizes().size()==2 && getBinSizes().back()==1);
}
        
string OneBinPayoutResults::gameDescription() const
{
    ostringstream os;
    os << "Playing " << _spots << " spots on card with " << getNumBins()[0] << " groups of " << _spots << ", progressive is $" << _progressive;
    return os.str();
}

unsigned long OneBinPayoutResults::playsPerCard() const
{
    return getNumBins()[0];
}
        
double OneBinPayoutResults::calculatePayout(const BinHits &card) const
{
    double payout = 0;
    auto bh = card.getBinDistributions()[0];
    for (int first = 0;first<bh.size();++first)
        payout += bh[first]*_payouts[_spots][first];
    return payout;
}

bool OneBinPayoutResults::hitJackpot(const BinHits &card) const
{
    auto bh = card.getBinDistributions()[0];
    return bh.back()>=1;
}

void OneBinDoubledPayoutResults::checkBinSizes() const
{
    assert(getBinSizes().size()==1 || getBinSizes().size()==2 && getBinSizes().back()==1);
}
        
string OneBinDoubledPayoutResults::gameDescription() const
{
    ostringstream os;
    os << "Playing " << _spots << " spots on card with " << getNumBins()[0] << " groups of " << getBinSizes()[0]
       << ", progressive is $" << _progressive;
    return os.str();
}

unsigned long OneBinDoubledPayoutResults::playsPerCard() const
{
    auto n = getNumBins()[0];
    return n*(n-1)/2;
//    return nChoosek((int)getNumBins()[0], 2);
}
        
double OneBinDoubledPayoutResults::calculatePayout(const BinHits &card) const
{
    double payout=0;
    auto bh = card.getBinDistributions()[0];
    for (int first = 0;first<bh.size();++first)
        for(int second = first; second<bh.size();++second) {
            if (first == second)
                payout += (double)nChoosek(bh[first],2)*_payouts[_spots][first+second];
            else
                payout += bh[first]*bh[second]*_payouts[_spots][first+second];
        }
    return payout;
}

bool OneBinDoubledPayoutResults::hitJackpot(const BinHits &card) const
{
    auto bh = card.getBinDistributions()[0];
    return bh.back()>=2;
}
        
void TwoBinsPayoutResults::checkBinSizes() const
{
    assert(getBinSizes().size()==2 || getBinSizes().size()==3 && getBinSizes().back()==1);
}

string TwoBinsPayoutResults::gameDescription() const
{
    ostringstream os;
    os << "Playing " << _spots << " spots on card with " << getNumBins()[0] << " groups of " << getBinSizes()[0]
       << " and " << getNumBins()[1] << " groups of " << getBinSizes()[1] << ", progressive is $" << _progressive;
    return os.str();
}

unsigned long TwoBinsPayoutResults::playsPerCard() const
{
    return getNumBins()[0] * getNumBins()[1];
}

double TwoBinsPayoutResults::calculatePayout(const BinHits &card) const
{
    double payout=0;
    auto bh1 = card.getBinDistributions()[0];
    auto bh2 = card.getBinDistributions()[1];
    for (int first = 0;first<bh1.size();++first)
        for(int second = 0; second<bh2.size();++second)
            payout += bh1[first]*bh2[second]*_payouts[_spots][first+second];
    return payout;
}

bool TwoBinsPayoutResults::hitJackpot(const BinHits &card) const
{
    auto bh1 = card.getBinDistributions()[0];
    auto bh2 = card.getBinDistributions()[1];
    return (bh1.back()>=1 && bh2.back()>=1);
}

void PayoutResults::calculateStats()
{
    checkBinSizes();
    _gameDescription = gameDescription();
    _costPerCard = playsPerCard() * _costs[_spots];
    unsigned long long waysHit=0;
    unsigned long long waysMiss=0;
    double totalPayHit = 0;
    double totalPayMiss = 0;
    for (auto card : _possibleBinHits) {
        double payout= calculatePayout(card);
        if(hitJackpot(card)) {
            payout += _progressive-_payouts[_spots][_spots];
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


PayoutResults::PayoutResults(int spots, double progressive, const std::vector<BinHits> &possibleBinHits):_spots(spots),_progressive(progressive),_possibleBinHits(possibleBinHits)
{
    auto firstCard = _possibleBinHits.front();
    unsigned long totalOfBinSizes=0;
    for (auto binSet : firstCard.getBinDistributions()) {
        _binSizes.push_back(binSet.size()-1);
        _numBins.push_back(accumulate(begin(binSet),end(binSet),0));
        totalOfBinSizes += _binSizes.back()*_numBins.back();
    }
    assert (totalOfBinSizes==80);
////
////        assert(binSizes.size()==1 || binSizes.size()==2 && binSizes.back()==1);
////        os << "Playing " << spots << " spots on card with " << numBins[0] << " groups of " << spots << ", progressive is $" << _progressive;
////        _gameDescription = os.str();
////        _costPerCard = numBins[0] * _costs[spots];
////        unsigned long long waysHit=0;
////        unsigned long long waysMiss=0;
////        double totalPayHit = 0;
////        double totalPayMiss = 0;
////        for (auto card : _possibleBinHits) {
////            double payout=0;
////            auto bh = card.getBinDistributions()[0];
////            for (int first = 0;first<bh.size();++first)
////                payout += bh[first]*_payouts[spots][first];
////            if(bh.back()>=1) {
////                payout += progressive-_payouts[spots][spots];
////                waysHit += card.numWays();
////                totalPayHit += payout * card.numWays();
////            }
////            else {
////                waysMiss += card.numWays();
////                totalPayMiss += payout * card.numWays();
////            }
////            _payoutForCard.push_back(payout);
////        }
////        assert(waysHit+waysMiss == 3535316142212174320); //80c20
////        _EVGivenHitProgressive = (double)totalPayHit/waysHit;
////        _EVGivenMissProgressive = (double) totalPayMiss/waysMiss;
////        _EV = (double) (totalPayMiss+totalPayHit)/(waysHit+waysMiss);
////        _pProgressive = (double) waysHit / (waysHit+waysMiss);
////        
////    }
//    else if (spots == binSizes.front()*2) {//One type of bins, fill two bins for progressive
//        assert(binSizes.size()==1 || binSizes.size()==2 && binSizes.back()==1);
//        os << "Playing " << spots << " spots on card with " << numBins[0] << " groups of " << binSizes[0]
//           << ", progressive is $" << _progressive;
//        _gameDescription = os.str();
//        _costPerCard = nChoosek((int)numBins[0], 2) * _costs[spots];
//        unsigned long long waysHit=0;
//        unsigned long long waysMiss=0;
//        double totalPayHit = 0;
//        double totalPayMiss = 0;
//        for (auto card : _possibleBinHits) {
//            double payout=0;
//            auto bh = card.getBinDistributions()[0];
//            for (int first = 0;first<bh.size();++first)
//                for(int second = first; second<bh.size();++second) {
//                    if (first == second)
//                        payout += nChoosek(bh[first],2)*_payouts[spots][first+second];
//                    else
//                        payout += bh[first]*bh[second]*_payouts[spots][first+second];
//                }
//            if(bh.back()>=2) {
//                payout += progressive-_payouts[spots][spots];
//                waysHit += card.numWays();
//                totalPayHit += payout * card.numWays();
//            }
//            else {
//                waysMiss += card.numWays();
//                totalPayMiss += payout * card.numWays();
//            }
//            _payoutForCard.push_back(payout);
//        }
//        assert(waysHit+waysMiss == 3535316142212174320); //80c20
//        _EVGivenHitProgressive = (double)totalPayHit/waysHit;
//        _EVGivenMissProgressive = (double) totalPayMiss/waysMiss;
//        _EV = (double) (totalPayMiss+totalPayHit)/(waysHit+waysMiss);
//        _pProgressive = (double) waysHit / (waysHit+waysMiss);
//    }
//    else { //Two types of bins, fill one of each type for progressive.
//        assert(binSizes.size()==2 || binSizes.size()==3 && binSizes.back()==1);
//        os << "Playing " << spots << " spots on card with " << numBins[0] << " groups of " << binSizes[0]
//        << " and " << numBins[1] << " groups of " << binSizes[1] << ", progressive is $" << _progressive;
//        _gameDescription = os.str();
//        _costPerCard = numBins[0] * numBins[1] * _costs[spots];
//        unsigned long long waysHit=0;
//        unsigned long long waysMiss=0;
//        double totalPayHit = 0;
//        double totalPayMiss = 0;
//        for (auto card : _possibleBinHits) {
//            double payout=0;
//            auto bh1 = card.getBinDistributions()[0];
//            auto bh2 = card.getBinDistributions()[1];
//            for (int first = 0;first<bh1.size();++first)
//                for(int second = 0; second<bh2.size();++second)
//                       payout += bh1[first]*bh2[second]*_payouts[spots][first+second];
//            if(bh1.back()>=1 && bh2.back()>=1) {
//                payout += progressive-_payouts[spots][spots];
//                waysHit += card.numWays();
//                totalPayHit += payout * card.numWays();
//            }
//            else {
//                waysMiss += card.numWays();
//                totalPayMiss += payout * card.numWays();
//            }
//            _payoutForCard.push_back(payout);
//        }
//        assert(waysHit+waysMiss == 3535316142212174320); //80c20
//        _EVGivenHitProgressive = (double)totalPayHit/waysHit;
//        _EVGivenMissProgressive = (double) totalPayMiss/waysMiss;
//        _EV = (double) (totalPayMiss+totalPayHit)/(waysHit+waysMiss);
//        _pProgressive = (double) waysHit / (waysHit+waysMiss);    }
}

void PayoutResults::showStats() const
{
    cout.precision(10);
    printf("%s\n",_gameDescription.c_str());
    printf("EV: $%.2f\n",_EV);
    printf("EV given hit progressive: $%.2f = P+$%.2f\n",_EVGivenHitProgressive,_EVGivenHitProgressive-_progressive);
    cout << "EV given miss progressive: $" <<  _EVGivenMissProgressive << endl;
    cout << "Probability to hit progressive: " <<  _pProgressive << endl;
    cout << "Cost per card: $" <<  _costPerCard << endl;
}