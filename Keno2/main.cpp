#include <iostream>
using std::cin;
using std::endl;
using std::cout;
#include <iomanip>
using std::setw;
using Int = unsigned long;
#include <vector>
using std::vector;
#include <cmath>
#include <sstream>
using std::ostringstream;
#include <random>
#include <algorithm>
using std::min;
#include <string>
using std::string;
using std::to_string;
#include <iterator>
using std::back_inserter;
#include <cassert>

#include "util.h"
#include "BinHits.h"
#include "CardResults.h"

struct binDistribution {
    int binsWith4SpotsFilled;
    int binsWith3SpotsFilled;
    int binsWith2SpotsFilled;
    int binsWith1SpotsFilled;
    BigInt waysThisCanHappen;
    BigInt payout;
    
};

void displayStats(const vector<binDistribution> &bds)
{
    BigInt totalDraws = 0;
    BigInt hits = 0;
    BigInt w2s = 0;
    double totalResult = 0;
    int count = 0;
    
    cout << setw(6)  << "Count";
    cout << setw(19) << "Ways";
    cout << setw(20) << "Cumulative ways";
    cout << setw(7)  << "Payout";
    cout << "   Bins" << endl;
    
    for(auto bd:bds) {
        ++count;
        totalDraws += bd.waysThisCanHappen;
        
        if(bd.binsWith4SpotsFilled>1)
            hits += bd.waysThisCanHappen;
        
        
        cout << setw(5)  << count << ":";
        cout << setw(19) << bd.waysThisCanHappen;
        cout << setw(20) << totalDraws;
        cout << setw(7)  << bd.payout;
        cout << "   [ ";
        for (int i=0;i<bd.binsWith4SpotsFilled;++i) cout << "4 ";
        for (int i=0;i<bd.binsWith3SpotsFilled;++i) cout << "3 ";
        for (int i=0;i<bd.binsWith2SpotsFilled;++i) cout << "2 ";
        for (int i=0;i<bd.binsWith1SpotsFilled;++i) cout << "1 ";
        cout << "]" << endl;
        
        if (bd.payout >= 1500+285)
            w2s += bd.waysThisCanHappen;
        
        totalResult += (double)bd.payout*(double)bd.waysThisCanHappen;
    }
    cout << endl;
    cout << "Hit progressive " << hits << " times. " << endl;
    cout << "Progressive prob = " << double(hits)/(double)totalDraws << endl;
    cout << "W2g prob = " << double(w2s)/(double)totalDraws << endl;
    cout << "average result = " << totalResult/double(totalDraws) << endl;
}

vector<binDistribution> calcBinDistributions()
{
    vector<binDistribution> binDistributions;
    
    const int ballsChosen = 20;
    const int numberOfBins = 20;
    
    for(int k4 = ballsChosen/4; k4 >= 0; --k4)
        for(int k3 = (ballsChosen-4*k4)/3; k3 >= 0; --k3)
            for(int k2 = (ballsChosen-4*k4-3*k3)/2; k2 >= 0; --k2) {
                int k1 = 20-4*k4-3*k3-2*k2;
                
                binDistribution bd;
                bd.binsWith1SpotsFilled = k1;
                bd.binsWith2SpotsFilled = k2;
                bd.binsWith3SpotsFilled = k3;
                bd.binsWith4SpotsFilled = k4;
                
                BigInt waysToChooseBins = 1;
                waysToChooseBins *= nChoosek(numberOfBins,k4);
                waysToChooseBins *= nChoosek(numberOfBins-k4,k3)       * iPower(4,k3);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3,k2)    * iPower(6,k2);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3-k2,k1) * iPower(4,k1);
                bd.waysThisCanHappen = waysToChooseBins;
                
                BigInt thisTimeResult = 0;
                BigInt thisTimeHits = nChoosek(k4,2);
                if (thisTimeHits>=1)
                    thisTimeResult += 262000 + (thisTimeHits-1)*48000;
                BigInt  sevens = k4*k3;
                thisTimeResult += 1480*sevens;
                BigInt sixes = nChoosek(k3, 2) + k4*k2;
                thisTimeResult += 80*sixes;
                BigInt  fives = k4*k1 + k3*k2;
                thisTimeResult += 4*fives;
                
                bd.payout = thisTimeResult;
                
                binDistributions.push_back(bd);
            }
    
    return binDistributions;
}

