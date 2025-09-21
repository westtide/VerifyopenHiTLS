/*
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
#ifdef HITLS_CRYPTO_XMSS

#include "securec.h"
#include "bsl_sal.h"
#include "crypt_errno.h"
#include "crypt_algid.h"
#include "crypt_util_rand.h"
#include "eal_md_local.h"
#include "crypt_xmss.h"
#include "slh_dsa_local.h"
#include "slh_dsa_xmss.h"
#include "slh_dsa_hypertree.h"
#include "xmss_hash.h"

#define XMSS_ADRS_LEN SLH_DSA_ADRS_LEN

typedef SlhDsaPara XmssPara;

typedef struct {
    BSL_Param *pubSeed;
    BSL_Param *pubRoot;
} XmssPubKeyParam;

typedef struct {
    BSL_Param *prvIndex;
    BSL_Param *prvSeed;
    BSL_Param *prvPrf;
    BSL_Param *pubSeed;
    BSL_Param *pubRoot;
} XmssPrvKeyParam;

/* params isCompressed, a, k, m, secCategory are not used for xmss */
static const XmssPara XmssParaTable[] = {
    /* { algId, isCompressed, n, h, d, hp, a, k, m, secCategory, pkBytes, sigBytes }, */
    
    { CRYPT_XMSS_SHA2_10_256, 0, 32, 10, 1, 10, 0, 0, 0, 0,  68, 2500 },
    { CRYPT_XMSS_SHA2_16_256, 0, 32, 16, 1, 16, 0, 0, 0, 0,  68, 2692 },
    { CRYPT_XMSS_SHA2_20_256, 0, 32, 20, 1, 20, 0, 0, 0, 0,  68, 2820 },
    { CRYPT_XMSS_SHA2_10_512, 0, 64, 10, 1, 10, 0, 0, 0, 0, 132, 9092 },
    { CRYPT_XMSS_SHA2_16_512, 0, 64, 16, 1, 16, 0, 0, 0, 0, 132, 9476 },
    { CRYPT_XMSS_SHA2_20_512, 0, 64, 20, 1, 20, 0, 0, 0, 0, 132, 9732 },

    { CRYPT_XMSS_SHAKE_10_256, 0, 32, 10, 1, 10, 0, 0, 0, 0,  68, 2500 },
    { CRYPT_XMSS_SHAKE_16_256, 0, 32, 16, 1, 16, 0, 0, 0, 0,  68, 2692 },
    { CRYPT_XMSS_SHAKE_20_256, 0, 32, 20, 1, 20, 0, 0, 0, 0,  68, 2820 },
    { CRYPT_XMSS_SHAKE_10_512, 0, 64, 10, 1, 10, 0, 0, 0, 0, 132, 9092 },
    { CRYPT_XMSS_SHAKE_16_512, 0, 64, 16, 1, 16, 0, 0, 0, 0, 132, 9476 },
    { CRYPT_XMSS_SHAKE_20_512, 0, 64, 20, 1, 20, 0, 0, 0, 0, 132, 9732 },

    { CRYPT_XMSS_SHA2_10_192, 0, 24, 10, 1, 10, 0, 0, 0, 0, 52, 1492 },
    { CRYPT_XMSS_SHA2_16_192, 0, 24, 16, 1, 16, 0, 0, 0, 0, 52, 1636 },
    { CRYPT_XMSS_SHA2_20_192, 0, 24, 20, 1, 20, 0, 0, 0, 0, 52, 1732 },
 
    { CRYPT_XMSS_SHAKE256_10_256, 0, 32, 10, 1, 10, 0, 0, 0, 0, 68, 2500 },
    { CRYPT_XMSS_SHAKE256_16_256, 0, 32, 16, 1, 16, 0, 0, 0, 0, 68, 2692 },
    { CRYPT_XMSS_SHAKE256_20_256, 0, 32, 20, 1, 20, 0, 0, 0, 0, 68, 2820 },
 
    { CRYPT_XMSS_SHAKE256_10_192, 0, 24, 10, 1, 10, 0, 0, 0, 0, 52, 1492 },
    { CRYPT_XMSS_SHAKE256_16_192, 0, 24, 16, 1, 16, 0, 0, 0, 0, 52, 1636 },
    { CRYPT_XMSS_SHAKE256_20_192, 0, 24, 20, 1, 20, 0, 0, 0, 0, 52, 1732 },

    { CRYPT_XMSSMT_SHA2_20_2_256,  0, 32, 20,  2, 10, 0, 0, 0, 0,  68,   4963 },
    { CRYPT_XMSSMT_SHA2_20_4_256,  0, 32, 20,  4,  5, 0, 0, 0, 0,  68,   9251 },
    { CRYPT_XMSSMT_SHA2_40_2_256,  0, 32, 40,  2, 20, 0, 0, 0, 0,  68,   5605 },
    { CRYPT_XMSSMT_SHA2_40_4_256,  0, 32, 40,  4, 10, 0, 0, 0, 0,  68,   9893 },
    { CRYPT_XMSSMT_SHA2_40_8_256,  0, 32, 40,  8,  5, 0, 0, 0, 0,  68,  18469 },
    { CRYPT_XMSSMT_SHA2_60_3_256,  0, 32, 60,  3, 20, 0, 0, 0, 0,  68,   8392 },
    { CRYPT_XMSSMT_SHA2_60_6_256,  0, 32, 60,  6, 10, 0, 0, 0, 0,  68,  14824 },
    { CRYPT_XMSSMT_SHA2_60_12_256, 0, 32, 60, 12,  5, 0, 0, 0, 0,  68,  27688 },
    { CRYPT_XMSSMT_SHA2_20_2_512,  0, 64, 20,  2, 10, 0, 0, 0, 0, 132,  18115 },
    { CRYPT_XMSSMT_SHA2_20_4_512,  0, 64, 20,  4,  5, 0, 0, 0, 0, 132,  34883 },
    { CRYPT_XMSSMT_SHA2_40_2_512,  0, 64, 40,  2, 20, 0, 0, 0, 0, 132,  19397 },
    { CRYPT_XMSSMT_SHA2_40_4_512,  0, 64, 40,  4, 10, 0, 0, 0, 0, 132,  36165 },
    { CRYPT_XMSSMT_SHA2_40_8_512,  0, 64, 40,  8,  5, 0, 0, 0, 0, 132,  69701 },
    { CRYPT_XMSSMT_SHA2_60_3_512,  0, 64, 60,  3, 20, 0, 0, 0, 0, 132,  29064 },
    { CRYPT_XMSSMT_SHA2_60_6_512,  0, 64, 60,  6, 10, 0, 0, 0, 0, 132,  54216 },
    { CRYPT_XMSSMT_SHA2_60_12_512, 0, 64, 60, 12,  5, 0, 0, 0, 0, 132, 104520 },

    { CRYPT_XMSSMT_SHAKE_20_2_256,  0, 32, 20,  2, 10, 0, 0, 0, 0,  68,   4963 },
    { CRYPT_XMSSMT_SHAKE_20_4_256,  0, 32, 20,  4,  5, 0, 0, 0, 0,  68,   9251 },
    { CRYPT_XMSSMT_SHAKE_40_2_256,  0, 32, 40,  2, 20, 0, 0, 0, 0,  68,   5605 },
    { CRYPT_XMSSMT_SHAKE_40_4_256,  0, 32, 40,  4, 10, 0, 0, 0, 0,  68,   9893 },
    { CRYPT_XMSSMT_SHAKE_40_8_256,  0, 32, 40,  8,  5, 0, 0, 0, 0,  68,  18469 },
    { CRYPT_XMSSMT_SHAKE_60_3_256,  0, 32, 60,  3, 20, 0, 0, 0, 0,  68,   8392 },
    { CRYPT_XMSSMT_SHAKE_60_6_256,  0, 32, 60,  6, 10, 0, 0, 0, 0,  68,  14824 },
    { CRYPT_XMSSMT_SHAKE_60_12_256, 0, 32, 60, 12,  5, 0, 0, 0, 0,  68,  27688 },
    { CRYPT_XMSSMT_SHAKE_20_2_512,  0, 64, 20,  2, 10, 0, 0, 0, 0, 132,  18115 },
    { CRYPT_XMSSMT_SHAKE_20_4_512,  0, 64, 20,  4,  5, 0, 0, 0, 0, 132,  34883 },
    { CRYPT_XMSSMT_SHAKE_40_2_512,  0, 64, 40,  2, 20, 0, 0, 0, 0, 132,  19397 },
    { CRYPT_XMSSMT_SHAKE_40_4_512,  0, 64, 40,  4, 10, 0, 0, 0, 0, 132,  36165 },
    { CRYPT_XMSSMT_SHAKE_40_8_512,  0, 64, 40,  8,  5, 0, 0, 0, 0, 132,  69701 },
    { CRYPT_XMSSMT_SHAKE_60_3_512,  0, 64, 60,  3, 20, 0, 0, 0, 0, 132,  29064 },
    { CRYPT_XMSSMT_SHAKE_60_6_512,  0, 64, 60,  6, 10, 0, 0, 0, 0, 132,  54216 },
    { CRYPT_XMSSMT_SHAKE_60_12_512, 0, 64, 60, 12,  5, 0, 0, 0, 0, 132, 104520 },

    { CRYPT_XMSSMT_SHA2_20_2_192,  0, 24, 20,  2, 10, 0, 0, 0, 0, 52,  2955 },
    { CRYPT_XMSSMT_SHA2_20_4_192,  0, 24, 20,  4,  5, 0, 0, 0, 0, 52,  5403 },
    { CRYPT_XMSSMT_SHA2_40_2_192,  0, 24, 40,  2, 20, 0, 0, 0, 0, 52,  3437 },
    { CRYPT_XMSSMT_SHA2_40_4_192,  0, 24, 40,  4, 10, 0, 0, 0, 0, 52,  5885 },
    { CRYPT_XMSSMT_SHA2_40_8_192,  0, 24, 40,  8,  5, 0, 0, 0, 0, 52, 10781 },
    { CRYPT_XMSSMT_SHA2_60_3_192,  0, 24, 60,  3, 20, 0, 0, 0, 0, 52,  5144 },
    { CRYPT_XMSSMT_SHA2_60_6_192,  0, 24, 60,  6, 10, 0, 0, 0, 0, 52,  8816 },
    { CRYPT_XMSSMT_SHA2_60_12_192, 0, 24, 60, 12,  5, 0, 0, 0, 0, 52, 16160 },

    { CRYPT_XMSSMT_SHAKE256_20_2_256,  0, 32, 20,  2, 10, 0, 0, 0, 0, 68,  4963 },
    { CRYPT_XMSSMT_SHAKE256_20_4_256,  0, 32, 20,  4,  5, 0, 0, 0, 0, 68,  9251 },
    { CRYPT_XMSSMT_SHAKE256_40_2_256,  0, 32, 40,  2, 20, 0, 0, 0, 0, 68,  5605 },
    { CRYPT_XMSSMT_SHAKE256_40_4_256,  0, 32, 40,  4, 10, 0, 0, 0, 0, 68,  9893 },
    { CRYPT_XMSSMT_SHAKE256_40_8_256,  0, 32, 40,  8,  5, 0, 0, 0, 0, 68, 18469 },
    { CRYPT_XMSSMT_SHAKE256_60_3_256,  0, 32, 60,  3, 20, 0, 0, 0, 0, 68,  8392 },
    { CRYPT_XMSSMT_SHAKE256_60_6_256,  0, 32, 60,  6, 10, 0, 0, 0, 0, 68, 14824 },
    { CRYPT_XMSSMT_SHAKE256_60_12_256, 0, 32, 60, 12,  5, 0, 0, 0, 0, 68, 27688 },

    { CRYPT_XMSSMT_SHAKE256_20_2_192,  0, 24, 20,  2, 10, 0, 0, 0, 0, 52,  2955 },
    { CRYPT_XMSSMT_SHAKE256_20_4_192,  0, 24, 20,  4,  5, 0, 0, 0, 0, 52,  5403 },
    { CRYPT_XMSSMT_SHAKE256_40_2_192,  0, 24, 40,  2, 20, 0, 0, 0, 0, 52,  3437 },
    { CRYPT_XMSSMT_SHAKE256_40_4_192,  0, 24, 40,  4, 10, 0, 0, 0, 0, 52,  5885 },
    { CRYPT_XMSSMT_SHAKE256_40_8_192,  0, 24, 40,  8,  5, 0, 0, 0, 0, 52, 10781 },
    { CRYPT_XMSSMT_SHAKE256_60_3_192,  0, 24, 60,  3, 20, 0, 0, 0, 0, 52,  5144 },
    { CRYPT_XMSSMT_SHAKE256_60_6_192,  0, 24, 60,  6, 10, 0, 0, 0, 0, 52,  8816 },
    { CRYPT_XMSSMT_SHAKE256_60_12_192, 0, 24, 60, 12,  5, 0, 0, 0, 0, 52, 16160 },
};

