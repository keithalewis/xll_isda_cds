// xll_isda_cds.cpp
#include "jpmcds.h"
#include "xll_isda_cds.h"

using namespace Jpmcds;
using namespace xll;

AddIn xai_isda_cds(
    Documentation(L"The JP Morgan ISDA CDS model.")
);

XLL_ENUM_DOC(FREQUENCY::FREQ_ANNUAL, FREQ_ANNUAL, L"JPMCDS", L"Once per year.", L"Once per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_SEMIANNUAL, FREQ_SEMIANNUAL, L"JPMCDS", L"Twice per year.", L"Twice per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_QUARTERLY, FREQ_QUARTERLY, L"JPMCDS", L"Four times per year.", L"Four times per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_MONTHLY, FREQ_MONTHLY, L"JPMCDS", L"Twelve times per year.", L"Twelve times per year.");

XLL_ENUM_DOC(RATE_BASIS::CONTINUOUS_BASIS, RATE_CONTINUOUS_BASIS, L"JPMCDS", L"Means continuous compounding", L"Means continuous compounding");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_RATE, RATE_DISCOUNT_RATE, L"JPMCDS", L"Discount RATE (not factor) K's b-Day", L"Discount RATE (not factor) K's b-Day");
XLL_ENUM_DOC(RATE_BASIS::SIMPLE_BASIS, RATE_SIMPLE_BASIS, L"JPMCDS", L"Means simple interest", L"Means simple interest");
XLL_ENUM_DOC(RATE_BASIS::ANNUAL_BASIS, RATE_ANNUAL_BASIS, L"JPMCDS", L"Annual compounding", L"Annual compounding");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_FACTOR, RATE_DISCOUNT_FACTOR, L"JPMCDS", L"Rate is stored as a discount factor", L"Rate is stored as a discount factor");

XLL_ENUM_DOC(DAY_COUNT_CONVENTION::BAD_DCC, DAY_COUNT_BAD_DCC, L"JPMCDS", L"Dummmy Day Count Conv for Brazilian bonds", L"Dummmy Day Count Conv for Brazilian bonds");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365, DAY_COUNT_ACT_365, L"JPMCDS", L"Actual/365", L"Actual/365");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365F, DAY_COUNT_ACT_365F, L"JPMCDS", L"Actual/365 Fixed", L"Actual/365 Fixed");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_360, DAY_COUNT_ACT_360, L"JPMCDS", L"Actual/360", L"Actual/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30_360, DAY_COUNT_B30_360, L"JPMCDS", L"30/360", L"30/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30E_360, DAY_COUNT_B30E_360, L"JPMCDS", L"30E/360", L"30E/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_ACT, DAY_COUNT_ACT_ACT, L"JPMCDS", L"Same as Actual/365", L"Same as Actual/365");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::EFFECTIVE_RATE, DAY_COUNT_EFFECTIVE_RATE, L"JPMCDS", L"Effective rate - YF always 1", L"Effective rate - YF always 1");

XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_FOLLOW, ROLL_FOLLOW, L"JPMCDS", L"Following business day.", L"Following business day.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_PREVIOUS, ROLL_PREVIOUS, L"JPMCDS", L"Previous business day.", L"Previous business day.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_NONE, ROLL_NONE, L"JPMCDS", L"No adustment.", L"No adustment.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_MODIFIED, ROLL_MODIFIED, L"JPMCDS", L"Following business day unless at end of month.", L"Following business day unless at end of month.");

XLL_ENUM_DOC(INTERPOLATION_TYPE::LINEAR_FORWARDS, INTERPOLATION_TYPE_LINEAR_FORWARDS, L"JPMCDS", L"Linear forward interpolation", L"Linear forward interpolation");
XLL_ENUM_DOC(INTERPOLATION_TYPE::FLAT_FORWARDS, INTERPOLATION_TYPE_FLAT_FORWARDS, L"JPMCDS", L"Flat forward interpolation", L"Flat forward interpolation");

