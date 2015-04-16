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
#include <string>
#include <memory>

#include "BinHits.h"

class PayoutResults {
public:
    static std::unique_ptr<PayoutResults> PayoutResultsFactory(int spots, double progressive, const std::vector<BinHits> &);

    void showStats() const;
    virtual ~PayoutResults() = default;
    const std::vector<unsigned long> & getBinSizes() const;
    const std::vector<unsigned long> & getNumBins() const;

protected:
    const int _spots;
    const double _progressive;

    PayoutResults(int spots, double progressive, const std::vector<BinHits> &);
    const static std::vector<std::vector<double>> _payouts;
    const static std::vector<double> _costs;
    
private:
    virtual void checkBinSizes() const=0;
    virtual std::string gameDescription() const=0;
    virtual double calculatePayout(const BinHits &card) const =0;
    virtual bool hitJackpot(const BinHits &card) const =0;
    virtual unsigned long playsPerCard() const =0;
    
    void calculateStats();
    const std::vector<BinHits> &_possibleBinHits;
    double _pProgressive;
    double _EVGivenHitProgressive;
    double _EVGivenMissProgressive;
    double _EV;
    double _costPerCard;
    std::vector<unsigned long> _binSizes;
    std::vector<unsigned long> _numBins;
    std::string _gameDescription;
    std::vector<double> _payoutForCard;
    std::vector<bool> _hitProgressive;
    //double _sd;
    //double _var; // !!!Add these stats
    
};
//!!! Add standard deviation and variance to this class

class OneBinPayoutResults : public PayoutResults
{
public:
    OneBinPayoutResults(int spots, double progressive, const std::vector<BinHits> &bh):PayoutResults(spots,progressive,bh){}
private:
    virtual void checkBinSizes() const override;
    virtual std::string gameDescription() const override;
    virtual double calculatePayout(const BinHits &card) const override;
    virtual bool hitJackpot(const BinHits &card) const override;
    virtual unsigned long playsPerCard() const override;
};

class OneBinDoubledPayoutResults : public PayoutResults
{
    using PayoutResults::PayoutResults;
    virtual void checkBinSizes() const override;
    virtual std::string gameDescription() const override;
    virtual double calculatePayout(const BinHits &card) const override;
    virtual bool hitJackpot(const BinHits &card) const override;
    virtual unsigned long playsPerCard() const override;
public:
    OneBinDoubledPayoutResults(int spots, double progressive, const std::vector<BinHits> &bh):PayoutResults(spots,progressive,bh){}
};

class TwoBinsPayoutResults : public PayoutResults
{
    using PayoutResults::PayoutResults;
    virtual void checkBinSizes() const override;
    virtual std::string gameDescription() const override;
    virtual double calculatePayout(const BinHits &card) const override;
    virtual bool hitJackpot(const BinHits &card) const override;
    virtual unsigned long playsPerCard() const override;
public:
    TwoBinsPayoutResults(int spots, double progressive, const std::vector<BinHits> &bh):PayoutResults(spots,progressive,bh){}
};

#endif /* defined(__Keno2__CardResults__) */
