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

constexpr unsigned long long nChoosek( unsigned n, unsigned k )
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;
    
    unsigned long long result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

constexpr unsigned long long ipower(unsigned long long a, unsigned long long n) {
    unsigned long long res = 1;
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
    unsigned long long waysThisCanHappen;
    unsigned long long payout;
    
};

void displayStats(const vector<binDistribution> &bds)
{
    unsigned long long totalDraws = 0;
    unsigned long long hits = 0;
    unsigned long long w2s = 0;
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
        
        totalResult += (double)bd.payout*bd.waysThisCanHappen;
    }
    cout << endl;
    cout << "Hit progressive " << hits << " times. " << endl;
    cout << "Progressive prob = " << double(hits)/totalDraws << endl;
    cout << "W2g prob = " << double(w2s)/totalDraws << endl;
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
                
                unsigned long long waysToChooseBins = 1;
                waysToChooseBins *= nChoosek(numberOfBins,k4);
                waysToChooseBins *= nChoosek(numberOfBins-k4,k3)       * ipower(4,k3);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3,k2)    * ipower(6,k2);
                waysToChooseBins *= nChoosek(numberOfBins-k4-k3-k2,k1) * ipower(4,k1);
                bd.waysThisCanHappen = waysToChooseBins;
                
                long long thisTimeResult = 0;
                unsigned long long thisTimeHits = nChoosek(k4,2);
                if (thisTimeHits>=1)
                    thisTimeResult += 262000 + (thisTimeHits-1)*48000;
                unsigned long long  sevens = k4*k3;
                thisTimeResult += 1480*sevens;
                unsigned long long sixes = nChoosek(k3, 2) + k4*k2;
                thisTimeResult += 80*sixes;
                unsigned long long  fives = k4*k1 + k2*k3;
                thisTimeResult += 4*fives;
                
                bd.payout = thisTimeResult;
                
                binDistributions.push_back(bd);
            }
    
    return binDistributions;
}

void displayBinDistributions() { //suitable for pasting into Excel
    const int ballsChosen = 20;
    for(int k4 = ballsChosen/4; k4 >= 0; --k4)
        for(int k3 = (ballsChosen-4*k4)/3; k3 >= 0; --k3)
            for(int k2 = (ballsChosen-4*k4-3*k3)/2; k2 >= 0; --k2){
                int k1 = 20-4*k4-3*k3-2*k2;
                
                cout << k4 << " " << k3 << " " << k2 << " " << k1 << endl;
            }
    
}

void simulateSingleGame(const vector<binDistribution> & bins)
{
    std::random_device rd;
    std::default_random_engine e1(rd());
    
    vector<unsigned long long> probs;
    for(auto &bd:bins)
        probs.push_back(bd.waysThisCanHappen);
    std::discrete_distribution<int> dist(probs.begin(),probs.end());
    
    unsigned long long trial = 0;
    unsigned long long hits = 0;
    double cumulativeResult = 0;
    while(1) {
        ++trial;
        auto b = bins[dist(e1)];
        cumulativeResult += b.payout;
        if (b.binsWith4SpotsFilled > 1)
            ++hits;
        if (trial % 10000000 == 0)
            cout << "Trials: " << trial << " Average result: " << cumulativeResult/trial << ", hit probability: " << double(hits)/trial << endl;
        }
}

void simulate(const vector<binDistribution> & bins)
{
    const long initialBankroll = 1'200'000;
    std::random_device rd;
    std::default_random_engine e1(rd());
    
    vector<unsigned long long> probs;
    for(auto &bd:bins)
        probs.push_back(bd.waysThisCanHappen);
    std::discrete_distribution<int> dist(probs.begin(),probs.end());
    
    unsigned long long trial = 0;
    unsigned long long busts = 0;
    double cumulativeResult = 0;
    long bankroll;
    while(1) {
        bankroll = initialBankroll;
        ++trial;
        
        bool hit =false;
        while(bankroll >= 285 && !hit) {
            int cards = min(56,int(bankroll/285));
//            double resultOf56 = 0;
            for (int ii=0;ii<cards;++ii) {
                auto b = bins[dist(e1)];
                bankroll -= 285;
                bankroll += b.payout;
//                resultOf56 -= 285;
//                resultOf56 += b.payout;
                if (b.binsWith4SpotsFilled > 1)
                    hit = true;
                }
//            if ((resultOf56 > 100000) != hit)
//                cout << "resultOf56?" << resultOf56 << endl << 0,exit(1);
        }
        if (bankroll < 285)
            ++busts;
        cumulativeResult += (bankroll - initialBankroll);
        // cout << (bankroll - initialBankroll) << endl;
        if (trial % 10000 == 0)
            cout << "Trials: " << trial << " Bust probability: " << double(busts)/trial << ", average result: " << cumulativeResult/trial << endl;
    }
}

int main() {
    auto bins = calcBinDistributions();
    displayStats(bins);
    simulateSingleGame(bins);
//    simulate(bins);
    //    displayBinDistributions();
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
 
 Trials: 6520000000 Average result: 310.908, hit probability: 0.000821031
 Trials: 6530000000 Average result: 310.907, hit probability: 0.000821029
 Trials: 6540000000 Average result: 310.913, hit probability: 0.00082105
 */

