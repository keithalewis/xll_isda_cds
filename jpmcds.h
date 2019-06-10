// jpmcds.h - C++ wrappers for JP Morgan ISDA CDS model
#pragma once
#include <limits>
#include <utility>
#include <vector>
#include "cds.h"
#include "cx.h"
#include "feeleg.h"
#include "ldate.h"
#include "tcurve.h"

namespace Jpmcds {

    enum FREQUENCY {
        FREQ_ANNUAL = 1,
        FREQ_SEMIANNUAL = 2,
        FREQ_QUARTERLY = 4,
        FREQ_MONTHLY = 12,
    };
    
    enum RATE_BASIS {
        CONTINUOUS_BASIS = JPMCDS_CONTINUOUS_BASIS,   /* Means continuous compounding */
        DISCOUNT_RATE    = JPMCDS_DISCOUNT_RATE,      /* Discount RATE (not factor) K's b-Day*/
        SIMPLE_BASIS     = JPMCDS_SIMPLE_BASIS,       /* Means simple interest */
        ANNUAL_BASIS     = JPMCDS_ANNUAL_BASIS,       /* Annual compounding */
        DISCOUNT_FACTOR  = JPMCDS_DISCOUNT_FACTOR,    /* Rate is stored as a discount factor */
    };

    enum DAY_COUNT_CONVENTION {
        BAD_DCC  = JPMCDS_BAD_DCC,  /* Dummmy Day Count Conv for Brazilian bods*/
        ACT_365  = JPMCDS_ACT_365,  /* Actual/365 */
        ACT_365F = JPMCDS_ACT_365F, /* Actual/365 Fixed */
        ACT_360  = JPMCDS_ACT_360,  /* Actual/360 */
        B30_360  = JPMCDS_B30_360,  /* 30/360 */
        B30E_360 = JPMCDS_B30E_360, /* 30E/360 */
        ACT_ACT  = JPMCDS_ACT_ACT,
        EFFECTIVE_RATE = JPMCDS_EFFECTIVE_RATE, /* Effective rate - YF always 1 */
    };

    enum ROLL_CONVENTION {
        ROLL_FOLLOW   = JPMCDS_BAD_DAY_FOLLOW,
        ROLL_PREVIOUS = JPMCDS_BAD_DAY_PREVIOUS,
        ROLL_NONE     = JPMCDS_BAD_DAY_NONE,
        ROLL_MODIFIED = JPMCDS_BAD_DAY_MODIFIED,
    };

    /* Interpolation methods which can be applied to a zero curve only. */
    enum INTERPOLATION_TYPE {
        LINEAR_FORWARDS = JPMCDS_LINEAR_FORWARDS, /* Linear forward interpolation  */
        FLAT_FORWARDS = JPMCDS_FLAT_FORWARDS,     /* Flat forward interpolation  */
    };

    namespace Instrument {

        // Short term cash deposit.
        struct MoneyMarket {
            TDateInterval tenor; // e.g., TDateInterval{3, 'M'}
            DAY_COUNT_CONVENTION dcc;
            ROLL_CONVENTION roll;
            int days; // T + days settlement
            double rate;
        private:
            TDate date_[2]; // settlement, maturity
            double cash_[2]; // cash flows
        public:
            MoneyMarket(TDateInterval tenor, DAY_COUNT_CONVENTION dcc = ACT_360, ROLL_CONVENTION roll = ROLL_FOLLOW, int days = 3)
                : tenor(tenor), dcc(dcc), roll(roll), days(days)
            { }
            MoneyMarket(const MoneyMarket&) = default;
            MoneyMarket& operator=(const MoneyMarket&) = default;
            ~MoneyMarket()
            { }