// “X” means xmss
static void XAdrsSetLayerAddr(XmssAdrs *adrs, uint32_t layer)
{
    PUT_UINT32_BE(layer, adrs->x.layerAddr, 0);
}

static void XAdrsSetTreeAddr(XmssAdrs *adrs, uint64_t tree)
{
    PUT_UINT64_BE(tree, adrs->x.treeAddr, 0);
}

static void XAdrsSetType(XmssAdrs *adrs, AdrsType type)
{
    PUT_UINT32_BE(type, adrs->x.type, 0);
    (void)memset_s(adrs->x.padding, sizeof(adrs->x.padding), 0, sizeof(adrs->x.padding));
}

static void XAdrsSetKeyPairAddr(XmssAdrs *adrs, uint32_t keyPair)
{
    PUT_UINT32_BE(keyPair, adrs->x.padding, 0);
}

static void XAdrsSetChainAddr(XmssAdrs *adrs, uint32_t chain)
{
    PUT_UINT32_BE(chain, adrs->x.padding, 4); // chain address is 4 bytes, start from 4-th byte
}

static void XAdrsSetTreeHeight(XmssAdrs *adrs, uint32_t height)
{
    PUT_UINT32_BE(height, adrs->x.padding, 4); // tree height is 4 bytes, start from 4-th byte
}

