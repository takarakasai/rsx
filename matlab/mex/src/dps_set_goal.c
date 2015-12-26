/*=================================================================
 * dps_set_goal.c
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
    
    if (nrhs < 2 || !mxIsUint8(prhs[0]) || !mxIsDouble(prhs[1])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_set_goal id(uint8_t) angle(double)"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE(uint8_t, id, prhs[0]);
    DP_MEX_GET_PRIMITIVE(double, angle, prhs[1]);
    
    mexPrintf("id:%02x angle:%lf\n", id, angle);

    errno_t eno = dps_proxy_set_goal(id, (float64_t)angle);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

