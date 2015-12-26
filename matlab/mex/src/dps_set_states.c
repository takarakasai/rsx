/*=================================================================
 * dps_set_states.c 
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
    
    if (nrhs < 1 || !mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_set_states off|break|on(string)"); 
        return;
    }

    DP_MEX_GET_STRING(len_str, str, prhs[0]);
    
    mexPrintf(" state for all : %s\n", str);

    errno_t eno = dps_proxy_set_states(str);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

