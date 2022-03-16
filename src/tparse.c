#define USE_RINTERNALS 1

#include <Rinternals.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define DIGIT(X) ((X) >= '0' && (X) <= '9')

/* start of each month in seconds */
static const int cml[] = { 0, 0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400,
			   18316800, 20995200, 23587200, 26265600, 28857600, 31536000 };

typedef int64_t time_int_t;

/* separated input version, argitrary length of each component */
static SEXP parse_ts_(SEXP str, SEXP sRequiredComp, int date) {
    SEXP res;
    double *tsv;
    int required_components = Rf_asInteger(sRequiredComp);
    size_t n, i;
    if (TYPEOF(str) != STRSXP) Rf_error("invalid timestamp vector");
    n = XLENGTH(str);
    res = Rf_allocVector(REALSXP, n);
    tsv = REAL(res);
    for (i = 0; i < n; i++) {
	const char *c = CHAR(STRING_ELT(str, i));
	int comp = 0;
	double ts = 0.0;
	if (DIGIT(*c)) {
	    int y = 0, m = 0, d = 0, h = 0, mi = 0;
	    while (DIGIT(*c)) { y = y * 10 + (*c - '0'); c++; }
	    if (y < 100) y += 2000;
	    y -= 1970;
	    /* we only support the range of 1970-2199 to cover
	       unsigned int POSIX time without getting into more leap year mess */
	    if (y < 0 || y >= 230 ) {
		tsv[i] = NA_REAL;
		continue;
	    } else {
		/* adjust for all leap years prior to the current one */
	      ts += ((time_int_t)((y + 1) / 4)) * (time_int_t) 86400;
		if (y > 130) /* 2100 is an exception - not a leap year */
		    ts -= 86400;
		ts += ((time_int_t) y) * ((time_int_t) 31536000);
		comp++;
		while (*c && !DIGIT(*c)) c++;
		if (*c) {
		    while (DIGIT(*c)) { m = m * 10 + (*c - '0'); c++; }
		    if (m > 0 && m < 13) {
			ts += cml[m];
			if (m > 2 && (y & 3) == 2 && y != 130 /* 2100 again */) ts += 86400;
			comp++;
			while (*c && !DIGIT(*c)) c++;
			if (*c) {
			    while (DIGIT(*c)) { d = d * 10 + (*c - '0'); c++; }
			    if (d > 1) ts += (d - 1) * 86400;
			    comp++;
			    while (*c && !DIGIT(*c)) c++;
			    if (*c) {
				while (DIGIT(*c)) { h = h * 10 + (*c - '0'); c++; }
				ts += h * 3600;
				comp++;
				while (*c && !DIGIT(*c)) c++;
				if (*c) {
				    while (DIGIT(*c)) { mi = mi * 10 + (*c - '0'); c++; }
				    ts += mi * 60;
				    comp++;
				    while (*c && !(DIGIT(*c) || *c == '.')) c++;
				    if (*c) {
					ts += atof(c);
					comp++;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
	tsv[i] = (comp >= required_components) ? (date ? ts / 86400 : ts) : NA_REAL;
    }
    return res;
}

/* fixed version - limits components to <4/2><2><2><2><2><*>
   and does not need separators. Note that it will still skip
   separators if present */
static SEXP fparse_ts_(SEXP str, SEXP sRequiredComp, int date, int year_length) {
    SEXP res;
    double *tsv;
    int required_components = Rf_asInteger(sRequiredComp);
    size_t n, i;
    if (TYPEOF(str) != STRSXP) Rf_error("invalid timestamp vector");
    n = XLENGTH(str);
    res = Rf_allocVector(REALSXP, n);
    tsv = REAL(res);
    for (i = 0; i < n; i++) {
	const char *c = CHAR(STRING_ELT(str, i));
	int comp = 0;
	double ts = 0.0;
	if (DIGIT(*c)) {
	    int y = 0, m = 0, d = 0, h = 0, mi = 0, left = year_length;
	    while (left-- && DIGIT(*c)) { y = y * 10 + (*c - '0'); c++; }
	    if (y < 100) y += 2000;
	    y -= 1970;
	    /* we only support the range of 1970-2199 to cover
	       unsigned int POSIX time without getting into more leap year mess */
	    if (y < 0 || y >= 230 ) {
		tsv[i] = NA_REAL;
		continue;
	    } else {
		/* adjust for all leap years prior to the current one */
	      ts += ((time_int_t)((y + 1) / 4)) * (time_int_t) 86400;
		if (y > 130) /* 2100 is an exception - not a leap year */
		    ts -= 86400;
		ts += ((time_int_t) y) * ((time_int_t) 31536000);
		comp++;
		while (*c && !DIGIT(*c)) c++;
		if (*c) {
		    int monl = 2;
		    while (monl-- && DIGIT(*c)) { m = m * 10 + (*c - '0'); c++; }
		    if (m > 0 && m < 13) {
			ts += cml[m];
			if (m > 2 && (y & 3) == 2 && y != 130 /* 2100 again */) ts += 86400;
			comp++;
			while (*c && !DIGIT(*c)) c++;
			if (*c) {
			    int dayl = 2;
			    while (dayl-- && DIGIT(*c)) { d = d * 10 + (*c - '0'); c++; }
			    if (d > 1) ts += (d - 1) * 86400;
			    comp++;
			    while (*c && !DIGIT(*c)) c++;
			    if (*c) {
				int hleft = 2;
				while (hleft-- && DIGIT(*c)) { h = h * 10 + (*c - '0'); c++; }
				ts += h * 3600;
				comp++;
				while (*c && !DIGIT(*c)) c++;
				if (*c) {
				    int mileft = 2;
				    while (mileft-- && DIGIT(*c)) { mi = mi * 10 + (*c - '0'); c++; }
				    ts += mi * 60;
				    comp++;
				    while (*c && !(DIGIT(*c) || *c == '.')) c++;
				    if (*c) {
					ts += atof(c);
					comp++;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
	tsv[i] = (comp >= required_components) ? (date ? ts / 86400 : ts) : NA_REAL;
    }
    return res;
}

/* -- API -- */

static SEXP sym_tzone;

SEXP parse_ts(SEXP str, SEXP sRequiredComp, SEXP sFixed, SEXP sTZ) {
    SEXP res, cls;
    int fixed = Rf_asInteger(sFixed);
    int date = 0;

    /* we (ab)use tz of "Date" to signal that the user wants a date ;) */
    if (TYPEOF(sTZ) == STRSXP &&
	LENGTH(sTZ) == 1 &&
	!strcmp(CHAR(STRING_ELT(sTZ, 0)), "Date"))
	date = 1;

    /* call the correct parser */
    res = (fixed < 1) ? parse_ts_(str, sRequiredComp, date) : fparse_ts_(str, sRequiredComp, date, fixed);

    /* non-string values will make us return the naked object */
    if (TYPEOF(sTZ) != STRSXP && sTZ != R_NilValue) return res;
    PROTECT(res);
    if (date)
	Rf_classgets(res, Rf_mkString("Date"));
    else {
	/* if TZ is a string, turn this into a POSIXct object */
	if (sTZ != R_NilValue) {
	    if (!sym_tzone)
		sym_tzone = Rf_install("tzone");
	    Rf_setAttrib(res, sym_tzone, sTZ);
	}
	cls = PROTECT(allocVector(STRSXP, 2));
	SET_STRING_ELT(cls, 0, Rf_mkChar("POSIXct"));
	SET_STRING_ELT(cls, 1, Rf_mkChar("POSIXt"));    
	Rf_classgets(res, cls);
	UNPROTECT(1);
    }
    UNPROTECT(1);
    return res;
}



/* registration */

#include <R_ext/Rdynload.h>

static const R_CallMethodDef CallEntries[] = {
    {"parse_ts", (DL_FUNC) &parse_ts, 4},
    {NULL,       NULL,                0}
};

void R_init_fasttime(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
