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

BigInt nChoosek( unsigned n, unsigned k )
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

BigInt ipower(BigInt a, BigInt n) {
    BigInt res = 1;
    while (n) {
        if (n & 1)
            res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}

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
                waysToChooseBins *= nChoosek(numberOfBins-k4,k3)       * ipower(4,k3);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3,k2)    * ipower(6,k2);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3-k2,k1) * ipower(4,k1);
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

void displayBinDistributions20x4() { //suitable for pasting into Excel
    const int ballsChosen = 20;
    for(int k4 = ballsChosen/4; k4 >= 0; --k4)
        for(int k3 = (ballsChosen-4*k4)/3; k3 >= 0; --k3)
            for(int k2 = (ballsChosen-4*k4-3*k3)/2; k2 >= 0; --k2){
                int k1 = ballsChosen-4*k4-3*k3-2*k2;
                
                cout << k4 << " " << k3 << " " << k2 << " " << k1 << endl;
            }
    
}

BigInt ways10p13(int k44,int k43, int k42, int k41, int k33, int k32, int k31)
{
    BigInt ways = 1;
    ways *= nChoosek(10, k44);
    ways *= nChoosek(10-k44        , k43)    * ipower(4,k43);
    ways *= nChoosek(10-k44-k43    , k42)    * ipower(6,k42);
    ways *= nChoosek(10-k44-k43-k42, k41)    * ipower(4,k41);
    ways *= nChoosek(13        , k33);
    ways *= nChoosek(13-k33    , k32) * ipower(3,k32);
    ways *= nChoosek(13-k33-k32, k31) * ipower(3,k31);
    return ways;
}

BigInt ways11p12(int k44,int k43, int k42, int k41, int k33, int k32, int k31)
{
    BigInt ways = 1;
    ways *= nChoosek(11, k44);
    ways *= nChoosek(11-k44        , k43)    * ipower(4,k43);
    ways *= nChoosek(11-k44-k43    , k42)    * ipower(6,k42);
    ways *= nChoosek(11-k44-k43-k42, k41)    * ipower(4,k41);
    ways *= nChoosek(12        , k33);
    ways *= nChoosek(12-k33    , k32) * ipower(3,k32);
    ways *= nChoosek(12-k33-k32, k31) * ipower(3,k31);
    return ways;
}

void displayBinDistributuions11x7()
{
//    const int ballsChosen = 20;
//    for(int k77 = ballsChosen/7; k77 >= 0; --k77)
//        for(int k76 = (ballsChosen-7*k77)/6; k76 >= 0; --k76)
//            for(int k75 = (ballsChosen-7*k77-6*k76)/5; k75 >= 0; --k75) {
//                
//                
//            }
// some sort of on the fly for loops? for 1..7 calculate and run
}

