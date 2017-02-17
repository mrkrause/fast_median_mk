/* fast_median_lib.cpp
 * Ver 0.87
 * Peter H. Li 2013 FreeBSD License 
 * See nth_element.m for documentation. 
 */
#ifndef FAST_MEDIAN_LIB_CPP
#define FAST_MEDIAN_LIB_CPP

#include <algorithm>
#include <cstdint>

#include "mex.h"
#include "nth_element_lib.cpp"

/**
 * This runs on indata inplace!
 * Assumes inputs have been checked for 
 */
template <typename T> 
mxArray *fast_median(T *indata, mxArray *arr) {
  const mwSize nrows = mxGetM(arr);
  const mwSize ncols = mxGetN(arr);
  
  // Catch special case of 0xN input
  if (nrows == 0) {
      mxArray *result = mxCreateNumericMatrix(0, 0, mxGetClassID(arr), mxREAL);
      return result;
  }
  
  // Find halfway point, i.e. the rank of the median
  const mwIndex half = nrows / 2;
  
  // Loop through columns and iteratively pivot to put median in rank position
  nth_element_cols(indata, half, ncols, nrows);

  // Create output array, get pointer to its internal data
  mxArray *result = mxCreateNumericMatrix(1, ncols, mxGetClassID(arr), mxREAL);
  T *outdata = (T *) mxGetData(result);

  // Get output values from pivoted indata, assign to output
  for (mwIndex i = 0; i < ncols; ++i) {
    outdata[i] = indata[(i * nrows) + half];
  }

  // If even number of elements, we have more work to do
  if (half * 2 == nrows) {
    #pragma omp parallel for
    for (mwIndex i = 0; i < ncols; i++) {
      mwIndex start = i * nrows;
      T *median2 = std::max_element(indata + start, indata + start + half);

      outdata[i] = (0.5 * outdata[i]) + (0.5 * *median2);
    }
  }

  return result;
}


/**
 * Determine type of data, run fast_median, assign output
 * This runs on inarr inplace!
 */
mxArray *run_fast_median(mxArray *inarr) {
  void *indata = mxGetData(inarr);
  mxArray *outarr;

  switch (mxGetClassID(inarr)) {
    case mxDOUBLE_CLASS:
      outarr = fast_median(static_cast<double*>(indata), inarr);
      break;

    case mxSINGLE_CLASS:
      outarr = fast_median(static_cast<float*>(indata), inarr);
      break;

    case mxINT8_CLASS:
      outarr = fast_median(static_cast<std::int8_t*>(indata), inarr);
      break;

    case mxUINT8_CLASS:
      outarr = fast_median(static_cast<std::uint8_t*>(indata), inarr);
      break;

    case mxINT16_CLASS:
      outarr = fast_median(static_cast<std::int16_t*>(indata), inarr);
      break;

    case mxUINT16_CLASS:
      outarr = fast_median(static_cast<std::uint16_t*>(indata), inarr);
      break;

    case mxINT32_CLASS:
      outarr = fast_median(static_cast<std::int32_t*>(indata), inarr);
      break;

    case mxUINT32_CLASS:
      outarr = fast_median(static_cast<std::uint32_t*>(indata), inarr);
      break;
    
    case mxINT64_CLASS:
      outarr = fast_median(static_cast<std::int64_t*>(indata), inarr);
      break;

    case mxUINT64_CLASS:
      outarr = fast_median(static_cast<std::uint64_t*>(indata), inarr);
      break;

    default:
      mexErrMsgIdAndTxt("Numerical:fast_median:prhs", "Unrecognized numeric array type.");
  }

  return outarr;
}

#endif