static void XAdrsSetHashAddr(XmssAdrs *adrs, uint32_t hash)
{
    PUT_UINT32_BE(hash, adrs->x.padding, 8); // hash address is 4 bytes, start from 8-th byte
}

static void XAdrsSetTreeIndex(XmssAdrs *adrs, uint32_t index)
{
    PUT_UINT32_BE(index, adrs->x.padding, 8); // tree index is 4 bytes, start from 8-th byte
}

static void XAdrsSetKeyAndMask(XmssAdrs *adrs, uint32_t KeyAndMask)
{
    PUT_UINT32_BE(KeyAndMask, adrs->x.padding, 12); // KeyAndMask is 4 bytes, start from 12-th byte
}

static uint32_t XAdrsGetTreeHeight(const XmssAdrs *adrs)
{
    return GET_UINT32_BE(adrs->x.padding, 4);
}

static uint32_t XAdrsGetTreeIndex(const XmssAdrs *adrs)
{
    return GET_UINT32_BE(adrs->x.padding, 8); // tree index is 4 bytes, start from 8-th byte
}

static void XAdrsCopyKeyPairAddr(XmssAdrs *adrs, const XmssAdrs *adrs2)
{
    (void)memcpy_s(adrs->x.padding, sizeof(adrs->x.padding), adrs2->x.padding,
                   4); // key pair address is 4 bytes, start from 4-th byte
}

