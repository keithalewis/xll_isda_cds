// xll_isda_cds.cpp
#include "jpmcds.h"
#include "xll_isda_cds.h"

using namespace Jpmcds;
using namespace xll;

// Date stupidity
inline TDate excel2TDate(double date)
{
    return (TDate)date + 109205;
}
inline double TDate2excel(TDate date)
{
    return date - 109205;
}

AddIn xai_isda_cds(
    Documentation(L"The JP Morgan ISDA CDS model.")
);

XLL_ENUM_DOC(FREQUENCY::FREQ_ANNUAL, FREQ_ANNUAL, L"JPMCDS", L"Once per year.", L"Once per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_SEMIANNUAL, FREQ_SEMIANNUAL, L"JPMCDS", L"Twice per year.", L"Twice per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_QUARTERLY, FREQ_QUARTERLY, L"JPMCDS", L"Four times per year.", L"Four times per year.");
XLL_ENUM_DOC(FREQUENCY::FREQ_MONTHLY, FREQ_MONTHLY, L"JPMCDS", L"Twelve times per year.", L"Twelve times per year.");

XLL_ENUM_DOC(RATE_BASIS::CONTINUOUS_BASIS, RATE_CONTINUOUS_BASIS, L"JPMCDS", L"Continuous compounding.", L"Continuous compounding.");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_RATE, RATE_DISCOUNT_RATE, L"JPMCDS", L"Discount RATE (not factor) K's b-Day.", L"Discount RATE (not factor) K's b-Day.");
XLL_ENUM_DOC(RATE_BASIS::SIMPLE_BASIS, RATE_SIMPLE_BASIS, L"JPMCDS", L"Simple interest.", L"Simple interest.");
XLL_ENUM_DOC(RATE_BASIS::ANNUAL_BASIS, RATE_ANNUAL_BASIS, L"JPMCDS", L"Annual compounding.", L"Annual compounding.");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_FACTOR, RATE_DISCOUNT_FACTOR, L"JPMCDS", L"Rate is stored as a discount factor.", L"Rate is stored as a discount factor.");

XLL_ENUM_DOC(DAY_COUNT_CONVENTION::BAD_DCC, DAY_COUNT_BAD_DCC, L"JPMCDS", L"Dummmy Day Count Conv for Brazilian bonds.", L"Dummmy Day Count Conv for Brazilian bonds.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365, DAY_COUNT_ACT_365, L"JPMCDS", L"Actual/365", L"Number of days divided by 365.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365F, DAY_COUNT_ACT_365F, L"JPMCDS", L"Actual/365 Fixed", L"Number of days divided by 365 Fixed.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_360, DAY_COUNT_ACT_360, L"JPMCDS", L"Actual/360", L"Number of days divided by 360.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30_360, DAY_COUNT_B30_360, L"JPMCDS", L"30/360", L"30 days per month, 360 days per year.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30E_360, DAY_COUNT_B30E_360, L"JPMCDS", L"30E/360", L"30 days per month, 360 days per year European convention.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_ACT, DAY_COUNT_ACT_ACT, L"JPMCDS", L"Same as Actual/365.", L"Same as Actual/365.");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::EFFECTIVE_RATE, DAY_COUNT_EFFECTIVE_RATE, L"JPMCDS", L"Effective rate - YF always 1", L"Effective rate - YF always 1");

XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_FOLLOW, ROLL_FOLLOW, L"JPMCDS", L"Following business day.", L"Following business day.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_PREVIOUS, ROLL_PREVIOUS, L"JPMCDS", L"Previous business day.", L"Previous business day.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_NONE, ROLL_NONE, L"JPMCDS", L"No adustment.", L"No adustment.");
XLL_ENUM_DOC(ROLL_CONVENTION::ROLL_MODIFIED, ROLL_MODIFIED, L"JPMCDS", L"Following business day unless at end of month.", L"Following business day unless at end of month.");

XLL_ENUM_DOC(INTERPOLATION_TYPE::LINEAR_FORWARDS, INTERPOLATION_TYPE_LINEAR_FORWARDS, L"JPMCDS", L"Linear forward interpolation.", L"Linear forward interpolation.");
XLL_ENUM_DOC(INTERPOLATION_TYPE::FLAT_FORWARDS, INTERPOLATION_TYPE_FLAT_FORWARDS, L"JPMCDS", L"Flat forward interpolation.", L"Flat forward interpolation.");

