/*=================================================================
 * dps_open.c 
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
    
    if (nrhs < 3 || !mxIsChar(prhs[0]) || !mxIsChar(prhs[1]) || !mxIsInt32(prhs[2])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_open device(string) port(string) baudrate(int)"); 
        return;
    }

    DP_MEX_GET_STRING(dev_len, device, prhs[0]); 
    DP_MEX_GET_STRING(port_len, port, prhs[1]); 
    DP_MEX_GET_PRIMITIVE(int32_t, baudrate, prhs[2]);
    
    mexPrintf("dev:%s port:%s baudrate:%d\n",  device, port, baudrate);

    errno_t eno = dps_proxy_open(device, port, baudrate);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