void simulateSingleGame(const vector<binDistribution> & bins)
{
    std::random_device rd;
    std::default_random_engine e1(rd());
    
    vector<BigInt> probs;
    for(auto &bd:bins)
        probs.push_back(bd.waysThisCanHappen);
    std::discrete_distribution<int> dist(begin(probs),end(probs));
    
    BigInt trial = 0;
    BigInt hits = 0;
    double cumulativeResult = 0;
    while(1) {
        ++trial;
        auto choice = dist(e1);
        auto b = bins[choice];
        cumulativeResult += (double)b.payout;
        if (b.binsWith4SpotsFilled > 1)
            ++hits;
        if (trial % 10'000'000 == 0)
            cout << "Trials: " << trial << " Average result: " << cumulativeResult/(double)trial << ", hit probability: " << double(hits)/(double)trial << endl;
        }
}

void simulate(const vector<binDistribution> & bins)
{
    const long initialBankroll = 1'200'000;
    std::random_device rd;
    std::default_random_engine e1(rd());
    
    vector<BigInt> probs;
    for(auto &bd:bins)
        probs.push_back(bd.waysThisCanHappen);
    std::discrete_distribution<int> dist(probs.begin(),probs.end());
    
    BigInt trial = 0;
    BigInt busts = 0;
    
    double cumulativeResult = 0;
    long bankroll;
    while(1) {
        BigInt addedToMeter = 0;
        bankroll = initialBankroll;
        ++trial;
        
        bool hit =false;
        while(bankroll >= 285 && !hit) {
            int cards = min(56,int(bankroll/285));
            addedToMeter += 95*cards;
            for (int ii=0;ii<cards;++ii) {
                auto b = bins[dist(e1)];
                bankroll -= 285;
                bankroll += (double)b.payout;
                if (b.binsWith4SpotsFilled > 1)
                {
                    hit = true;
                    bankroll += (double)addedToMeter;
                }
            }
        }
        if (bankroll < 285)
            ++busts;
        cumulativeResult += (bankroll - initialBankroll);
        if (trial % 10000 == 0)
            cout << "Trials: " << trial << " Bust probability: " << double(busts)/(double)trial << ", average result: " << cumulativeResult/(double)trial << endl;
    }
}

void calcProgressivesIE11x4p12x3()
{
BigInt total = 0;
for(int s4=1;s4<=20/4;++s4)
    for(int s3=1;s3<=(20-4*s4)/3;++s3) {
        total += nChoosek(11,s4)*nChoosek(12, s3)*nChoosek(80-4*s4-3*s3, 20-4*s4-3*s3)*((s4+s3)%2 == 0? 1:-1);
    }
cout << total << endl;
}

vector<vector<int>> bins(int numBalls, int numBins, int binSize)
{//all ways of distributing numBalls into numBins of size binSize
    if (binSize == 0)
        {
        if(numBalls==0)
            return {{numBins}};
        else
            return vector<vector<int>>();
        }
    
    vector<vector<int>> result;
    int maxFullBins = min(numBalls/binSize,numBins);
    for(int fullBins = maxFullBins; fullBins>=0; --fullBins) {
        int binsLeft = numBins-fullBins;
        int ballsLeft = numBalls - binSize*fullBins;
        
        auto theRest = bins(ballsLeft, binsLeft, binSize-1);
        for (auto binDist : theRest) {
            binDist.push_back(fullBins);
            result.emplace_back(binDist);
        }
    }
    return result;
}

vector<BinHits>  binsTwoSizes(int numType1Bins, int binType1Size, int numType2Bins, int binType2Size)
{
    vector<BinHits> result;
    
    int spotsUnused = 80 - numType1Bins*binType1Size - numType2Bins*binType2Size;
    for(int ballsInUnusedSpots = 0; ballsInUnusedSpots <= spotsUnused; ++ballsInUnusedSpots){
        int numBallsLeft = 20-ballsInUnusedSpots;
        for(int ballsInType1Bins = numBallsLeft; ballsInType1Bins >= 0; --ballsInType1Bins) {
            int ballsInType2Bins = numBallsLeft-ballsInType1Bins;
            auto type1Distributions = bins(ballsInType1Bins,numType1Bins,binType1Size);
            auto type2Distributions = bins(ballsInType2Bins,numType2Bins,binType2Size);
            for( auto t1Dist : type1Distributions)
                for (auto t2Dist : type2Distributions) {
                    vector<decltype(t1Dist)> binDists{t1Dist};
                    if (numType2Bins != 0)
                        binDists.push_back(t2Dist);
                    if (spotsUnused>0)
                        binDists.push_back(vector<int>{spotsUnused-ballsInUnusedSpots,ballsInUnusedSpots});
                        result.emplace_back(BinHits{binDists});
                }
        }
    }
    
    return result;
}