XLL_ENUM_DOC(PROT_PAY_DEF, PROT_PAY_DEF, L"JPMCDS", L"Default", L"Protection payment is due at default.");
XLL_ENUM_DOC(PROT_PAY_MAT, PROT_PAY_MAT, L"JPMCDS", L"Maturity", L"Protection payment is only due at the maturity of the protection leg.");

XLL_ENUM_DOC(ACCRUAL_PAY_NONE, ACCRUAL_PAY_NONE, L"JPMCDS", L"None", L"No accrual in case of default.");
XLL_ENUM_DOC(ACCRUAL_PAY_ALL, ACCRUAL_PAY_ALL, L"JPMCDS", L"All", L"Interest since the last accrual date is due in case of default.");

AddIn xai_jpmcds_instrument_money_market(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_money_market", L"JPMCDS.INSTRUMENT.MONEY.MARKET")
    .Arg(XLL_LONG, L"count", L"is the number of units in the tenor.")
    .Arg(XLL_CSTRING4, L"unit", L"is the unit corresponding to count.")
    .Arg(XLL_LONG, L"dcc", L"is the day count convention for the DAY_COUNT_* enumeration. Default is Actual/360")
    .Arg(XLL_LONG, L"roll", L"is the rolling convention for payment dates from the ROLL_* enumeration. Default is following business day.")
    .Arg(XLL_LONG, L"days", L"is the number of days it takes to settle. Default is 3.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a money market instrument.")
    .Category(L"JPMCDS")
    .Documentation(L"Short term instrument.")
);
HANDLEX WINAPI xll_jpmcds_instrument_money_market(LONG count, const char* unit, DAY_COUNT_CONVENTION dcc, 
    ROLL_CONVENTION roll, LONG days)
{
#pragma XLLEXPORT
    handlex h;

    try {
        if (dcc == 0)
            dcc = ACT_360;
        if (roll == 0)
            roll = ROLL_FOLLOW;
        if (days == 0)
            days = 3;

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
    .Arg(XLL_HANDLE, L"hande", L"is a handle returned by PMCDS.INSTRUMENT.MONEY.MARKET.")
    .Arg(XLL_DOUBLE, L"rate", L"is the money market rate.")
    .Arg(XLL_DOUBLE, L"valuation", L"is the valuation date of the rate.")
    .FunctionHelp(L"Return a handle to a money market instrument after setting rate and valuation.")
    .Category(L"JPMCDS")
    .Documentation(L"Fix the rate and valuation date.")
);
HANDLEX WINAPI xll_jpmcds_instrument_money_market_set(HANDLEX h, double rate, double val)
{
#pragma XLLEXPORT
    try {
        handle<Instrument::MoneyMarket> h_(h);
        ensure (h_.ptr());
        h_->set(rate, excel2TDate(val));
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}
// MONEY.MARKET.CASH.FLOWS
AddIn xai_jpmcds_instrument_money_market_flows(
    Function(XLL_FP, L"?xll_jpmcds_instrument_money_market_flows", L"JPMCDS.INSTRUMENT.MONEY.MARKET.FLOWS")
    .Arg(XLL_HANDLE, L"hande", L"is a handle returned by JPMCDS.INSTRUMENT.MONEY.MARKET.")
    .FunctionHelp(L"Return a two row array of cash flow dates and amounts.")
    .Category(L"JPMCDS")
    .Documentation(L"Return cash flow dates and amounts associated with the money market instrument.")
);
_FP12* WINAPI xll_jpmcds_instrument_money_market_flows(HANDLEX h)
{
#pragma XLLEXPORT
    static xll::FP12 dc(2,2);

    try {
        handle<Instrument::MoneyMarket> h_(h);
        ensure(h_.ptr());
        const auto& d = h_->date();
        dc(0,0) = TDate2excel(d[0]); dc(0,1) = TDate2excel(d[1]);
        const auto& c = h_->cash();
        dc(1,0) = c[0]; dc(1,1) = c[1];
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());

        return 0;
    }

    return dc.get();
}

AddIn xai_jpmcds_instrument_interest_rate_swap(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_interest_rate_swap", L"JPMCDS.INSTRUMENT.INTEREST.RATE.SWAP")
    .Arg(XLL_LONG, L"count", L"is the number of units in the tenor.")
    .Arg(XLL_CSTRING4, L"unit", L"is the unit corresponding ot count.")
    .Arg(XLL_LONG, L"freq", L"is the payment frequency from the FREQ_* enumeration.")
    .Arg(XLL_LONG, L"dcc", L"is the day count convention from the DAY_COUNT_* enumeration. Default is Actual/360")
    .Arg(XLL_LONG, L"roll", L"is the rolling convention for payment dates from the ROLL_* enumeration. Default is modified following business day.")
    .Arg(XLL_LONG, L"days", L"is the number of days it takes to settle. Default is 3.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a swap instrument.")
    .Category(L"JPMCDS")
    .Documentation(L"Interest rate swap instrument.")
);
HANDLEX WINAPI xll_jpmcds_instrument_interest_rate_swap(LONG count, const char* unit, FREQUENCY freq, DAY_COUNT_CONVENTION dcc,
    ROLL_CONVENTION roll, LONG days)
{
#pragma XLLEXPORT
    handlex h;

    try {
        if (freq == 0)
            freq = FREQ_SEMIANNUAL;
        if (dcc == 0)
            dcc = ACT_360;
        if (roll == 0)
            roll = ROLL_MODIFIED;
        if (days == 0)
            days = 3;

        handle<Instrument::InterestRateSwap> h_(new Instrument::InterestRateSwap(TDateInterval{count, unit[0]}, freq, dcc, roll, days));
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}
AddIn xai_jpmcds_instrument_interest_rate_swap_set(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_interest_rate_swap_set", L"JPMCDS.INSTRUMENT.INTEREST.RATE.SWAP.SET")
    .Arg(XLL_HANDLE, L"hande", L"is a handle returned by PMCDS.INSTRUMENT.MONEY.MARKET.")
    .Arg(XLL_DOUBLE, L"rate", L"is the money market rate.")
    .Arg(XLL_DOUBLE, L"valuation", L"is the valuation date for rate.")
    .FunctionHelp(L"Return a handle to a money market instrument after setting rate and valuation.")
    .Category(L"JPMCDS")
    .Documentation(L"Fix the rate and valuation date.")
);
HANDLEX WINAPI xll_jpmcds_instrument_interest_rate_swap_set(HANDLEX h, double rate, double val)
{
#pragma XLLEXPORT
    try {
        handle<Instrument::InterestRateSwap> h_(h);
        ensure(h_.ptr());
        h_->set(rate, excel2TDate(val));
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_instrument_contingent_leg(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_contingent_leg", L"JPMCDS.INSTRUMENT.CDS.CONTINGENT.LEG")
    .Arg(XLL_DOUBLE, L"startDate", L"is the start of protection.")
    .Arg(XLL_DOUBLE, L"endDate", L"is the end of protection.")
    .Arg(XLL_DOUBLE, L"notional", L"is the notional amount of protection. Default is 1.")
    .Arg(XLL_LONG, L"payType", L"is the type of protection from the PROT_* enumeration. Default is PROT_PAY_DEF.")
    .Arg(XLL_BOOL, L"protectStart", L"start protection the day after start date. Default is false.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a contingent leg.")
    .Category(L"JMPCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_instrument_contingent_leg(double startDate, double endDate, double notional,
    TProtPayConv payType, BOOL protectStart)
{
#pragma XLLEXPORT
    handlex h;

    try {
        if (notional == 0)
            notional = 1;
        if (payType == 0)
            payType = PROT_PAY_DEF;

        handle<Instrument::ContingentLeg> h_(new Instrument::ContingentLeg(excel2TDate(startDate), excel2TDate(endDate),
            notional, payType, protectStart));
        ensure (h_.ptr());
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_instrument_fee_leg(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_fee_leg", L"JPMCDS.INSTRUMENT.CDS.FEE.LEG")
    .Arg(XLL_DOUBLE, L"startDate", L"is the start of protection.")
    .Arg(XLL_DOUBLE, L"endDate", L"is the end of protection.")
    .Arg(XLL_DOUBLE, L"coupon", L"is the coupon rate.")
    .Arg(XLL_BOOL, L"accOnDefault", L"is a pay accrual on defalut boolean.")
    .Arg(XLL_LONG, L"count", L"is the number of units in the payment interval. Default is 3.")
    .Arg(XLL_CSTRING4, L"unit", L"is the unit corresponding to the payment interval. Default is 'M' for months.")
    // stub method { pay at end, long stub }
    .Arg(XLL_DOUBLE, L"notional", L"is the notional amount of protection. Default is 1.")
    .Arg(XLL_LONG, L"dcc", L"is the day count convention from the DAY_COUNT_* enumeration. Default is Actual/360")
    .Arg(XLL_LONG, L"roll", L"is the rolling convention for payment dates from the ROLL_* enumeration. Default is modified following business day.")
    .Arg(XLL_BOOL, L"protectStart", L"start protection the day after start date. Default is false.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a fee leg.")
    .Category(L"JMPCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_instrument_fee_leg(double startDate, double endDate, double coupon,
    BOOL accOnDefault, LONG count, const char* unit, double notional, DAY_COUNT_CONVENTION dcc,
    ROLL_CONVENTION roll, BOOL protectStart)
{
#pragma XLLEXPORT
    handlex h;

    try {
        if (count == 0)
            count = 3;
        if (unit == 0 || *unit == 0)
            unit = "M";
        if (notional == 0)
            notional = 1;
        if (dcc == 0)
            dcc = ACT_360;
        if (roll == 0)
            roll = ROLL_MODIFIED;

        TStubMethod stubType{false,false};
        char* calendar = const_cast<char*>("NONE");
        TDateInterval dateInterval{count, unit[0]};
        handle<Instrument::FeeLeg> h_(new Instrument::FeeLeg(excel2TDate(startDate), excel2TDate(endDate),
            accOnDefault, &dateInterval, &stubType, notional, coupon, dcc, roll, calendar, protectStart));
        ensure(h_.ptr());
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_instrument_cds(
    Function(XLL_HANDLE, L"?xll_jpmcds_instrument_cds", L"JPMCDS.INSTRUMENT.CDS")
    .Arg(XLL_HANDLE, L"contingentLeg", L"is a handle to a contingent leg.")
    .Arg(XLL_HANDLE, L"feeLeg", L"is a handle to a fee leg.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a credit default swap.")
    .Category(L"JMPCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_instrument_cds(HANDLEX contingentLeg, HANDLEX feeLeg))
{
#pragma XLLEXPORT
    handlex h;

    try {
        handle<Instrument::ContingentLeg> c_(contingentLeg);
        ensure (c_.ptr());
        handle<Instrument::FeeLeg> f_(feeLeg);
        ensure(f_.ptr());
        handle<Instrument::CreditDefaultSwap> h_(new Instrument::CreditDefaultSwap(*c_, *f_));)
        ensure(h_.ptr());
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_tcurve(
    Function(XLL_HANDLE, L"?xll_jpmcds_tcurve", L"JPMCDS.TCURVE")
    .Arg(XLL_DOUBLE, L"baseDate", L"is the base date of the curve.")
    .Arg(XLL_FP, L"dates", L"is an array of curve dates.")
    .Arg(XLL_FP, L"rates", L"is an array of curve rates.")
    .Arg(XLL_LONG, L"basis", L"is the rate basis from the RATE_* enumeration.")
    .Arg(XLL_LONG, L"dayCountConvention", L"is the day count convention from the DAY_COUNT_* enumeration.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a rate curve.")
    .Category(L"JPMCDS")
    .Documentation(L"Create a curve from dates and rates.")
);
HANDLEX WINAPI xll_jpmcds_tcurve(double base, _FP12* dates, _FP12* rates, RATE_BASIS basis, DAY_COUNT_CONVENTION dcc)
{
#pragma XLLEXPORT
    handlex h;

    try {
        ensure (size(*dates) == size(*rates));
        
        int n = size(*dates);
        std::vector<LONG> d(n);
        std::transform(dates->array, dates->array + n, &d[0], [](double x) { return excel2TDate(x); });
        handle<Jpmcds::Curve> h_(new Jpmcds::Curve(excel2TDate(base), &d[0], rates->array, n, basis, dcc));
        
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

AddIn xai_jpmcds_tcurve_points(
    Function(XLL_FP, L"?xll_jpmcds_tcurve_points", L"JPMCDS.TCURVE.POINTS")
    .Arg(XLL_HANDLE, L"handle", L"is a handle to a Curve.")
    .FunctionHelp(L"Return two row array of dates and rates in curve.")
    .Category(L"JMPCDS")
    .Documentation(L"")
);
const _FP12* WINAPI xll_jpmcds_tcurve_points(HANDLEX h)
{
#pragma XLLEXPORT
    static xll::FP12 dr;

    try {
        handle<Curve> h_(h);
        ensure (h_.ptr() || !__FUNCTION__ ": failed to lookup pointer.");
        int n = h_->size();
        dr.resize(2, n);
        auto pt = h_->points();
        for (int i = 0; i < n; ++i) {
            dr(0, i) = TDate2excel(pt[i].fDate);
            dr(1, i) = pt[i].fRate;
        }
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());

        return 0;
    }

    return dr.get();
}

AddIn xai_jpmcds_tcurve_discount(
    Function(XLL_DOUBLE, L"?xll_jpmcds_tcurve_discount", L"JPMCDS.TCURVE.DISCOUNT")
    .Arg(XLL_HANDLE, L"handle", L"is a handle returned by JPMCDS.TCURVE.")
    .Arg(XLL_DOUBLE, L"date", L"is the date at which to calulate the discount.")
    .Arg(XLL_LONG, L"interpType", L"is the type of interpolation to use from the INTERPOLATION_TYPE_* enumeration. "
        L"Default is INTERPOLATION_TYPE_FLAT_FORWARDS.")
    .FunctionHelp(L"Return the discount to a date.")
    .Category(L"JPMCDS")
    .Documentation(L"Return the discount.")
);
double WINAPI xll_jpmcds_tcurve_discount(HANDLEX h, double date, INTERPOLATION_TYPE type)
{
#pragma XLLEXPORT
    double D = std::numeric_limits<double>::quiet_NaN();

    try {
        if (!type)
            type = INTERPOLATION_TYPE::FLAT_FORWARDS;

        handle<Jpmcds::Curve> h_(h);
        ensure (h_.get());

        D = h_->DiscountDate(excel2TDate(date), type);
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return D;
}

AddIn xai_jpmcds_tcurve_rate(
    Function(XLL_DOUBLE, L"?xll_jpmcds_tcurve_rate", L"JPMCDS.TCURVE.RATE")
    .Arg(XLL_HANDLE, L"handle", L"is a handle returned by JPMCDS.TCURVE.")
    .Arg(XLL_DOUBLE, L"date", L"is the date at which to calulate the rate.")
    .Arg(XLL_LONG, L"interpType", L"is the type of interpolation to use from the INTERPOLATION_TYPE_* enumeration. "
        L"Default is INTERPOLATION_TYPE_FLAT_FORWARDS.")
    .FunctionHelp(L"Return the rate to a date.")
    .Category(L"JPMCDS")
    .Documentation(L"Return the rate.")
);
double WINAPI xll_jpmcds_tcurve_rate(HANDLEX h, double date, INTERPOLATION_TYPE type)
{
#pragma XLLEXPORT
    double r = std::numeric_limits<double>::quiet_NaN();

    try {
        if (!type)
            type = INTERPOLATION_TYPE::FLAT_FORWARDS;

        handle<Jpmcds::Curve> h_(h);
        ensure(h_.ptr()); // || !__FUNCTION__ ": failed to lookup curve handle.");

        r = h_->InterpRate(excel2TDate(date), type);
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return r;
}

#include "gtozc.h"
#include "zerocurve.h"

AddIn xai_jpmcds_build_ir_zero_curve(
    Function(XLL_HANDLE, L"?xll_jpmcds_build_ir_zero_curve", L"JPMCDS.BUILD.IR.ZERO.CURVE")
    .Arg(XLL_DOUBLE, L"valueDate", L"is the date the data are valid.")
    .Arg(XLL_FP, L"cashs", L"is an array of money market cash deposits.")
    .Arg(XLL_FP, L"swaps", L"is an array of swaps.")
    .Uncalced()
    .FunctionHelp(L"Return handle to zero curve.")
    .Category(L"JPMCDS")
    .Documentation(L"Bootstrap a curve from money market instruments and swaps.")
);
HANDLEX WINAPI xll_jpmcds_build_ir_zero_curve(double valueDate, const _FP12* cashs, const _FP12* swaps)
{
#pragma XLLEXPORT
    handlex h;

    try {
        Jpmcds::Curve curve(excel2TDate(valueDate), 0, ANNUAL_BASIS, ACT_365F);

        int nCash = size(*cashs);
        if (!(nCash == 1 && cashs->array[0] == 0)) {
            std::vector<TDate> cashDates(nCash);
            std::vector<double> cashRates(nCash);
            DAY_COUNT_CONVENTION mmDCC = ACT_360;
            for (int i = 0; i < nCash; ++i) {
                handle<Instrument::MoneyMarket> hi(cashs->array[i]);
                ensure (hi.ptr());
                if (i == 0)
                    mmDCC = hi->dcc;
                else
                    ensure (mmDCC == hi->dcc);
                cashDates[i] = hi->maturity();
                cashRates[i] = hi->rate;
            }
            curve = Curve(JpmcdsZCCash(curve, &cashDates[0], &cashRates[0], nCash, mmDCC));
        }

        int nSwap = size(*swaps);
        if (!(nSwap == 1 && swaps->array[0] == 0)) {
            std::vector<TDate> swapDates(nSwap);
            std::vector<double> swapRates(nSwap);
            FREQUENCY fixedFreq = FREQ_SEMIANNUAL, floatFreq = FREQ_QUARTERLY;
            DAY_COUNT_CONVENTION fixedDCC = B30E_360, floatDCC = B30E_360;
            ROLL_CONVENTION rollConv = ROLL_FOLLOW;
            char* holidays = const_cast<char*>("NONE");
            for (int i = 0; i < nSwap; ++i) {
                handle<Instrument::InterestRateSwap> hi(swaps->array[i]);
                ensure (hi);
                if (i == 0) {
                    fixedFreq = hi->fixedLeg.freq;
                    floatFreq = hi->floatLeg.freq;
                    fixedDCC = hi->fixedLeg.dcc;
                    floatDCC = hi->floatLeg.dcc;
                    rollConv = hi->fixedLeg.roll;
                    ensure (rollConv == hi->floatLeg.roll);
                }
                else {
                    ensure(fixedFreq == hi->fixedLeg.freq);
                    ensure(floatFreq == hi->floatLeg.freq);
                    ensure(fixedDCC == hi->fixedLeg.dcc);
                    ensure(floatDCC == hi->floatLeg.dcc);
                    ensure(rollConv == hi->fixedLeg.roll);
                    ensure(rollConv == hi->floatLeg.roll);
                }
                swapDates[i] = hi->fixedLeg.maturity(excel2TDate(valueDate));
                swapRates[i] = hi->fixedLeg.rate;
            }
            curve = Curve(JpmcdsZCSwaps(curve, NULL, &swapDates[0], &swapRates[0], nSwap, 
                fixedFreq, floatFreq, fixedDCC, floatDCC, 0, rollConv, holidays));
        }
        handle<Curve> h_(new Curve(curve));
        //handle<Curve> h_(&curve);
        //curve.~Curve(); //??? can we move it ???
        h = h_.get();
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return h;
}

/*
// JpmcdsCdsoneUpfrontCharge - upfront given flat spread
AddIn xai_jpmcds_upfront_flat(
    Function(XLL_DOUBLE, L"?xll_jpmcds_upfront_flat", L"JPMCDS.UPFRONT.FLAT")
    .Arg(XLL_HANDLE, L"curve", L"is a handle to a curve.")
    .Arg(XLL_HANDLE, L"cds", L"is a handle to a credit default swap.")
    .FunctionHelp(L"Return the up-front fee for a flat credit spread.")
);
*/

// JpmcdsCdsoneSpread - flat spread
AddIn xai_jpmcds_one_spread(
    Function(XLL_DOUBLE, L"?xll_jpmcds_one_spread", L"JPMCDS_ONE_SPREAD")
    .Arg(XLL_HANDLE, L"curve", L"is a curve.")
    .FunctionHelp(L"Return the constant spread repricing the CDS.")
);
double WINAPI xll_jpmcds_one_spread(HANDLEX h)
{
#pragma XLLEXPORT
    double spread = std::numeric_limits<double>::quiet_NaN();

    h = h;

    return spread;
}

    
    