// depends.cpp
#include "xll12/xll/xll.h"

#ifndef CATEGORY
#define CATEGORY L"Utility"
#endif

using namespace xll;

static AddIn xai_depends(
	Function(XLL_LPXLOPER, L"?xll_depends", L"DEPENDS")
    .Arg(XLL_LPXLOPER, L"value", L"is the value to return.")
    .Arg(XLL_LPXLOPER, L"dependent", L"is a reference to the cell that value depends on.")
	.Category(CATEGORY)
    .FunctionHelp(L"Return value after dependent has been computed.")
    .Documentation(
	    L"Excel calculation order is unspecified for independent formulas. This function "
        L"can be used to make that deterministic."
    )
);
LPXLOPER12 WINAPI xll_depends(LPXLOPER12 pRef, LPXLOPER12 pDep)
{
#pragma XLLEXPORT

	return xlretUncalced == ::Excel12(xlCoerce, 0, 1, pDep) ? 0 : pRef;
}
