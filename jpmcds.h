// jpmcds.h - C++ wrappers for JP Morgan ISDA CDS model
#pragma once
#include <limits>
#include <utility>
#include "ldate.h"
#include "tcurve.h"

namespace Jpmcds {

    /*------- Calculating discount factors----------*/
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

    /* Interpolation methods which can be applied to a zero curve only. */
    enum INTERPOLATION_TYPE {
        LINEAR_FORWARDS = JPMCDS_LINEAR_FORWARDS, /* Linear forward interpolation  */
        FLAT_FORWARDS = JPMCDS_FLAT_FORWARDS,     /* Flat forward interpolation  */
    };

    class TCurve_ {
        ::TCurve* p;
    public:
        TCurve_()
            : p(nullptr)
        { }
        TCurve_(::TDate baseDate, int numPts, RATE_BASIS basis, DAY_COUNT_CONVENTION dayCountConv)
            : p(JpmcdsNewTCurve(baseDate, numPts, basis, dayCountConv))
        { }
        TCurve_(TDate baseDate, TDate *dates, double *rates, int numPts, RATE_BASIS basis, DAY_COUNT_CONVENTION dayCountConv)
            : p(JpmcdsMakeTCurve(baseDate, dates, rates, numPts, basis, dayCountConv))
        { }
        TCurve_(::TCurve* p)
            : p(::JpmcdsCopyCurve(p))
        { }
        TCurve_(const TCurve_& curve)
            : TCurve_(curve.p)
        { }
        TCurve_& operator=(const TCurve_& curve)
        {
            return *this = TCurve_(curve);
        }
        TCurve_(TCurve_&& curve) noexcept
            : p(std::exchange(curve.p, nullptr))
        { }
        TCurve_& operator=(TCurve_&& curve) noexcept
        {
            std::swap(p, curve.p);

            return *this;
        }
        ~TCurve_()
        {
            if (p)
                JpmcdsFreeTCurve(p);
        }
        operator ::TCurve*()
        {
            return p;
        }
        // Calculate discount factor for specified date and zero curve.
        double DiscountDate(TDate date, long interpType = JPMCDS_FLAT_FORWARDS)
        {
            double D;

            int rc = JpmcdsDiscountDate(date, p, interpType, &D);
            if (rc != SUCCESS)
                D = std::numeric_limits<double>::quiet_NaN();

            return D;
        }
        // Interpolates a TCurve at a TDate using the specified interp method. 
        double InterpRate(TDate date, long interpType = JPMCDS_FLAT_FORWARDS)
        {
            double r;

            int rc = JpmcdsInterpRate(date, p, interpType, &r);
            if (rc != SUCCESS)
                r = std::numeric_limits<double>::quiet_NaN();

            return r;
        }
    };

} // namespace Jpmcds