static uint32_t XAdrsGetAdrsLen()
{
    return XMSS_ADRS_LEN;
}

CryptXmssCtx *CRYPT_XMSS_NewCtx(void)
{
    CryptXmssCtx *ctx = (CryptXmssCtx *)BSL_SAL_Calloc(sizeof(CryptXmssCtx), 1);
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return NULL;
    }
    ctx->isXmss = true;
    ctx->para.algId = 0;
    return ctx;
}

CryptXmssCtx *CRYPT_XMSS_NewCtxEx(void *libCtx)
{
    CryptXmssCtx *ctx = CRYPT_XMSS_NewCtx();
    if (ctx == NULL) {
        return NULL;
    }
    ctx->libCtx = libCtx;
    return ctx;
}

void CRYPT_XMSS_FreeCtx(CryptXmssCtx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    BSL_SAL_CleanseData(ctx->prvKey.seed, sizeof(ctx->prvKey.seed));
    BSL_SAL_CleanseData(ctx->prvKey.prf, sizeof(ctx->prvKey.prf));
    BSL_SAL_Free(ctx);
}

static bool CheckNotXmssAlgId(int32_t algId)
{
    if (algId > CRYPT_XMSSMT_SHAKE256_60_12_192 || algId < CRYPT_XMSS_SHA2_10_256) {
        return true;
    }
    return false;
}

