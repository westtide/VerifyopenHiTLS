/* ivette -wp -rte \
  -cpp-extra-args="-DHITLS_CRYPTO_SM3 -I ./include/bsl -I ./include/crypto -I ./bsl/err/include  -I ./crypto/include  -I ./config/macro_config -I ./config -I ./include -I ./sm3/include" \
  sm3/src/noasm_sm3_count0.c
 * This file is part of the openHiTLS project.
 *
 * openHiTLS is licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *     http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include "hitls_build.h"
#ifdef HITLS_CRYPTO_SM3

#include <stdint.h>
#include "crypt_utils.h"
#include "bsl_sal.h"
#include "crypt_sm3.h"

/*
  宏定义的常量属于C语言的预处理器定义，Frama-C默认会识别这些常量，
  因此不需要在ACSL中重复定义。
*/
#define K0 0x79cc4519U
#define K1 0xf3988a32U
#define K2 0xe7311465U
#define K3 0xce6228cbU
#define K4 0x9cc45197U
#define K5 0x3988a32fU
#define K6 0x7311465eU
#define K7 0xe6228cbcU
#define K8 0xcc451979U
#define K9 0x988a32f3U
#define K10 0x311465e7U
#define K11 0x6228cbceU
#define K12 0xc451979cU
#define K13 0x88a32f39U
#define K14 0x11465e73U
#define K15 0x228cbce6U
#define K16 0x9d8a7a87U
#define K17 0x3b14f50fU
#define K18 0x7629ea1eU
#define K19 0xec53d43cU
#define K20 0xd8a7a879U
#define K21 0xb14f50f3U
#define K22 0x629ea1e7U
#define K23 0xc53d43ceU
#define K24 0x8a7a879dU
#define K25 0x14f50f3bU
#define K26 0x29ea1e76U
#define K27 0x53d43cecU
#define K28 0xa7a879d8U
#define K29 0x4f50f3b1U
#define K30 0x9ea1e762U
#define K31 0x3d43cec5U
#define K32 0x7a879d8aU
#define K33 0xf50f3b14U
#define K34 0xea1e7629U
#define K35 0xd43cec53U
#define K36 0xa879d8a7U
#define K37 0x50f3b14fU
#define K38 0xa1e7629eU
#define K39 0x43cec53dU
#define K40 0x879d8a7aU
#define K41 0x0f3b14f5U
#define K42 0x1e7629eaU
#define K43 0x3cec53d4U
#define K44 0x79d8a7a8U
#define K45 0xf3b14f50U
#define K46 0xe7629ea1U
#define K47 0xcec53d43U
#define K48 0x9d8a7a87U
#define K49 0x3b14f50fU
#define K50 0x7629ea1eU
#define K51 0xec53d43cU
#define K52 0xd8a7a879U
#define K53 0xb14f50f3U
#define K54 0x629ea1e7U
#define K55 0xc53d43ceU
#define K56 0x8a7a879dU
#define K57 0x14f50f3bU
#define K58 0x29ea1e76U
#define K59 0x53d43cecU
#define K60 0xa7a879d8U
#define K61 0x4f50f3b1U
#define K62 0x9ea1e762U
#define K63 0x3d43cec5U

/*@
  // 用logic定义ROTL32（32位循环左移），确保在ACSL中可以使用
  logic integer ROTL32_Logic(integer x, integer n) =
    ((x << n) | ((x >> (32 - n)) & ((1 << n) - 1))) & 0xFFFFFFFF;

  // P0由ROTL32定义
  logic integer P0_Logic(integer x) =
    (x ^ ROTL32_Logic(x, 9) ^ ROTL32_Logic(x, 17)) & 0xFFFFFFFF;

    // P1由ROTL32定义
  logic integer P1_Logic(integer x) =
    (x ^ ROTL32_Logic(x, 15) ^ ROTL32_Logic(x, 23)) & 0xFFFFFFFF;
*/
#define P0(x) ((x) ^ ROTL32((x), 9) ^ ROTL32((x), 17))
#define P1(x) ((x) ^ ROTL32((x), 15) ^ ROTL32((x), 23))

/*@
  // 以下是SM3中的四个布尔函数FF和GG
  // FF函数拆分为FF0和FF1
  logic integer FF0_Logic(integer x, integer y, integer z) =
    (x ^ y ^ z) & 0xFFFFFFFF;
  logic integer FF1_Logic(integer x, integer y, integer z) =
    ((x & y) | (x & z) | (y & z)) & 0xFFFFFFFF;

  // GG函数拆分为GG0和GG1
  logic integer GG0_Logic(integer x, integer y, integer z) =
    (x ^ y ^ z) & 0xFFFFFFFF;

  logic integer GG1_Logic(integer x, integer y, integer z) =
    ((x & y) | ((~x) & z)) & 0xFFFFFFFF;
*/
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | (~(x) & (z)))

/*
  SM3中的单轮运算步骤，因为while(0)只执行一次
  其中H..F使用()括起来以避免宏展开时的优先级问题
  但是FF、GG是传入时决定FF0、FF1、GG0、GG1的
  因此ROUND不写logic，需要单独写ROUND00_15和ROUND16_63
*/
#define ROUND(A, B, C, D, E, F, G, H, K, FF, GG, Wj, Wi) \
  do                                                     \
  {                                                      \
    uint32_t a12 = ROTL32((A), 12);                      \
    uint32_t ss1 = ROTL32(a12 + (E) + (K), 7);           \
    uint32_t ss2 = ss1 ^ a12;                            \
    uint32_t tt1 = FF((A), (B), (C)) + (D) + ss2 + (Wi); \
    uint32_t tt2 = GG((E), (F), (G)) + (H) + ss1 + (Wj); \
    (H) = tt1;                                           \
    (D) = P0(tt2);                                       \
    (B) = ROTL32((B), 9);                                \
    (F) = ROTL32((F), 19);                               \
  } while (0)

/*@
  logic integer EXPAND_Logic(integer W1, integer W2, integer W3,
                       integer W4, integer W5) =
    (P1_Logic(W1 ^ W2 ^ ROTL32_Logic(W3, 15)) ^ ROTL32_Logic(W4, 7) ^ W5) & 0xFFFFFFFF;
*/
#define EXPAND(W1, W2, W3, W4, W5) \
  (P1((W1) ^ (W2) ^ ROTL32((W3), 15)) ^ ROTL32((W4), 7) ^ (W5))

/*@
  requires \valid(state + (0..7));
  // blockCnt == 0 implies data is not read, so no need for \valid_read on data for this case
  requires blockCnt > 0 ==> \valid_read(data + (0 .. blockCnt * 64 - 1));
  requires \separated(state + (0..7), data + (0 .. blockCnt * 64 - 1));

  assigns state[0..7]; // State is modified in general, even if not for blockCnt == 0

  ensures blockCnt == 0 ==>
    (\forall integer i; 0 <= i <= 7 ==> state[i] == \old(state[i]));
  // If blockCnt is 0, 'count' (local var) will be 0, not relevant as a post-condition for the state
*/
void SM3_Compress(uint32_t state[8], const uint8_t *data, uint32_t blockCnt)
{
  uint32_t w[16] = {0};
  const uint8_t *input = data;
  uint32_t count = blockCnt;

  //@ assert (\forall integer j; 0 <= j < 16 ==> w[j] == 0);
  // The 'count == 0 => ignore while()' comment refers to the expected behavior,
  // which is captured by the ensures clause for blockCnt == 0.
}
#endif // HITLS_CRYPTO_SM3
