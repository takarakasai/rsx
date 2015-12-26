/*=================================================================
 * dps_assign.c 
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 2015- Deshi Prompt.
 * All rights reserved.
 *=================================================================*/

#include "dps_proxy.h"

#include "mex.h"
#include "matrix.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) { 

    if (nrhs < 1 || !mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt(
          "dps_proxy:EINVAL",
          "dps_assign ics|rsx(string)"); 
        return;
    }

    int num = mxGetNumberOfElements(prhs[0]);
    char name[num + 1];
    if (mxGetString(prhs[0], name, num + 1) != 0) {
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    errno_t eno = dps_proxy_assign(name);

    if (eno != EOK) {
        mexPrintf("The input string is:  %s\n", name);
        mexErrMsgIdAndTxt("dps_proxy:ERR","");
    }

    return;
}

