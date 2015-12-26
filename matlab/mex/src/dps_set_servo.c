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

    mexPrintf(" %d\n", nrhs);
    
    if (nrhs < 1 || !mxIsUint8(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_set_servo [id1,id2,id3,...](uint8_t)"); 
        return;
    }

    mexPrintf(" %d\n", nrhs);

    DP_MEX_GET_PRIMITIVE_ARRAY(uint8_t, num, ids, prhs[0]);
  
    mexPrintf(" %d :", num);
    for (size_t i = 0; i < num; i++) {
      mexPrintf(" %02x", ids[i]);
    }
    mexPrintf("\n");

    errno_t eno = dps_proxy_set_servo(num, ids);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

