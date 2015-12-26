
#ifndef HELPER_H
#define HELPER_H

#define DP_MEX_GET_STRING(len, str, mxarray)       \
  size_t len = mxGetNumberOfElements(mxarray) + 1; \
  char8_t str[len];                                \
  if (mxGetString(mxarray, str, len) != 0) {       \
      mexErrMsgIdAndTxt("dps_proxy:ERR","");       \
  }

#define DP_MEX_GET_PRIMITIVE(type, name, mxarray)  \
  type name = *(type *)mxGetData(mxarray);

#define DP_MEX_GET_PRIMITIVE_ARRAY(type, len, name, mxarray)  \
  size_t len = mxGetNumberOfElements(mxarray);                \
  type name[len];                                             \
  type *pname = (type *)mxGetData(mxarray);                   \
  for (size_t i = 0; i < len; i++) {                          \
    name[i] = pname[i];                                       \
  }

#endif

