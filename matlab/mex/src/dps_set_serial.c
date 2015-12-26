/*=================================================================
 * dps_set_serial.c
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
    
    if (nrhs < 1 || !mxIsLogical(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_set_serial use_serial(bool)"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE(bool, use_serial, prhs[0]);
    
    mexPrintf("use_serial:%s\n",  use_serial ? "true" : "false");

    errno_t eno = dps_proxy_set_serial(use_serial);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