void displayBinDistributions10x4p13x3() { //suitable for pasting into Excel
    BigInt progressives = 0;
    BigInt total = 0;
    int ballsChosen = 20;
    for(int k44 = ballsChosen/4; k44 >= 0; --k44)
    for(int k43 = (ballsChosen-4*k44)/3; k43 >= 0; --k43)
    for(int k42 = (ballsChosen-4*k44-3*k43)/2; k42 >= 0; --k42)
    for(int k41 = min(ballsChosen-4*k44-3*k43-2*k42,10-k44-k43-k42); k41 >= 0; --k41) {
        const int ballsLeft = ballsChosen-4*k44-3*k43-2*k42-k41;
        for(int k33 = ballsLeft/3; k33 >= 0; --k33)
        for(int k32 = (ballsLeft-3*k33)/2; k32 >= 0; --k32) {
        int k31 = ballsLeft-3*k33-2*k32;
            if (k31 > 13 - k33-k32)
                continue;
            cout << k44 << " " << k43 << " " << k42 << " " << k41 << " " << 10-k44-k43-k42-k41 << " " << k33 << " " << k32 << " " << k31 << " " << 13-k33-k32-k31 << " " << 0 << endl;
            total += ways10p13(k44,k43,k42,k41,k33,k32,k31);
            if (k44>0 && k33>0)
                progressives += ways10p13(k44,k43,k42,k41,k33,k32,k31);
        }
    }
    ballsChosen = 19;
    for(int k44 = ballsChosen/4; k44 >= 0; --k44)
        for(int k43 = (ballsChosen-4*k44)/3; k43 >= 0; --k43)
            for(int k42 = (ballsChosen-4*k44-3*k43)/2; k42 >= 0; --k42)
                for(int k41 = min(ballsChosen-4*k44-3*k43-2*k42,10-k44-k43-k42); k41 >= 0; --k41) {
                    const int ballsLeft = ballsChosen-4*k44-3*k43-2*k42-k41;
                    for(int k33 = ballsLeft/3; k33 >= 0; --k33)
                        for(int k32 = (ballsLeft-3*k33)/2; k32 >= 0; --k32) {
                            int k31 = ballsLeft-3*k33-2*k32;
                            if (k31 > 13 - k33-k32)
                                continue;
                            cout << k44 << " " << k43 << " " << k42 << " " << k41 << " " << 10-k44-k43-k42-k41 << " " << k33 << " " << k32 << " " << k31 << " " << 13-k33-k32-k31 << " " << 1 << endl;
                            total += ways10p13(k44,k43,k42,k41,k33,k32,k31);
                            if (k44>0 && k33>0)
                                progressives += ways10p13(k44,k43,k42,k41,k33,k32,k31);
                        }
                }
    cout << total << endl;
    cout << progressives << " " << double(progressives)/(double)total << endl;
}

