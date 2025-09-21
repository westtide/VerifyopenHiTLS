#pragma once
#include <stdint.h>
#include "crypt_sm4.h"

/*@
	// 8位和32位无符号整数的逻辑定义
    logic integer U8(integer x)   = x & 0xFF;
    logic integer U32(integer x)  = x & 0xFFFFFFFF;
	// 逻辑谓词：判断两个字节数组在前n个字节上是否相等
    predicate bytes_eq{L}(const uint8_t *a, const uint8_t *b, integer n)
        reads a[0..n-1], b[0..n-1];
        = \forall integer i; 0 <= i < n ==> a[i] == b[i];
	// 逻辑谓词：判断两个字节数组在前n个字节上是否不重叠
    predicate disjoint_ranges(const uint8_t *p, integer n,
                                const uint8_t *q, integer m) =
        \separated(p+(0..n-1), q+(0..m-1));

	// 32位循环左移
    logic integer rol32(integer x, integer n) =
        U32( (x << (n % 32)) | ((x >> (32 - (n % 32))) & ((1 << (32 - (n % 32))) - 1)) );
	// 32位循环左移结果范围
    lemma rol32_range : \forall integer x, n; 0 <= n < 32 == > 0 <= rol32(x, n) <= 0xFFFFFFFF;

	// 从字节数组中按大端序读取32位整数
    logic integer be32{L}(const uint8_t *p)
        reads p[0..3];
        = ((integer)p[0] << 24) + ((integer)p[1] << 16) + ((integer)p[2] << 8) + (integer)p[3];
	// 从字节数组中按小端序读取32位整数
    logic integer le32{L}(const uint8_t *p)
        reads p[0..3];
        = ((integer)p[3] << 24) + ((integer)p[2] << 16) + ((integer)p[1] << 8) + (integer)p[0];
	// 32位整数按大端序写入字节数组
    logic integer xor8{L}(const uint8_t *a, const uint8_t *b, integer j)
        reads a[j], b[j];
        = U8((integer)a[j]) ^ U8((integer)b[j]);
	// 32位整数按小端序写入字节数组
    predicate block_xor16{L}(const uint8_t *a, const uint8_t *b, const uint8_t *out) =
        \valid_read(a + (0..15)) && \valid_read(b + (0..15)) && \valid(out + (0..15)) &&
        (\forall integer j; 0 <= j<16 ==>(integer) out[j] == ((integer)a[j] ^ (integer)b[j]));

	// 判断字节数组的前n个字节是否都是合法的8位无符号整数
    predicate all_bytes_u8{L}(const uint8_t *p, integer n)
        reads p[0..n - 1];
        = \forall integer i; 0 <= i < n == > 0 <= (integer)p[i] <= 255;
    // 判断32位整数数组的前n个元素是否都是合法的32位无符号整数
    predicate rk32_ok{L}(const uint32_t *rk) =
        \valid_read(rk+(0..CRYPT_SM4_ROUNDS-1)) &&
        (\forall integer i; 0 <= i < CRYPT_SM4_ROUNDS ==> 0 <= (integer)rk[i] <= 0xFFFFFFFF);

	// SM4 加解密单块函数的逻辑定义
    logic integer SM4_E_byte{L}(const uint32_t *rk,
                                    const uint8_t *in, integer j)
        reads rk[0..CRYPT_SM4_ROUNDS-1], in[0..15];
	// SM4_E_byte 定义了使用轮密钥 rk 对输入块 in 进行加密后，第 j 个字节的值
    logic integer SM4_D_byte{L}(const uint32_t *rk,
                                    const uint8_t *in, integer j)
        reads rk[0..CRYPT_SM4_ROUNDS-1], in[0..15];

	// axiams 定义 SM4_E_byte 和 SM4_D_byte 的取值范围
    axiom sm4_e_byte_range:
        \forall const uint32_t *rk, const uint8_t *in, integer j; 0 <= j < 16 ==> 0 <= SM4_E_byte(rk,in,j) <= 255;
    axiom sm4_d_byte_range:
        \forall const uint32_t *rk, const uint8_t *in, integer j; 0 <= j < 16 ==> 0 <= SM4_D_byte(rk,in,j) <= 255;

	// 单块加密的正确性谓词
    predicate block_enc_by_rk{L}(const uint32_t *rk,
                                    const uint8_t *in, const uint8_t *out) =
        rk32_ok(rk) && \valid_read(in+(0..15)) && \valid(out+(0..15)) &&
        (\forall integer j; 0 <= j < 16 ==> (integer)out[j] == SM4_E_byte(rk,in,j));
	// 单块解密的正确性谓词
    predicate block_dec_by_rk{L}(const uint32_t *rk,
                                    const uint8_t *in, const uint8_t *out) =
        rk32_ok(rk) && \valid_read(in+(0..15)) && \valid(out+(0..15)) &&
        (\forall integer j; 0 <= j < 16 ==> (integer)out[j] == SM4_D_byte(rk,in,j));
*/