            // Set rate and cash flow dates.
            MoneyMarket& set(double rate_, TDate valuation_)
            {
                static char* holiday = const_cast<char*>("NONE"); // only weekends
                rate = rate_;

                TDateAdjIntvl dai;
                dai.badDayConv = roll;
                dai.holidayFile = holiday;
                dai.interval = TDateInterval{days, 'D'};
                dai.isBusDays = TRUE;
               
                // first date
                JpmcdsDtFwdAdj(valuation_, &dai, date_ + 0);

                // second date
                dai.interval = tenor;
                JpmcdsDtFwdAdj(date_[0], &dai, date_ + 1);

                cash_[0] = -1;
                double dcf;
                JpmcdsDayCountFraction(date_[0],date_[1], dcc, &dcf);
                cash_[1] = 1 + rate*dcf;

                return *this;
            }
            TDate maturity() const
            {
                return date_[1];
            }
            int size() const
            {
                return 2;
            }
            const TDate* date() const
            {
                return date_;
            }
            const double* cash() const
            {
                return cash_;
            }
        };

        struct InterestRateFixedLeg {
            TDateInterval tenor;
            FREQUENCY freq;
            DAY_COUNT_CONVENTION dcc;
            ROLL_CONVENTION roll;
            int days; // T + days settlement
            double rate; // par coupon
            InterestRateFixedLeg(TDateInterval tenor, FREQUENCY freq = FREQ_SEMIANNUAL, DAY_COUNT_CONVENTION dcc = ACT_360, ROLL_CONVENTION roll = ROLL_MODIFIED, int days = 2)
                : tenor(tenor), freq(freq), dcc(dcc), roll(roll), days(days)
            { }
            InterestRateFixedLeg(const InterestRateFixedLeg&) = default;
            InterestRateFixedLeg& operator=(const InterestRateFixedLeg&) = default;
            ~InterestRateFixedLeg()
            { }
            // Set par coupon and cash flow dates.
            InterestRateFixedLeg& set(TDate valuation, double rate_)
            {
                rate = rate_;
                valuation = valuation;
                // !!! add dates and cash flows here !!!
                
                return *this;
            }
            TDate maturity(TDate valuation) const
            {
                TDate set, mat;
                TDateAdjIntvl ai;

                ai.badDayConv = roll;
                ai.holidayFile = const_cast<char*>("NONE");
                ai.interval = TDateInterval{days, 'D'};
                ai.isBusDays = TRUE;
                JpmcdsDtFwdAdj(valuation, &ai, &set);
                ai.interval = tenor;
                JpmcdsDtFwdAdj(set, &ai, &mat);

                return mat;
            }
        };

        struct InterestRateFloatLeg {
            TDateInterval tenor;
            FREQUENCY freq;
            DAY_COUNT_CONVENTION dcc;
            ROLL_CONVENTION roll;
            int days; // T + days settlement
            InterestRateFloatLeg(TDateInterval tenor, FREQUENCY freq = FREQ_QUARTERLY, DAY_COUNT_CONVENTION dcc = ACT_360, ROLL_CONVENTION roll = ROLL_MODIFIED, int days = 2)
                : tenor(tenor), freq(freq), dcc(dcc), roll(roll), days(days)
            { }
        };

