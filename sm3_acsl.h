/*
 * ACSL annotations for the SM3 implementation for Frama-C analysis.
 */

#include "sm3/include/crypt_sm3.h"
#include "sm3/src/sm3_local.h" // For CRYPT_SM3_Ctx definition

/*@
  // Predicate to describe a valid, initialized SM3 context.
  predicate valid_sm3_ctx(CRYPT_SM3_Ctx* ctx) =
    \valid(ctx) &&
    ctx->num < CRYPT_SM3_BLOCKSIZE;

  // Predicate for a freshly initialized context.
  predicate initialized_sm3_ctx(CRYPT_SM3_Ctx* ctx) =
    valid_sm3_ctx(ctx) &&
    ctx->num == 0 &&
    ctx->lNum == 0 &&
    ctx->hNum == 0 &&
    ctx->h[0] == 0x7380166F &&
    ctx->h[1] == 0x4914B2B9 &&
    ctx->h[2] == 0x172442D7 &&
    ctx->h[3] == 0xDA8A0600 &&
    ctx->h[4] == 0xA96F30BC &&
    ctx->h[5] == 0x163138AA &&
    ctx->h[6] == 0xE38DEE4D &&
    ctx->h[7] == 0xB0FB0E4E;
*/

/*@
  // Contract for CRYPT_SM3_Init
  requires \valid(ctx);
  assigns *ctx;
  ensures initialized_sm3_ctx(ctx);
  ensures \result == 0; // CRYPT_SUCCESS
*/
int32_t CRYPT_SM3_Init(CRYPT_SM3_Ctx *ctx, BSL_Param *param);

/*@
  // Contract for CRYPT_SM3_Update
  requires valid_sm3_ctx(ctx);
  requires len == 0 || \valid_read(in + (0 .. len-1));
  requires \separated(ctx, in + (0 .. len-1));

  // The maximum length check is complex, we focus on memory safety here.
  // A full spec would require formalizing IsInputOverflow.
  requires (unsigned long long)ctx->hNum * 0x100000000 + ctx->lNum + (unsigned long long)len * 8 < 0xFFFFFFFFFFFFFFFF;

  assigns *ctx;

  ensures valid_sm3_ctx(ctx);
  ensures \result == 0; // CRYPT_SUCCESS
*/
int32_t CRYPT_SM3_Update(CRYPT_SM3_Ctx *ctx, const uint8_t *in, uint32_t len);

/*@
  // Contract for CRYPT_SM3_Final
  requires valid_sm3_ctx(ctx);
  requires \valid(outLen);
  requires *outLen >= CRYPT_SM3_DIGESTSIZE;
  requires \valid(out + (0 .. CRYPT_SM3_DIGESTSIZE-1));
  requires \separated(ctx, out + (0 .. CRYPT_SM3_DIGESTSIZE-1), outLen);

  assigns *ctx;
  assigns out[0 .. CRYPT_SM3_DIGESTSIZE-1];
  assigns *outLen;

  ensures \result == 0; // CRYPT_SUCCESS
  ensures *outLen == CRYPT_SM3_DIGESTSIZE;
*/
int32_t CRYPT_SM3_Final(CRYPT_SM3_Ctx *ctx, uint8_t *out, uint32_t *outLen);

// Forward declaration for the internal compress function
void SM3_Compress(uint32_t state[8], const uint8_t *data, uint32_t blockCnt);

/*@
  // Contract for the internal SM3_Compress function
  requires \valid(state + (0..7));
  requires blockCnt > 0 ==> \valid_read(data + (0 .. blockCnt * 64 - 1));
  requires \separated(state + (0..7), data + (0 .. blockCnt * 64 - 1));

  assigns state[0..7];

  behavior zero_blocks:
    assumes blockCnt == 0;
    assigns \nothing;
    ensures \forall integer i; 0 <= i < 8 ==> state[i] == \old(state[i]);

  behavior compress_blocks:
    assumes blockCnt > 0;
    assigns state[0..7];
    // A full post-condition would require specifying the exact output of the
    // compression function, which is very complex. For now, we focus on safety.
    ensures \true;

  complete behaviors;
  disjoint behaviors;
*/
void SM3_Compress(uint32_t state[8], const uint8_t *data, uint32_t blockCnt);