int32_t CRYPT_XMSS_Gen(CryptXmssCtx *ctx)
{
    int32_t ret;
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (CheckNotXmssAlgId(ctx->para.algId)) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_ALGID);
        return CRYPT_XMSS_ERR_INVALID_ALGID;
    }
    uint32_t n = ctx->para.n;
    uint32_t d = ctx->para.d;
    uint32_t hp = ctx->para.hp;
    ret = CRYPT_RandEx(ctx->libCtx, ctx->prvKey.seed, n);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }

    ret = CRYPT_RandEx(ctx->libCtx, ctx->prvKey.prf, n);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    
    ret = CRYPT_RandEx(ctx->libCtx, ctx->prvKey.pub.seed, n);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    XmssAdrs adrs;
    (void)memset_s(&adrs, sizeof(XmssAdrs), 0, sizeof(XmssAdrs));
    ctx->adrsOps.setLayerAddr(&adrs, d - 1);
    uint8_t node[MAX_MDSIZE] = {0};
    ret = XmssNode(node, 0, hp, &adrs, ctx, NULL, 0);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    (void)memcpy_s(ctx->prvKey.pub.root, n, node, n);

    /* init the private key index to 0 */
    ctx->prvKey.index = 0;

    return CRYPT_SUCCESS;
}

/* integer to big-endian bytes */
static void U64toBytes(uint8_t *out, uint32_t outlen, uint64_t in)
{
    for (int32_t i = outlen - 1; i >= 0; i--) {
        out[i] = in & 0xff;
        in = in >> 8;
    }
}

/* big-endian bytes to integer. */
static uint64_t BytestoU64(const uint8_t *in, uint32_t inlen)
{
    uint64_t ret = 0;
    for (; inlen > 0; in++, inlen--) {
        ret = ret << 8;
        ret |= in[0];
    }
    return ret;
}

