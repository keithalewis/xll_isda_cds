// xll_isda_cds.cpp
#include "jpmcds.h"
#include "xll_isda_cds.h"

using namespace Jpmcds;
using namespace xll;

AddIn xai_isda_cds(
    Documentation(L"The JP Morgan ISDA CDS model.")
);

XLL_ENUM_DOC(RATE_BASIS::CONTINUOUS_BASIS, RATE_BASIS_CONTINUOUS_BASIS, L"JPMCDS", L"Means continuous compounding", L"Means continuous compounding");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_RATE, RATE_BASIS_DISCOUNT_RATE, L"JPMCDS", L"Discount RATE (not factor) K's b-Day", L"Discount RATE (not factor) K's b-Day");
XLL_ENUM_DOC(RATE_BASIS::SIMPLE_BASIS, RATE_BASIS_SIMPLE_BASIS, L"JPMCDS", L"Means simple interest", L"Means simple interest");
XLL_ENUM_DOC(RATE_BASIS::ANNUAL_BASIS, RATE_BASIS_ANNUAL_BASIS, L"JPMCDS", L"Annual compounding", L"Annual compounding");
XLL_ENUM_DOC(RATE_BASIS::DISCOUNT_FACTOR, RATE_BASIS_DISCOUNT_FACTOR, L"JPMCDS", L"Rate is stored as a discount factor", L"Rate is stored as a discount factor");

XLL_ENUM_DOC(DAY_COUNT_CONVENTION::BAD_DCC, DAY_COUNT_CONVENTION_BAD_DCC, L"JPMCDS", L"Dummmy Day Count Conv for Brazilian bonds", L"Dummmy Day Count Conv for Brazilian bonds");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365, DAY_COUNT_CONVENTION_ACT_365, L"JPMCDS", L"Actual/365", L"Actual/365");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_365F, DAY_COUNT_CONVENTION_ACT_365F, L"JPMCDS", L"Actual/365 Fixed", L"Actual/365 Fixed");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_360, DAY_COUNT_CONVENTION_ACT_360, L"JPMCDS", L"Actual/360", L"Actual/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30_360, DAY_COUNT_CONVENTION_B30_360, L"JPMCDS", L"30/360", L"30/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::B30E_360, DAY_COUNT_CONVENTION_B30E_360, L"JPMCDS", L"30E/360", L"30E/360");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::ACT_ACT, DAY_COUNT_CONVENTION_ACT_ACT, L"JPMCDS", L"Same as Actual/365", L"Same as Actual/365");
XLL_ENUM_DOC(DAY_COUNT_CONVENTION::EFFECTIVE_RATE, DAY_COUNT_CONVENTION_EFFECTIVE_RATE, L"JPMCDS", L"Effective rate - YF always 1", L"Effective rate - YF always 1");

XLL_ENUM_DOC(INTERPOLATION_TYPE::LINEAR_FORWARDS, INTERPOLATION_TYPE_LINEAR_FORWARDS, L"JPMCDS", L"Linear forward interpolation", L"Linear forward interpolation");
XLL_ENUM_DOC(INTERPOLATION_TYPE::FLAT_FORWARDS, INTERPOLATION_TYPE_FLAT_FORWARDS, L"JPMCDS", L"Flat forward interpolation", L"Flat forward interpolation");

AddIn xai_jpmcds_tcurve(
    Function(XLL_HANDLE, L"?xll_jpmcds_tcurve", L"JPMCDS.TCURVE")
    .Arg(XLL_LONG, L"baseDate", L"is the base date or the curve.")
    .Arg(XLL_FP, L"dates", L"is an array of curve dates.")
    .Arg(XLL_FP, L"rates", L"is an array of curve rates.")
    .Arg(XLL_LONG, L"basis", L"is the rate basis from the RATE_BASIS_* enumeration.")
    .Arg(XLL_LONG, L"dayCountConvention", L"is the day count convention from the DAY_COUNT_CONVENTION_* enumeration.")
    .Uncalced()
    .FunctionHelp(L"Return a handle to a rate curve.")
    .Category(L"JPMCDS")
    .Documentation(L"")
);
HANDLEX WINAPI xll_jpmcds_tcurve(LONG base, _FP12* dates, _FP12* rates, RATE_BASIS basis, DAY_COUNT_CONVENTION dcb)
{
#pragma XLLEXPORT
    handlex h;

    try {
        ensure (size(*dates) == size(*rates));
        
        int n = size(*dates);
        std::vector<LONG> d(n);
        std::transform(dates->array, dates->array + n, &d[0], [](double x) { return (LONG)x; });
        handle<Jpmcds::TCurve_> h_(new Jpmcds::TCurve_(base, &d[0], rates->array, n, basis, dcb));
        
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

        handle<Jpmcds::TCurve_> h_(h);
        ensure (h_);

        D = h_->DiscountDate(static_cast<TDate>(date), type);
    }
    catch (const std::exception& ex) {
        XLL_ERROR(ex.what());
    }

    return D;
}
