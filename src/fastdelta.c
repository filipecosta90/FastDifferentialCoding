#include "fastdelta.h"
#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) &&                               \
    (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

// write to output the successive differences of input (input[0]-starting_point,
// input[1]-input[2], ...) there are "length" values in input and output input
// and output must be distinct
void compute_deltas(const uint32_t *__restrict__ input, size_t length,
                    uint32_t *__restrict__ output, uint32_t starting_point) {
  __m128i prev = _mm_set1_epi32(starting_point);
  size_t i = 0;
  for (; i < length / 4; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)input + i);
    __m128i delta = _mm_sub_epi32(curr, _mm_alignr_epi8(curr, prev, 12));
    _mm_storeu_si128((__m128i *)output + i, delta);
    prev = curr;
  }
  uint32_t lastprev = _mm_extract_epi32(prev, 3);
  for (i = 4 * i; i < length; ++i) {
    uint32_t curr = input[i];
    output[i] = curr - lastprev;
    lastprev = curr;
  }
}

void compute_deltas64(const int64_t *__restrict__ input, size_t length,
                      int64_t *__restrict__ output, int64_t starting_point) {
  __m128i prev = _mm_set1_epi64x(starting_point);
  size_t i = 0;
  for (; i < length / 2; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)input + i);
    __m128i delta = _mm_sub_epi32(curr, _mm_alignr_epi8(curr, prev, 8));
    _mm_storeu_si128((__m128i *)output + i, delta);
    prev = curr;
  }
  int64_t lastprev = _mm_extract_epi64(prev, 1);
  for (i = 2 * i; i < length; ++i) {
    int64_t curr = input[i];
    output[i] = curr - lastprev;
    lastprev = curr;
  }
}

// write to buffer the successive differences of buffer
// (buffer[0]-starting_point, buffer[1]-buffer[2], ...) there are "length"
// values in buffer
void compute_deltas_inplace(uint32_t *buffer, size_t length,
                            uint32_t starting_point) {
  __m128i prev = _mm_set1_epi32(starting_point);
  size_t i = 0;
  for (; i < length / 4; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)buffer + i);
    __m128i delta = _mm_sub_epi32(curr, _mm_alignr_epi8(curr, prev, 12));
    _mm_storeu_si128((__m128i *)buffer + i, delta);
    prev = curr;
  }
  uint32_t lastprev = _mm_extract_epi32(prev, 3);
  for (i = 4 * i; i < length; ++i) {
    uint32_t curr = buffer[i];
    buffer[i] = curr - lastprev;
    lastprev = curr;
  }
}

// write to buffer the successive differences of buffer
// (buffer[0]-starting_point, buffer[1]-buffer[2], ...) there are "length"
// values in buffer
void compute_deltas64_inplace(int64_t *buffer, size_t length,
                              int64_t starting_point) {
  __m128i prev = _mm_set1_epi64x(starting_point);
  size_t i = 0;
  for (; i < length / 2; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)buffer + i);
    __m128i delta = _mm_sub_epi32(curr, _mm_alignr_epi8(curr, prev, 8));
    _mm_storeu_si128((__m128i *)buffer + i, delta);
    prev = curr;
  }
  int64_t lastprev = _mm_extract_epi64(prev, 1);
  for (i = 2 * i; i < length; ++i) {
    int64_t curr = buffer[i];
    buffer[i] = curr - lastprev;
    lastprev = curr;
  }
}

