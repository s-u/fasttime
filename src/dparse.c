#define USE_RINTERNALS 1

#include <Rinternals.h>
#include <stdlib.h>
#include <stdint.h>

#define DIGIT(X) ((X) >= '0' && (X) <= '9')

/* start of each month in days */
static const int cml[] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

typedef int64_t time_int_t;

SEXP parse_date(SEXP str, SEXP sRequiredComp) {
    SEXP res;
    double *tsv;
    int warn_nas = 0;
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
    	    /* we only support the range of 1901-2500 to cover
    	       unsigned int dates without getting into more leap year mess */
    	    if (y <= -70) {
	            tsv[i] = NA_REAL;
    	        warn_nas = 1;
	            continue;
    	    } else {
        		/* adjust for all leap years prior to the current one
        		 * Work with dates before 1970 first
        		 */
                if (y < 0) {
                    if ((y-2) % 4 == 0) {
                        ts--;
                    }
                    ts += ((time_int_t)((y - 1) / 4)) * (time_int_t) 1;
                    
                /* Then work with dates after 1970 */
                } else {
                    ts += ((time_int_t)((y + 1) / 4)) * (time_int_t) 1;
                    if (y > 130) {
                        /* compensate for 2100, 2200, 2300, 2500:
                         * NOT leap years */
                        if (y <= 230) {
                            ts--;
                        } else if (y <= 330) {
                            ts -= 2;
                        } else if (y <= 530) {
                            ts -= 3;
                        } else {
                            tsv[i] = NA_REAL;
                            warn_nas = 1;
                            continue;
                        }
                    }
                }
        		
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
            			if (m > 2 && (y & 3) == 2 &&
                            y != 130 && y !=  230 && y != 330 && y != 530 /* 2100 again */) {
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
    /* send a warning to the console if NAs were introduced */
    if (warn_nas == 1) {
        Rf_warning("NAs introduced for out of range dates.");
    }
    return res;
}