vector<BinHits> cards(int numBins, int binSize)
{
    int spotsUnused = 80 - numBins*binSize;
    return binsTwoSizes(numBins,binSize,spotsUnused,1);
}

vector<BinHits>  cards(int numType1Bins, int binType1Size, int numType2Bins, int binType2Size)
{
return binsTwoSizes(numType1Bins, binType1Size, numType2Bins, binType2Size);
}

void show(const vector<BinHits> &bins)
{
    int count = 0;
    
    cout << "       |";
    auto bDs = bins.front().getBinDistributions();
    for(auto bD:bDs) {
        int spaces = (int)bD.size()*5+2;
        cout << setw(spaces/2-1) << bD.size()-1 << "-bins" << setw(spaces-(spaces/2)-2) << "|";
    }
    cout << endl;

    cout << "Label  ";
    for(auto bD:bDs){
        cout << "|   ";
        for(int ii=0;ii<bD.size();++ii)
            cout << ii << "s" << "   ";
    }
    cout << "|                 #Ways" << endl;
    
    unsigned long long total=0;
    for(const auto &v:bins) {
        cout << setw(5) << ++count << "    ";
        for(auto bd : v.getBinDistributions()) {
            for(auto i : bd)
                cout << setw(4) << i << " ";
            cout << "    ";
            }
        cout << "   " << setw(18) << v.numWays();
        total += v.numWays();
        cout << endl;
    }
    cout << "Total number of ways: " << total << " ";
    if (total == 3535316142212174320)
        cout << "(Which is 80 choose 20.)" << endl;
    else
        cout << "(NOT 80 choose 20!!!! Alert!!!)" << endl;
}

int main() {
    assert((BinHits{{{15,0,0,0,5}}}.numWays()==15504));
    assert((BinHits{{{10,6,1,0,3}}}.numWays()==3814073303040ull));
    
//    auto bins = calcBinDistributions();
//    displayStats(bins);
//    simulateSingleGame(bins);
//    simulate(bins);
//    displayBinDistributions();
//    displayBinDistributions10x4p13x3();
//    displayBinDistributions11x4p12x3();
//    calcProgressivesIE11x4p12x3();
//    cout << nChoosek(80,20) << " should be\n3535316142212174320" << endl;
//    auto partitions = binsDriver(20,11,7);
//    auto partitions = binsTwoSizesDriver(11,4,12,3);
//    auto partitions = binsTwoSizesDriver(10,4,13,3);
//    show(cards(11,4,12,3));
//    show(cards(10,4,13,3));
//    show(cards(11,7));
    show(cards(20, 4));
    PayoutResults p(8,262000,cards(20,4));
    p.showStats();
    return 0;
}

/*
 4spot: $2, 4/4 pays jackpot $400+ (653)
 5spot: $2, 4/5 $2.50, 5/5 $1700+ (3050)
 6spot: $1.50, 4/6 $1.25, 5/6 $65, 6/6 $3500+ (9800)
 7spot: $1.50, 5/7 $10, 6/7 $325, 7/7 $15000+ (48200)
 8spot: $1.50, 5/8 $4, 6/8 $80, 7/8 $1480, 8/8 $48000+ (230k)
 9spot: $1.50, 5/9 $1, 6/9 $30, 7/9 $325, 8/9 $6000, 9/9 $90000+ (1.25M)

 Progressive odds:	0.000820911					0.0820911%
 w2g odds:          0.00891184					0.8911840%
 Expected payout:	308.3242193

 Trials: 10570000000 Average result: 310.381, hit probability: 0.000821154
 Trials: 10580000000 Average result: 310.385, hit probability: 0.000821171
 Trials: 10590000000 Average result: 310.382, hit probability: 0.000821157
 Trials: 10600000000 Average result: 310.378, hit probability: 0.000821143
 Trials: 10610000000 Average result: 310.377, hit probability: 0.000821140



3535316142212174320
3535316142212174320
  */

