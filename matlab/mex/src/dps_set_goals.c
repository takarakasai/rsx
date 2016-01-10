/*=================================================================
 * dps_set_goals.c
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 2015- Deshi Prompt.
 * All rights reserved.
 *=================================================================*/

#include "dps_proxy.h"

#include "helper.h"

#include "mex.h"
#include "matrix.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) { 
    
    if (nrhs < 1 || !mxIsDouble(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_goals [angle1, angle2, ...](double"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE_ARRAY(double, len, angle, prhs[0]);
    
    mexPrintf("id:all :");
    for (size_t i = 0; i < len; i++) {
      mexPrintf(" %lf", angle[i]);
    }
    mexPrintf("\n");

    errno_t eno = dps_proxy_set_goals(len, (float64_t*)angle);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