/*@
// ECB 逻辑
	// ECB的正确性谓词
	predicate ecb_correct{L}(const uint32_t *rk, const uint8_t *in, const uint8_t *out, integer len) =
      rk32_ok(rk) && len % 16 == 0 &&
      \valid_read(in+(0..len-1)) && \valid(out+(0..len-1)) &&
      (\forall integer b; 0 <= b < len/16 ==>
        (\forall integer j; 0 <= j < 16 ==>
          (integer)out[16*b + j] == SM4_E_byte(rk, in + 16*b, j)));

	// SM4_E_xor_byte 定义了对输入块 in 与块 b 进行异或后再加密，第 j 个字节的值
	logic integer SM4_E_xor_byte{L}(const uint32_t *rk, const uint8_t *a, const uint8_t *b, integer j)
      	reads rk[0..CRYPT_SM4_ROUNDS-1], a[0..15], b[0..15];
	// 定义 SM4_E_xor_byte 的取值范围
	axiom sm4_e_xor_range:
      \forall const uint32_t *rk, const uint8_t *a, const uint8_t *b, integer j; 0 <= j < 16 ==> 0 <= SM4_E_xor_byte(rk,a,b,j) <= 255;

// CBC 逻辑
    // CBC的正确性谓词
	predicate cbc_enc_correct{L}(const uint32_t *rk, const uint8_t *in, const uint8_t *out, const uint8_t *iv0, integer len) =
      rk32_ok(rk) && len % 16 == 0 &&
      \valid_read(in+(0..len-1)) && \valid(out+(0..len-1)) &&
      \valid_read(iv0+(0..15)) &&
      (\forall integer b; 0 <= b < len/16 ==>
         (\forall integer j; 0 <= j < 16 ==>
           (integer)out[16*b + j] ==
             SM4_E_xor_byte(rk, in + 16*b, (b==0 ? iv0 : out + 16*(b-1)), j)));

*/


/*@
// CTR 逻辑 
logic integer add_with_carry(integer a, integer b, integer c) = (a + b + c) % 256; 
logic integer carry_out(integer a, integer b, integer c) = ((a + b + c) / 256); 

 logic integer ctr_be_byte_plus(const uint8_t *ctr0, integer n, integer j)
      reads ctr0[0..15];
      // 把 ctr0 视为 128-bit 大端整数，返回加上 n 后第 j 个字节（0..15）
      = (\let integer b0 = (integer)ctr0[15];
         \let integer b1 = (integer)ctr0[14];
         \let integer b2 = (integer)ctr0[13];
         \let integer b3 = (integer)ctr0[12];
         \let integer b4 = (integer)ctr0[11];
         \let integer b5 = (integer)ctr0[10];
         \let integer b6 = (integer)ctr0[9];
         \let integer b7 = (integer)ctr0[8];
         \let integer b8 = (integer)ctr0[7];
         \let integer b9 = (integer)ctr0[6];
         \let integer b10 = (integer)ctr0[5];
         \let integer b11 = (integer)ctr0[4];
         \let integer b12 = (integer)ctr0[3];
         \let integer b13 = (integer)ctr0[2];
         \let integer b14 = (integer)ctr0[1];
         \let integer b15 = (integer)ctr0[0];
         \let integer c0 = add_with_carry(b0, n % 256, 0);
         \let integer k0 = carry_out(b0, n % 256, 0);
         \let integer c1 = add_with_carry(b1, (n/256) % 256, k0);
         \let integer k1 = carry_out(b1, (n/256) % 256, k0);
         \let integer c2 = add_with_carry(b2, (n/(256*256)) % 256, k1);
         \let integer k2 = carry_out(b2, (n/(256*256)) % 256, k1);
         \let integer c3 = add_with_carry(b3, (n/(256*256*256)) % 256, k2);
         \let integer k3 = carry_out(b3, (n/(256*256*256)) % 256, k2);
         \let integer c4 = add_with_carry(b4, 0, k3);
         \let integer k4 = carry_out(b4, 0, k3);
         \let integer c5 = add_with_carry(b5, 0, k4);
         \let integer k5 = carry_out(b5, 0, k4);
         \let integer c6 = add_with_carry(b6, 0, k5);
         \let integer k6 = carry_out(b6, 0, k5);
         \let integer c7 = add_with_carry(b7, 0, k6);
         \let integer k7 = carry_out(b7, 0, k6);
         \let integer c8 = add_with_carry(b8, 0, k7);
         \let integer k8 = carry_out(b8, 0, k7);
         \let integer c9 = add_with_carry(b9, 0, k8);
         \let integer k9 = carry_out(b9, 0, k8);
         \let integer c10 = add_with_carry(b10, 0, k9);
         \let integer k10 = carry_out(b10, 0, k9);
         \let integer c11 = add_with_carry(b11, 0, k10);
         \let integer k11 = carry_out(b11, 0, k10);
         \let integer c12 = add_with_carry(b12, 0, k11);
         \let integer k12 = carry_out(b12, 0, k11);
         \let integer c13 = add_with_carry(b13, 0, k12);
         \let integer k13 = carry_out(b13, 0, k12);
         \let integer c14 = add_with_carry(b14, 0, k13);
         \let integer k14 = carry_out(b14, 0, k13);
         \let integer c15 = add_with_carry(b15, 0, k14);
         (j==0  ? c15 :
         (j==1  ? c14 :
         (j==2  ? c13 :
         (j==3  ? c12 :
         (j==4  ? c11 :
         (j==5  ? c10 :
         (j==6  ? c9  :
         (j==7  ? c8  :
         (j==8  ? c7  :
         (j==9  ? c6  :
         (j==10 ? c5  :
         (j==11 ? c4  :
         (j==12 ? c3  :
         (j==13 ? c2  :
         (j==14 ? c1  : c0
         ))))))))))))))));


 logic integer SM4_E_ctr_byte{L}(const uint32_t *rk,
                                     const uint8_t *ctr0, integer blk, integer j)
    reads rk[0..CRYPT_SM4_ROUNDS-1], ctr0[0..15];
 axiom sm4_e_ctr_range:
      \forall const uint32_t *rk, const uint8_t *ctr0, integer blk, integer j;
        0 <= j < 16 ==> 0 <= SM4_E_ctr_byte(rk,ctr0,blk,j) <= 255;

predicate ctr_correct{L}(const uint32_t *rk,
                             const uint8_t *in, const uint8_t *out,
                             const uint8_t *ctr0, integer len) =
      rk32_ok(rk) &&
      \valid_read(in+(0..len-1)) && \valid(out+(0..len-1)) &&
      \valid_read(ctr0+(0..15)) &&
      (\forall integer t; 0 <= t < len ==>
         (integer)out[t] == (((integer)in[t]) ^ SM4_E_ctr_byte(rk, ctr0, t/16, t%16)));
*/


