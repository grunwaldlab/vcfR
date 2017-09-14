// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// timesTwo
int timesTwo(int x);
RcppExport SEXP _vcfR_timesTwo(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(timesTwo(x));
    return rcpp_result_gen;
END_RCPP
}
// write_vcf_body
void write_vcf_body(Rcpp::CharacterMatrix fix, Rcpp::CharacterMatrix gt, std::string filename, int mask);
RcppExport SEXP _vcfR_write_vcf_body(SEXP fixSEXP, SEXP gtSEXP, SEXP filenameSEXP, SEXP maskSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::CharacterMatrix >::type fix(fixSEXP);
    Rcpp::traits::input_parameter< Rcpp::CharacterMatrix >::type gt(gtSEXP);
    Rcpp::traits::input_parameter< std::string >::type filename(filenameSEXP);
    Rcpp::traits::input_parameter< int >::type mask(maskSEXP);
    write_vcf_body(fix, gt, filename, mask);
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_vcfR_timesTwo", (DL_FUNC) &_vcfR_timesTwo, 1},
    {"_vcfR_write_vcf_body", (DL_FUNC) &_vcfR_write_vcf_body, 4},
    {NULL, NULL, 0}
};

RcppExport void R_init_vcfR(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