static int32_t CRYPT_XMSS_SignInternal(CryptXmssCtx *ctx, const uint8_t *msg, uint32_t msgLen, uint8_t *sig,
                                       uint32_t *sigLen)
{
    int32_t ret;
    uint32_t n = ctx->para.n;
    uint32_t d = ctx->para.d;
    uint32_t h = ctx->para.h;
    uint32_t hp = ctx->para.hp;
    uint32_t sigBytes = ctx->para.sigBytes;
    uint64_t index = ctx->prvKey.index;
    uint64_t treeIdx;
    uint32_t leafIdx;
    uint32_t IdxBytes;
    uint8_t idx[MAX_MDSIZE] = {0};

    if (*sigLen < sigBytes) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_SIG_LEN);
        return CRYPT_XMSS_ERR_INVALID_SIG_LEN;
    }
    
    if (h == 64) {
        /* we do not use the last signature while total height 64,
         * otherwisw, index will wrap. */
        if (index == ((1ULL << h) - 1)) {
            BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_KEY_EXPIRED);
            return CRYPT_XMSS_ERR_KEY_EXPIRED;
        }
    } else {
        if (index > ((1ULL << h) - 1)) {
            BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_KEY_EXPIRED);
            return CRYPT_XMSS_ERR_KEY_EXPIRED;
        }
    }
    
    /* increment the private key index */
    /* An implementation MUST NOT output the signature before the private key is updated. */
    ctx->prvKey.index++;
    
    uint32_t offset = 0;
    uint32_t left = 0;
    
    if (d == 1) {
        /* XMSS, 4-bytes index_bytes*/
        IdxBytes = 4;
    } else {
        /* XMSSMT, (ceil(h / 8))-bytes index_bytes */
        IdxBytes = (h + 7) / 8;
    }
    U64toBytes(sig, IdxBytes, index);
    offset += IdxBytes;
    
    PUT_UINT64_BE(index, idx, sizeof(idx) - 8);

    ret = ctx->hashFuncs.prfmsg(ctx, idx + sizeof(idx) - 32, NULL, 0, sig + offset);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }

    uint8_t digest[MAX_MDSIZE] = {0};
    ret = ctx->hashFuncs.hmsg(ctx, sig + offset, msg, msgLen, idx + sizeof(idx) - n, digest);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    offset += n;
    left = *sigLen - offset;
    
    leafIdx = index & ((1UL << hp) - 1);
    treeIdx = index >> hp;
    ret = HypertreeSign(digest, n, treeIdx, leafIdx, ctx, sig + offset, &left);
    *sigLen = offset + left;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_Sign(CryptXmssCtx *ctx, int32_t algId, const uint8_t *data, uint32_t dataLen, uint8_t *sign,
                        uint32_t *signLen)
{
    (void)algId;
    int32_t ret;

    if (ctx == NULL || data == NULL || dataLen == 0 || sign == NULL || signLen == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    ret = CRYPT_XMSS_SignInternal(ctx, data, dataLen, sign, signLen);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    return CRYPT_SUCCESS;
}

static int32_t CRYPT_XMSS_VerifyInternal(const CryptXmssCtx *ctx, const uint8_t *msg, uint32_t msgLen,
                                         const uint8_t *sig, uint32_t sigLen)
{
    int32_t ret;
    uint32_t n = ctx->para.n;
    uint32_t d = ctx->para.d;
    uint32_t h = ctx->para.h;
    uint32_t hp = ctx->para.hp;
    uint32_t sigBytes = ctx->para.sigBytes;
    uint64_t index;
    uint64_t treeIdx;
    uint32_t leafIdx;
    uint32_t IdxBytes;
    uint8_t idx[MAX_MDSIZE] = {0};

    if (sigLen != sigBytes) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_SIG_LEN);
        return CRYPT_XMSS_ERR_INVALID_SIG_LEN;
    }
    uint32_t offset = 0;
    
    if (d == 1) {
        /* XMSS, 4-bytes index_bytes*/
        IdxBytes = 4;
    } else {
        /* XMSSMT, (ceil(h / 8))-bytes index_bytes */
        IdxBytes = (h + 7) / 8;
    }
    index = BytestoU64(sig, IdxBytes);
    offset += IdxBytes;
    
    PUT_UINT64_BE(index, idx, sizeof(idx) - 8);

    uint8_t digest[MAX_MDSIZE] = {0};
    ret = ctx->hashFuncs.hmsg(ctx, sig + offset, msg, msgLen, idx + sizeof(idx) - n, digest);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    offset += n;

    leafIdx = index & ((1UL << hp) - 1);
    treeIdx = index >> hp;
    ret = HypertreeVerify(digest, n, sig + offset, sigLen - offset, treeIdx, leafIdx, ctx);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_Verify(const CryptXmssCtx *ctx, int32_t algId, const uint8_t *data, uint32_t dataLen,
                          const uint8_t *sign, uint32_t signLen)
{
    (void)algId;
    int32_t ret;

    if (ctx == NULL || data == NULL || dataLen == 0 || sign == NULL || signLen == 0) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    ret = CRYPT_XMSS_VerifyInternal(ctx, data, dataLen, sign, signLen);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    return CRYPT_SUCCESS;
}

static const XmssPara *FindXmssPara(CRYPT_PKEY_ParaId algId)
{
    for (uint32_t i = 0; i < sizeof(XmssParaTable)/sizeof(XmssParaTable[0]); i++) {
        if ((CRYPT_PKEY_ParaId)XmssParaTable[i].algId == algId) {
            return &XmssParaTable[i];
        }
    }
    return NULL;
}