/*@
// cfb 逻辑
logic integer SM4_E_stream_byte{L}(const uint32_t *rk,
                                       const uint8_t *iv0, integer blk, integer j)
      reads rk[0..CRYPT_SM4_ROUNDS-1], iv0[0..15];

axiom sm4_e_stream_range:
      \forall const uint32_t *rk, const uint8_t *iv0, integer blk, integer j;
        0 <= j < 16 ==> 0 <= SM4_E_stream_byte(rk,iv0,blk,j) <= 255;

predicate cfb_correct_block_aligned{L}(const uint32_t *rk,
                                           const uint8_t *in, const uint8_t *out,
                                           const uint8_t *iv0, integer len) =
      rk32_ok(rk) && len % 16 == 0 &&
      \valid_read(in+(0..len-1)) && \valid(out+(0..len-1)) &&
      \valid_read(iv0+(0..15)) &&
      (\forall integer b; 0 <= b < len/16 ==>
         (\forall integer j; 0 <= j < 16 ==>
            (integer)out[16*b+j] ==
              (((integer)in[16*b+j]) ^ SM4_E_stream_byte(rk, (b==0?iv0:out+16*(b-1)), 0, j))));


/*@
  ============================================================
  公共 API 级契约（与 crypt_sm4.h 中原型对应）
  ============================================================
*/

/*@ requires \valid(ctx);
    requires \valid_read(key+(0..CRYPT_SM4_BLOCKSIZE-1));
    requires keyLen == CRYPT_SM4_BLOCKSIZE;
    assigns ctx->rk[0..CRYPT_SM4_ROUNDS-1], ctx->iv[0..CRYPT_SM4_BLOCKSIZE-1];
    ensures rk32_ok(ctx->rk);
*/
int32_t CRYPT_SM4_SetKey(CRYPT_SM4_Ctx *ctx, const uint8_t *key, uint32_t keyLen);

/*@ requires \valid(ctx) && rk32_ok(ctx->rk);
    requires \valid_read(in+(0..length-1));
    requires \valid(out+(0..length-1));
    requires length % CRYPT_SM4_BLOCKSIZE == 0;
    requires disjoint_ranges(in, length, out, length);
    assigns out[0..length-1];
    ensures ecb_correct(ctx->rk, in, out, length);
*/
int32_t CRYPT_SM4_Encrypt(CRYPT_SM4_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

/*@ requires \valid(ctx) && rk32_ok(ctx->rk);
    requires \valid_read(in+(0..length-1));
    requires \valid(out+(0..length-1));
    requires length % CRYPT_SM4_BLOCKSIZE == 0;
    requires disjoint_ranges(in, length, out, length);
    assigns out[0..length-1];
*/
int32_t CRYPT_SM4_Decrypt(CRYPT_SM4_Ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t length);

/*@ requires \valid(ctx);
    assigns *ctx;
*/
void CRYPT_SM4_Clean(CRYPT_SM4_Ctx *ctx);