// write to output the successive differences of input (input[0]-starting_point,
// input[1]-input[2], ...) there are "length" values in input and output input
// and output must be distinct
void compute_prefix_sum(const uint32_t *__restrict__ input, size_t length,
                        uint32_t *__restrict__ output,
                        uint32_t starting_point) {
  __m128i prev = _mm_set1_epi32(starting_point);
  size_t i = 0;
  for (; i < length / 4; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)input + i);
    const __m128i _tmp1 = _mm_add_epi32(_mm_slli_si128(curr, 8), curr);
    const __m128i _tmp2 = _mm_add_epi32(_mm_slli_si128(_tmp1, 4), _tmp1);
    prev = _mm_add_epi32(_tmp2, _mm_shuffle_epi32(prev, 0xff));
    _mm_storeu_si128((__m128i *)output + i, prev);
  }
  uint32_t lastprev = _mm_extract_epi32(prev, 3);
  for (i = 4 * i; i < length; ++i) {
    lastprev = lastprev + input[i];
    output[i] = lastprev;
  }
}

// write to output the successive differences of input (input[0]-starting_point,
// input[1]-input[2], ...) there are "length" values in input and output input
// and output must be distinct
void compute_prefix_sum64(const int64_t *__restrict__ input, size_t length,
                          int64_t *__restrict__ output,
                          int64_t starting_point) {
  __m128i prev = _mm_set1_epi64x(starting_point);
  size_t i = 0;
  for (; i < length / 2; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)input + i);
    const __m128i _tmp1 = _mm_add_epi64(_mm_slli_si128(curr, 16), curr);
    const __m128i _tmp2 = _mm_add_epi64(_mm_slli_si128(_tmp1, 8), _tmp1);
    prev = _mm_add_epi64(_tmp2, _mm_shuffle_epi32(prev, 0xff));
    _mm_storeu_si128((__m128i *)output + i, prev);
  }
  int64_t lastprev = _mm_extract_epi64(prev, 1);
  for (i = 2 * i; i < length; ++i) {
    lastprev = lastprev + input[i];
    output[i] = lastprev;
  }
}

// write to buffer the successive differences of buffer
// (buffer[0]-starting_point, buffer[1]-buffer[2], ...) there are "length"
// values in buffer
void compute_prefix_sum_inplace(uint32_t *buffer, size_t length,
                                uint32_t starting_point) {
  __m128i prev = _mm_set1_epi32(starting_point);
  size_t i = 0;
  for (; i < length / 4; i++) {
    __m128i curr = _mm_lddqu_si128((const __m128i *)buffer + i);
    const __m128i _tmp1 = _mm_add_epi32(_mm_slli_si128(curr, 8), curr);
    const __m128i _tmp2 = _mm_add_epi32(_mm_slli_si128(_tmp1, 4), _tmp1);
    prev = _mm_add_epi32(_tmp2, _mm_shuffle_epi32(prev, 0xff));
    _mm_storeu_si128((__m128i *)buffer + i, prev);
  }
  uint32_t lastprev = _mm_extract_epi32(prev, 3);
  for (i = 4 * i; i < length; ++i) {
    lastprev = lastprev + buffer[i];
    buffer[i] = lastprev;
  }
}

/**
 * Encode in delta of delta format
 * @param in input array
 * @param n number of elements
 * @param out pointer to output buffer
 * @return number of bytes used for enconding in output array
 */
size_t _ddelta_enc64(const int64_t *__restrict__ in, size_t n,
                     int64_t *__restrict__ out) {
  if (!n)
    return 0;
  out[0] = in[0];
  // compute deltas
  compute_deltas64(in, n, out, 0);
  // compute deltas of deltas
  compute_deltas64_inplace(out + 1, n - 1, 0);
  return n * sizeof(uint64_t);
}

/**
 * Decode from delta of delta format
 * @param in pointer to input buffer
 * @param n number of elements
 * @param out output array
 * @return tbd
 */
size_t _ddelta_dec64(const int64_t *__restrict__ in, size_t n,
                     int64_t *__restrict__ out) {
  if (!n)
    return 0;
  out[0] = in[0];
  out[1] = in[1];
  size_t i = 1;
  for (i = 2; i < n; i++) {
    out[i] = in[i] + out[i - 1];
  }
  for (i = 1; i < n; i++) {
    out[i] = out[i] + out[i - 1];
  }
  return 0;
}