static int32_t XmssSetAlgId(CryptXmssCtx *ctx, CRYPT_PKEY_ParaId algId)
{
    int32_t ret;
    const XmssPara *para = NULL;
    static const AdrsOps XadrsOps = {
        .setLayerAddr = XAdrsSetLayerAddr,
        .setTreeAddr = XAdrsSetTreeAddr,
        .setType = XAdrsSetType,
        .setKeyPairAddr = XAdrsSetKeyPairAddr,
        .setChainAddr = XAdrsSetChainAddr,
        .setTreeHeight = XAdrsSetTreeHeight,
        .setHashAddr = XAdrsSetHashAddr,
        .setTreeIndex = XAdrsSetTreeIndex,
        .setKeyAndMask = XAdrsSetKeyAndMask,
        .getTreeHeight = XAdrsGetTreeHeight,
        .getTreeIndex = XAdrsGetTreeIndex,
        .copyKeyPairAddr = XAdrsCopyKeyPairAddr,
        .getAdrsLen = XAdrsGetAdrsLen,
    };

    para = FindXmssPara(algId);
    if (para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_ALGID);
        return CRYPT_XMSS_ERR_INVALID_ALGID;
    }
    ctx->para = *para;
    
    ret = XmssInitHashFuncs(ctx);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_ALGID);
        return CRYPT_XMSS_ERR_INVALID_ALGID;
    }
    ctx->adrsOps = XadrsOps;
    
    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_Ctrl(CryptXmssCtx *ctx, int32_t opt, void *val, uint32_t len)
{
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    switch (opt) {
        case CRYPT_CTRL_SET_PARA_BY_ID:
            if (val == NULL || len != sizeof(int32_t)) {
                BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
                return CRYPT_INVALID_ARG;
            }
            CRYPT_PKEY_ParaId algId = *(CRYPT_PKEY_ParaId *)val;
            if (CheckNotXmssAlgId(algId)) {
                BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_ALGID);
                return CRYPT_XMSS_ERR_INVALID_ALGID;
            }
            return XmssSetAlgId(ctx, algId);
        case CRYPT_CTRL_GET_XMSS_KEY_LEN:
            if (val == NULL || len != sizeof(uint32_t)) {
                BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
                return CRYPT_INVALID_ARG;
            }
            *(uint32_t *)val = ctx->para.n;
            return CRYPT_SUCCESS;
        default:
            BSL_ERR_PUSH_ERROR(CRYPT_NOT_SUPPORT);
            return CRYPT_NOT_SUPPORT;
    }
}

static int32_t XPubKeyParamCheck(const CryptXmssCtx *ctx, BSL_Param *para, XmssPubKeyParam *pub)
{
    if (ctx == NULL || para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    pub->pubSeed = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PUB_SEED);
    pub->pubRoot = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PUB_ROOT);
    if (pub->pubSeed == NULL || pub->pubSeed->value == NULL || pub->pubRoot == NULL || pub->pubRoot->value == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (pub->pubSeed->valueLen != ctx->para.n || pub->pubRoot->valueLen != ctx->para.n) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_KEYLEN);
        return CRYPT_XMSS_ERR_INVALID_KEYLEN;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_GetPubKey(const CryptXmssCtx *ctx, BSL_Param *para)
{
    XmssPubKeyParam pub;
    int32_t ret = XPubKeyParamCheck(ctx, para, &pub);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    pub.pubSeed->useLen = pub.pubRoot->useLen = ctx->para.n;
    (void)memcpy_s(pub.pubSeed->value, pub.pubSeed->valueLen, ctx->prvKey.pub.seed, ctx->para.n);
    (void)memcpy_s(pub.pubRoot->value, pub.pubRoot->valueLen, ctx->prvKey.pub.root, ctx->para.n);

    return CRYPT_SUCCESS;
}