void displayBinDistributions11x4p12x3() { //suitable for pasting into Excel
    BigInt progressives = 0;
    BigInt total = 0;
    int ballsChosen = 20;
    for(int k44 = ballsChosen/4; k44 >= 0; --k44)
        for(int k43 = (ballsChosen-4*k44)/3; k43 >= 0; --k43)
            for(int k42 = (ballsChosen-4*k44-3*k43)/2; k42 >= 0; --k42)
                for(int k41 = min(ballsChosen-4*k44-3*k43-2*k42,11-k44-k43-k42); k41 >= 0; --k41) {
                    const int ballsLeft = ballsChosen-4*k44-3*k43-2*k42-k41;
                    for(int k33 = ballsLeft/3; k33 >= 0; --k33)
                        for(int k32 = (ballsLeft-3*k33)/2; k32 >= 0; --k32) {
                            int k31 = ballsLeft-3*k33-2*k32;
                            if (k31 > 12 - k33-k32)
                                continue;
//                            cout << k44 << " " << k43 << " " << k42 << " " << k41 << " " << 11-k44-k43-k42-k41 << " " << k33 << " " << k32 << " " << k31 << " " << 12-k33-k32-k31 << " " << 0 << endl;
                            cout << ways11p12(k44,k43,k42,k41,k33,k32,k31) << endl;
                            total += ways11p12(k44,k43,k42,k41,k33,k32,k31);
                            if (k44>0 && k33>0)
                                progressives += ways11p12(k44,k43,k42,k41,k33,k32,k31);
                        }
                }
    cout << total << endl;
    cout << progressives << " " << double(progressives)/(double)total << endl;
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



void simulate7spots(const vector<binDistribution> & bins)
{
//    const long initialBankroll = 1'200'000;
//    std::random_device rd;
//    std::default_random_engine e1(rd());
//    
//    vector<BigInt> probs;
//    for(auto &bd:bins)
//        probs.push_back(bd.waysThisCanHappen);
//    std::discrete_distribution<int> dist(probs.begin(),probs.end());
//    
//    BigInt trial = 0;
//    BigInt busts = 0;
//    
//    double cumulativeResult = 0;
//    long bankroll;
//    while(1) {
//        BigInt addedToMeter = 0;
//        bankroll = initialBankroll;
//        ++trial;
//        
//        bool hit =false;
//        while(bankroll >= 285 && !hit) {
//            int cards = min(56,int(bankroll/285));
//            addedToMeter += 95*cards;
//            for (int ii=0;ii<cards;++ii) {
//                auto b = bins[dist(e1)];
//                bankroll -= 285;
//                bankroll += b.payout;
//                if (b.binsWith4SpotsFilled > 1)
//                    {
//                    hit = true;
//                    bankroll += addedToMeter;
//                    }
//                }
//        }
//        if (bankroll < 285)
//            ++busts;
//        cumulativeResult += (bankroll - initialBankroll);
//        if (trial % 10000 == 0)
//            cout << "Trials: " << trial << " Bust probability: " << double(busts)/trial << ", average result: " << cumulativeResult/trial << endl;
//    }
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

vector<string> bins(int numBalls, int numBins, int binSize)
{
    vector<string> result;
    if (binSize == 0)
        {
        if(numBalls==0)
            return vector<string>{to_string(numBins)+" "};
        else
            return vector<string>();
        }
    
    int maxFullBins = min(numBalls/binSize,numBins);
    for(int fullBins = maxFullBins; fullBins>=0; --fullBins) {
        string theseBins;
//        for (int bin = 0; bin<fullBins; ++bin )
//            theseBins += to_string(binSize) + " ";
        theseBins += to_string(fullBins)+" ";
        int binsLeft = numBins-fullBins;
        int ballsLeft = numBalls - binSize*fullBins;
        
        auto theRest = bins(ballsLeft, binsLeft, binSize-1);
        for (auto binDist : theRest) {
            result.emplace_back(theseBins+binDist);
        }
    }
    return result;
}

vector<string> binsTwoSizes(int numBalls, int numType1Bins, int binType1Size, int numType2Bins, int binType2Size)
{
    vector<string> result;
    
    for(int ballsInType1Bins = numBalls; ballsInType1Bins >= 0; --ballsInType1Bins) {
        int ballsInType2Bins = numBalls-ballsInType1Bins;
        auto type1Distributions = bins(ballsInType1Bins,numType1Bins,binType1Size);
        auto type2Distributions = bins(ballsInType2Bins,numType2Bins,binType2Size);
        for( auto t1Dist : type1Distributions)
            for (auto t2Dist : type2Distributions)
                result.emplace_back(t1Dist+t2Dist);
    }

    return result;
}

vector<string> binsDriver(int numBins, int binSize)
{
    const int numBalls=20;
    vector<string> result;
    int spotsUnused = 80 - numBins*binSize;
    for(int ballsInUnusedSpots = 0; ballsInUnusedSpots <= spotsUnused; ++ballsInUnusedSpots){
        auto some = bins(numBalls-ballsInUnusedSpots,numBins,binSize);
        for( auto dist : some){
            dist += to_string(ballsInUnusedSpots);
            result.emplace_back(move(dist));
        }
    }
    
    return result;
}

vector<string> binsTwoSizesDriver(int numType1Bins, int binType1Size, int numType2Bins, int binType2Size)
{
    const int numBalls=20;
    vector<string> result;
    int spotsUnused = 80 - numType1Bins*binType1Size - numType2Bins*binType2Size;
    for(int ballsInUnusedSpots = 0; ballsInUnusedSpots <= spotsUnused; ++ballsInUnusedSpots){
        auto some = binsTwoSizes(numBalls-ballsInUnusedSpots,numType1Bins,binType1Size,numType2Bins,binType2Size);
        result.insert(end(result),begin(some),end(some));
    }
    
    return result;
}

int main() {
//    auto bins = calcBinDistributions();
//    displayStats(bins);
//    simulateSingleGame(bins);
//    simulate(bins);
//    displayBinDistributions();
//    displayBinDistributions10x4p13x3();
//    displayBinDistributions11x4p12x3();
//    calcProgressivesIE11x4p12x3();
//    cout << nChoosek(80,20) << " should be\n3535316142212174320" << endl;
//    auto partitions = binsDriver(20,20,4);
//    auto partitions = binsDriver(20,11,7);
//    auto partitions = binsTwoSizesDriver(11,4,12,3);
    auto partitions = binsTwoSizesDriver(10,4,13,3);
    int count = 0;
    for( auto p:partitions)
        cout << setw(4) << ++count << ":    " << p << endl;
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