AddIn xai_jpmcds_instrument_money_market(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_money_market", L"JPMCDS.INSTRUMENT.MONEY.MARKET")
    .Arg(XLL_LONG, L"count", L"is the number of units in the tenor.")
    .Arg(XLL_CSTRING4, L"unit", L"is the unit corresponding ot count.")
    .Arg(XLL_LONG, L"dcc", L"is the day count convention. Default is Actual/360")
    .Arg(XLL_LONG, L"roll", L"is the rolling convention for payment dates. Default is following.")
    .Arg(XLL_LONG, L"days", L"is the number of days it takes to settle. Default is 1.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a money market instrument.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_instrument_money_market(LONG count, const char* unit, DAY_COUNT_CONVENTION dcc, 
    ROLL_CONVENTION roll, LONG days)
{
#pragma XLLEXPORT
    handlex h;

    try {
        if (dcc == 0)
            dcc = ACT_365F;
        if (roll == 0)
            roll = ROLL_FOLLOW;
        if (days == 0)
            days = 1;

        handle<Instrument::MoneyMarket> h_(new Instrument::MoneyMarket(TDateInterval{count, unit[0]}, dcc, roll, days));
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}
AddIn xai_jpmcds_instrument_money_market_set(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_money_market_set", L"JPMCDS.INSTRUMENT.MONEY.MARKET.SET")
    .Arg(XLL_DOUBLE, L"rate", L"is the money market rate.")
    .Arg(XLL_DOUBLE, L"valuation", L"is the valuation date for rate.")
    .FunctionHelp(L"Return a handle to a money market instrument after setting rate and valuation.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_instrument_money_market_set(HANDLEX h, double rate, double val)
{
#pragma XLLEXPORT
    try {
        handle<Instrument::MoneyMarket> h_(h);
        ensure (h_);
        h_->set(rate, (TDate)val);
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}
// MONEY.MARKET.CASH.FLOWS

AddIn xai_jpmcds_tcurve(
    Function(XLL_HANDLE, L"?xll_jpmcds_tcurve", L"JPMCDS.TCURVE")
    .Arg(XLL_LONG, L"baseDate", L"is the base date of the curve.")
    .Arg(XLL_FP, L"dates", L"is an array of curve dates.")
    .Arg(XLL_FP, L"rates", L"is an array of curve rates.")
    .Arg(XLL_LONG, L"basis", L"is the rate basis from the RATE_* enumeration.")
    .Arg(XLL_LONG, L"dayCountConvention", L"is the day count convention from the DAY_COUNT_* enumeration.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a rate curve.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_tcurve(LONG base, _FP12* dates, _FP12* rates, RATE_BASIS basis, DAY_COUNT_CONVENTION dcc)
{
#pragma XLLEXPORT
    handlex h;

    try {
        ensure (size(*dates) == size(*rates));
        
        int n = size(*dates);
        std::vector<LONG> d(n);
        std::transform(dates->array, dates->array + n, &d[0], [](double x) { return (LONG)x; });
        handle<Jpmcds::Curve> h_(new Jpmcds::Curve(base, &d[0], rates->array, n, basis, dcc));
        
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_tcurve_discount(
    Function(XLL_DOUBLE, L"?xll_jpmcds_tcurve_discount", L"JPMCDS.TCURVE.DISCOUNT")
    .Arg(XLL_HANDLE, L"handle", L"is a handle returned by JPMCDS.TCURVE.")
    .Arg(XLL_DOUBLE, L"date", L"is the date at which to calulate the discount.")
    .Arg(XLL_LONG, L"interpType", L"is the type of interpolation to use from the INTERPOLATION_TYPE_* enumeration. "
        L"Default is INTERPOLATION_TYPE_FLAT_FORWARDS.")
    .FunctionHelp(L"Return the discount to a date.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
double WINAPI xll_jpmcds_tcurve_discount(HANDLEX h, double date, INTERPOLATION_TYPE type)
{
#pragma XLLEXPORT
    double D = std::numeric_limits<double>::quiet_NaN();

    try {
        if (!type)
            type = INTERPOLATION_TYPE::FLAT_FORWARDS;

        handle<Jpmcds::Curve> h_(h);
        ensure (h_);

        D = h_->DiscountDate(static_cast<TDate>(date), type);
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return D;
}

#include "zerocurve.h"
/*
AddIn xai_jpmcds_build_ir_zero_curve(
    Function(XLL_HANDLE, L"?xll_jpmcds_build_ir_zero_curve", L"JPMCDS.BUILD.IR.ZERO.CURVE")
    .Arg(XLL_DOUBLE, L"valueDate", L"is the date the data are valid.")
    .Arg(XLL_LPOPER, L"type", L"is an array of 'M' (money market) and 'S' (swap) indicators.")
    .Arg(XLL_FP, L"dates", L"is an array of maturity dates of the instruments.")
    .Arg(XLL_FP, L"rates", L"is an array of rates for each instrument.")
    .Arg(XLL_LONG, L"mmDCC", L"is the day count convention for money market instruments. Default is ...")
    .Arg(XLL_LONG, L"fixedFreq", L"is the payment frequency of the fixed swap legs.")
    .Arg(XLL_LONG, L"floatFreq", L"is the payment frequency of the float swap legs.")
    .Arg(XLL_LONG, L"fixedDCC", L"is the day count convention for swap fixed legs. Default is ...")
    .Arg(XLL_LONG, L"floatDCC", L"is the day count convention for swap float legs. Default is ...")
    .Arg(XLL_LONG, L"dayConv", L"is the business day rolling convention.")
    .FunctionHelp(L"Return handle to zero curve.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_build_ir_zero_curve(double valueDate, LPOPER type, _FP12* dates, _FP12* rates,
    DAY_COUNT_CONVENTION mmdcc, FREQUENCY fixedFreq, FREQUENCY floatFreq,
    DAY_COUNT_CONVENTION fixedDCC, DAY_COUNT_CONVENTION floatDCC, DAY_COUNT_CONVENTION dayConv)
{
#pragma XLLEXPORT
    handlex h;

    try {
        TCurve* p = JpmcdsBuildIRZeroCurve(); calls...
        {
        zcurveIni = JpmcdsNewTCurve(valueDate, 0, (double) 1L, JPMCDS_ACT_365F);
        zcurveCash = JpmcdsZCCash(zcurveIni, cashDates, cashRates, nCash, mmDCC);
        zcurveSwap = JpmcdsZCSwaps(zcurveCash,
                            NULL,   // discZC 
                            swapDates,
                            swapRates,
                            nSwap,
                            fixedSwapFreq,
                            floatSwapFreq,
                            fixedSwapDCC,
                            floatSwapDCC,
                            fwdLength,
                            badDayConv,
                            holidayFile);

            JpmcdsZCAddSwaps
        }
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}
*/
    
    
    