static int32_t XPrvKeyParamCheck(const CryptXmssCtx *ctx, BSL_Param *para, XmssPrvKeyParam *prv)
{
    if (ctx == NULL || para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    prv->prvIndex = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PRV_INDEX);
    prv->prvSeed = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PRV_SEED);
    prv->prvPrf = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PRV_PRF);
    prv->pubSeed = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PUB_SEED);
    prv->pubRoot = BSL_PARAM_FindParam(para, CRYPT_PARAM_XMSS_PUB_ROOT);
    if (prv->prvIndex == NULL || prv->prvIndex->value == NULL || prv->prvSeed == NULL || prv->prvSeed->value == NULL ||
        prv->prvPrf == NULL || prv->prvPrf->value == NULL || prv->pubSeed == NULL || prv->pubSeed->value == NULL ||
        prv->pubRoot == NULL || prv->pubRoot->value == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (prv->prvIndex->valueLen != sizeof(ctx->prvKey.index) || prv->prvSeed->valueLen != ctx->para.n ||
        prv->prvPrf->valueLen != ctx->para.n || prv->pubSeed->valueLen != ctx->para.n ||
        prv->pubRoot->valueLen != ctx->para.n) {
        BSL_ERR_PUSH_ERROR(CRYPT_XMSS_ERR_INVALID_KEYLEN);
        return CRYPT_XMSS_ERR_INVALID_KEYLEN;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_GetPrvKey(const CryptXmssCtx *ctx, BSL_Param *para)
{
    XmssPrvKeyParam prv;
    uint64_t index = ctx->prvKey.index;
    int32_t ret = XPrvKeyParamCheck(ctx, para, &prv);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }

    prv.prvSeed->useLen = ctx->para.n;
    prv.prvPrf->useLen = ctx->para.n;
    prv.pubSeed->useLen = ctx->para.n;
    prv.pubRoot->useLen = ctx->para.n;
    (void)memcpy_s(prv.prvSeed->value, prv.prvSeed->valueLen, ctx->prvKey.seed, ctx->para.n);
    (void)memcpy_s(prv.prvPrf->value, prv.prvPrf->valueLen, ctx->prvKey.prf, ctx->para.n);
    (void)memcpy_s(prv.pubSeed->value, prv.pubSeed->valueLen, ctx->prvKey.pub.seed, ctx->para.n);
    (void)memcpy_s(prv.pubRoot->value, prv.pubRoot->valueLen, ctx->prvKey.pub.root, ctx->para.n);

    return BSL_PARAM_SetValue(prv.prvIndex, CRYPT_PARAM_XMSS_PRV_INDEX, BSL_PARAM_TYPE_UINT64,
            &index, sizeof(index));
}

int32_t CRYPT_XMSS_SetPubKey(CryptXmssCtx *ctx, const BSL_Param *para)
{
    XmssPubKeyParam pub;
    int32_t ret = XPubKeyParamCheck(ctx, (BSL_Param *)(uintptr_t)para, &pub);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }
    (void)memcpy_s(ctx->prvKey.pub.seed, ctx->para.n, pub.pubSeed->value, ctx->para.n);
    (void)memcpy_s(ctx->prvKey.pub.root, ctx->para.n, pub.pubRoot->value, ctx->para.n);

    return CRYPT_SUCCESS;
}

int32_t CRYPT_XMSS_SetPrvKey(CryptXmssCtx *ctx, const BSL_Param *para)
{
    XmssPrvKeyParam prv;
    uint32_t tmplen = sizeof(ctx->prvKey.index);
    int32_t ret = XPrvKeyParamCheck(ctx, (BSL_Param *)(uintptr_t)para, &prv);
    if (ret != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(ret);
        return ret;
    }

    (void)memcpy_s(ctx->prvKey.seed, sizeof(ctx->prvKey.seed), prv.prvSeed->value, ctx->para.n);
    (void)memcpy_s(ctx->prvKey.prf, sizeof(ctx->prvKey.prf), prv.prvPrf->value, ctx->para.n);
    (void)memcpy_s(ctx->prvKey.pub.seed, sizeof(ctx->prvKey.pub.seed), prv.pubSeed->value, ctx->para.n);
    (void)memcpy_s(ctx->prvKey.pub.root, sizeof(ctx->prvKey.pub.root), prv.pubRoot->value, ctx->para.n);

    return BSL_PARAM_GetValue(prv.prvIndex, CRYPT_PARAM_XMSS_PRV_INDEX, BSL_PARAM_TYPE_UINT64,
            &ctx->prvKey.index, &tmplen);
}

#endif // HITLS_CRYPTO_XMSS