        struct InterestRateSwap {
            InterestRateFixedLeg fixedLeg;
            InterestRateFloatLeg floatLeg;
            InterestRateSwap(TDateInterval tenor, FREQUENCY freq = FREQ_QUARTERLY, DAY_COUNT_CONVENTION dcc = ACT_360, ROLL_CONVENTION roll = ROLL_MODIFIED, int days = 2)
                : fixedLeg(tenor, freq, dcc, roll, days), floatLeg(tenor, freq, dcc, roll, days)
            { }
            InterestRateSwap& set(double rate_, TDate valuation)
            {
                fixedLeg.rate = rate_;
                valuation = valuation;
                //???JpmcdsDateListMakeRegular

                return *this;
            }
            TDate maturity(TDate valuation) const
            {
                return fixedLeg.maturity(valuation);
            }
        };
//#include "cx.h"
        struct ContingentLeg : public TContingentLeg {
            ContingentLeg(
                TDate           startDate,
                TDate           endDate,
                double          notional = 1,
                TProtPayConv    payType = PROT_PAY_DEF,
                TBoolean        protectStart = false)
                : TContingentLeg{startDate - protectStart, endDate, notional, payType, protectStart}
            { 
            }
        };
        struct FeeLeg {
            TFeeLeg* feeLeg;
            FeeLeg(
                TDate           startDate,
                TDate           endDate,
                TBoolean        payAccOnDefault,
                TDateInterval  *dateInterval,
                TStubMethod    *stubType,
                double          notional,
                double          couponRate,
                long            paymentDcc,
                long            badDayConv,
                char           *calendar,
                TBoolean        protectStart)
                : feeLeg(JpmcdsCdsFeeLegMake(startDate, endDate, payAccOnDefault, dateInterval, 
                    stubType, notional, couponRate, paymentDcc, badDayConv, calendar, protectStart))
            {
            }
            ~FeeLeg()
            {
                JpmcdsFeeLegFree(feeLeg);
            }
            /*
            double PresentValue()
            {
                double pv;

                int rc = JpmcdsFeeLegPV(feeLeg, today, stepinDate, valueDate, discount, spread, payAccrued, &pv);

                return pv;
            }
            
            double AccruedInterest(TDate today)
            {
                double ai;

                if (SUCCESS != FeeLegAI(feeLeg, today, &ai)) // FeeLegAI not in header file
                    throw std::runtime_error("FeeLetIA failed");

                return ai;
            }
            */
        };
        struct CreditDefaultSwap {
            TContingentLeg contingentLeg; // protection leg
            TFeeLeg feeLeg;
        };

    } // namespace Instrument

    // Make ISDA TCurve into a proper value type.
    class Curve {
        ::TCurve* p;
    public:
        Curve()
            : p(nullptr)
        { }
        Curve(::TDate baseDate, int numPts, RATE_BASIS basis, DAY_COUNT_CONVENTION dayCountConv)
            : p(JpmcdsNewTCurve(baseDate, numPts, basis, dayCountConv))
        { }
        Curve(TDate baseDate, TDate *dates, double *rates, int numPts, RATE_BASIS basis, DAY_COUNT_CONVENTION dayCountConv)
            : p(JpmcdsMakeTCurve(baseDate, dates, rates, numPts, basis, dayCountConv))
        { }
        Curve(::TCurve* p)
            : p(::JpmcdsCopyCurve(p))
        { }
        /*
        Curve& operator=(::TCurve* p_)
        {
            p = p_; // too dangerous
        }
        */
        Curve(const Curve& curve)
            : Curve(curve.p)
        { }
        Curve& operator=(const Curve& curve)
        {
            return *this = Curve(curve);
        }
        Curve(Curve&& curve) noexcept
            : p(std::exchange(curve.p, nullptr))
        { }
        Curve& operator=(Curve&& curve) noexcept
        {
            std::swap(p, curve.p);

            return *this;
        }
        ~Curve()
        {
            if (p)
                JpmcdsFreeTCurve(p);
        }
        operator ::TCurve*()
        {
            return p;
        }
        int size() const
        {
            return p->fNumItems;
        }
        const TRatePt* points() const
        {
            return p->fArray;
        }
        // Calculate discount factor for specified date and zero curve.
        double DiscountDate(TDate date, long interpType = JPMCDS_FLAT_FORWARDS)
        {
            double D = std::numeric_limits<double>::quiet_NaN();

            if (p) {
                int rc = JpmcdsDiscountDate(date, p, interpType, &D);
                if (rc != SUCCESS)
                    D = std::numeric_limits<double>::quiet_NaN();
            }
            
            return D;
        }
        // Interpolates a TCurve at a TDate using the specified interp method. 
        double InterpRate(TDate date, long interpType = JPMCDS_FLAT_FORWARDS)
        {
            double r = std::numeric_limits<double>::quiet_NaN();

            if (p) {
                int rc = JpmcdsInterpRate(date, p, interpType, &r);
                if (rc != SUCCESS)
                    r = std::numeric_limits<double>::quiet_NaN();
            }

            return r;
        }
    };

} // namespace Jpmcds
