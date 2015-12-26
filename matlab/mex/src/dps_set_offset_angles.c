/*=================================================================
 * dps_set_offset_angles.c
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
          "dps_offset_angles [angle1, angle2, ...](double)"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE_ARRAY(double, len, oangle, prhs[0]);
    
    mexPrintf("id:%02x :");
    for (size_t i = 0; i < len; i++) {
      mexPrintf(" %lf", oangle[i]);
    }
    mexPrintf("\n");

    errno_t eno = dps_proxy_set_offset_angles(len, (float64_t*)oangle);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

