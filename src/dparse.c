#define USE_RINTERNALS 1

#include <Rinternals.h>
#include <stdlib.h>
#include <stdint.h>

#define DIGIT(X) ((X) >= '0' && (X) <= '9')

/* start of each month in seconds */
static const int cml[] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

typedef int64_t time_int_t;

SEXP parse_date(SEXP str, SEXP sRequiredComp) {
    SEXP res;
    double *tsv;
    int required_components = Rf_asInteger(sRequiredComp);
    int n, i, comp, ts;
    if (TYPEOF(str) != STRSXP) Rf_error("invalid date vector");
    n = LENGTH(str);
    res = Rf_allocVector(REALSXP, n);
    tsv = REAL(res);
    for (i = 0; i < n; i++) {
    	const char *c = CHAR(STRING_ELT(str, i));
        comp = 0;
        ts = 0;
    	if (DIGIT(*c)) {
    	    int y = 0, m = 0, d = 0;
    	    while (DIGIT(*c)) {
    	        y = y * 10 + (*c - '0');
    	        c++;
    	    }
    	    if (y < 100) {
    	        y += 30;   
    	    } else {
    	        y -= 1970;   
    	    }
    	    /* we only support the range of 1970-2199 to cover
    	       unsigned int POSIX time without getting into more leap year mess */
    	    if (y < 0 || y >= 230 ) {
        		tsv[i] = NA_REAL;
        		continue;
    	    } else {
        		/* adjust for all leap years prior to the current one */
        	    ts += ((time_int_t)((y + 1) / 4)) * (time_int_t) 1;
        		if (y > 130) /* 2100 is an exception - not a leap year */
        		    ts--;
        		ts += ((time_int_t) y) * ((time_int_t) 365);
        		comp++;
        		while (*c && !DIGIT(*c)) c++;
        		if (*c) {
        		    while (DIGIT(*c)) {
        		        m = m * 10 + (*c - '0');
        		        c++;
        		    }
        		    if (m > 0 && m < 13) {
            			ts += cml[m];
            			if (m > 2 && (y & 3) == 2 && y != 130 /* 2100 again */) {
            			    ts++;   
            			}
            			comp++;
            			while (*c && !DIGIT(*c)) c++;
            			if (*c) {
            			    while (DIGIT(*c)) { d = d * 10 + (*c - '0'); c++; }
            			    if (d > 1) ts += (d - 1) * 1;
            			    comp++;
            			}
        		    }
        		}
    	    }
    	}
    	tsv[i] = (comp >= required_components) ? ts : NA_REAL;
    }
    return res;
}
