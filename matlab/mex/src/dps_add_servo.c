/*=================================================================
 * dps_set_servo.c
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
    
    if (nrhs < 1 || !mxIsUint8(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_add_servo id(uint8_t)"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE(uint8_t, id, prhs[0]);
    
    mexPrintf("id:%d\n",  id);

    errno_t eno = dps_proxy_add_servo(id);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

