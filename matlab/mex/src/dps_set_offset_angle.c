/*=================================================================
 * dps_set_offset_angle.c 
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
          "dps_set_offset_angle id(uint8_t) offset_angle(double)"); 
        return;
    }

    DP_MEX_GET_PRIMITIVE(uint8_t, id, prhs[0]);
    DP_MEX_GET_PRIMITIVE(double, oangle, prhs[1]);
    
    mexPrintf("id:%02x oangle:%lf\n", id, oangle);

    errno_t eno = dps_proxy_set_offset_angle(id, (float64_t)oangle);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

