/*=================================================================
 * dps_get_num_of_servo.c 
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
    
    uint8_t num = dps_proxy_get_num_of_servo();

    mexPrintf(" num_of_servo : %d\n", num);

    plhs[0] = mxCreateDoubleScalar((double)num);

    return;
}

