/*=================================================================
 * dps_write_mem.c
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 2015- Deshi Prompt.
 * All rights reserved.
 *=================================================================*/

#include "dps_proxy.h"

#include "helper.h"
#include "ics/mmap/ics3x.h"

#include "mex.h"
#include "matrix.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) { 
    
    if (nrhs < 5 || !mxIsUint8(prhs[0]) || !mxIsUint8(prhs[1]) || !mxIsUint8(prhs[2]) || !mxIsUint8(prhs[3]) || !mxIsInt32(prhs[4])) {
        mexPrintf("%s id(uint8_t) start_addr(uint8_t) size(uint8_t) [data1,data2,...](uint8_t) option(int32_t)\n", mexFunctionName()); 
        mexPrintf("  option (rsx) : none\n");
        mexPrintf("  option (ics) : %d(ics ver3.0) or %d(ics ver3.5)\n", ICS_V30, ICS_V35);
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          ""); 
        return;
    }

    DP_MEX_GET_PRIMITIVE(uint8_t,       id,         prhs[0]);
    DP_MEX_GET_PRIMITIVE(uint8_t,       start_addr, prhs[1]);
    DP_MEX_GET_PRIMITIVE(uint8_t,       wsize,      prhs[2]);
    DP_MEX_GET_PRIMITIVE_ARRAY(uint8_t, len, wdata, prhs[3]);
    DP_MEX_GET_PRIMITIVE(int32_t,       option,     prhs[4]);
    
    mexPrintf("id:%02x start_addr:%02x size:%d option:%d\n", id, start_addr, wsize, option);
    mexPrintf(" data :");
    for (size_t i = 0; i < wsize; i++) {
      mexPrintf(" %02x", wdata[i]);
    }
    mexPrintf("\n");

    errno_t eno = dps_proxy_write_mem(id, start_addr, wsize, wdata, option);

    if (eno != EOK) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

