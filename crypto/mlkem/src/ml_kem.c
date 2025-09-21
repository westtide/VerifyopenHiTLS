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
#ifdef HITLS_CRYPTO_MLKEM
#include "securec.h"
#include "crypt_errno.h"
#include "crypt_algid.h"
#include "bsl_sal.h"
#include "bsl_err_internal.h"
#include "eal_pkey_local.h"
#include "crypt_util_rand.h"
#include "crypt_utils.h"
#include "ml_kem_local.h"

static const CRYPT_MlKemInfo ML_KEM_INFO[] = {
    {2, 3, 2, 10, 4, 128, 800, 1632, 768, 32, 512},
    {3, 2, 2, 10, 4, 192, 1184, 2400, 1088, 32, 768},
    {4, 2, 2, 11, 5, 256, 1568, 3168, 1568, 32, 1024}
};

static const CRYPT_MlKemInfo *MlKemGetInfo(uint32_t bits)
{
    for (uint32_t i = 0; i < sizeof(ML_KEM_INFO) / sizeof(ML_KEM_INFO[0]); i++) {
        if (ML_KEM_INFO[i].bits == bits) {
            return &ML_KEM_INFO[i];
        }
    }
    return NULL;
}

CRYPT_ML_KEM_Ctx *CRYPT_ML_KEM_NewCtx(void)
{
    CRYPT_ML_KEM_Ctx *keyCtx = BSL_SAL_Malloc(sizeof(CRYPT_ML_KEM_Ctx));
    if (keyCtx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return NULL;
    }
    (void)memset_s(keyCtx, sizeof(CRYPT_ML_KEM_Ctx), 0, sizeof(CRYPT_ML_KEM_Ctx));
    BSL_SAL_ReferencesInit(&(keyCtx->references));
    return keyCtx;
}

CRYPT_ML_KEM_Ctx *CRYPT_ML_KEM_NewCtxEx(void *libCtx)
{
    CRYPT_ML_KEM_Ctx *ctx = CRYPT_ML_KEM_NewCtx();
    if (ctx == NULL) {
        return NULL;
    }
    ctx->libCtx = libCtx;
    return ctx;
}

void CRYPT_ML_KEM_FreeCtx(CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    int ret = 0;
    BSL_SAL_AtomicDownReferences(&(ctx->references), &ret);
    if (ret > 0) {
        return;
    }
    BSL_SAL_CleanseData(ctx->dk, ctx->dkLen);
    BSL_SAL_FREE(ctx->dk);
    BSL_SAL_FREE(ctx->ek);
    BSL_SAL_ReferencesFree(&(ctx->references));
    BSL_SAL_FREE(ctx);
}

static int32_t MlKemSetAlgInfo(CRYPT_ML_KEM_Ctx *ctx, void *val, uint32_t len)
{
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
        return CRYPT_INVALID_ARG;
    }
    if (ctx->info != NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_CTRL_INIT_REPEATED);
        return CRYPT_MLKEM_CTRL_INIT_REPEATED;
    }
    uint32_t bits = 0;
    int32_t keyType = *(int32_t*)val;
    if (keyType == CRYPT_KEM_TYPE_MLKEM_512) {
        bits = 512;  // MLKEM512
    } else if (keyType == CRYPT_KEM_TYPE_MLKEM_768) {
        bits = 768;  // MLKEM768
    } else if (keyType == CRYPT_KEM_TYPE_MLKEM_1024) {
        bits = 1024;  // MLKEM1024
    }
    const CRYPT_MlKemInfo *info = MlKemGetInfo(bits);
    if (info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NOT_SUPPORT);
        return CRYPT_NOT_SUPPORT;
    }
    ctx->info = info;
    return CRYPT_SUCCESS;
}

CRYPT_ML_KEM_Ctx *CRYPT_ML_KEM_DupCtx(CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return NULL;
    }
    CRYPT_ML_KEM_Ctx *newCtx = CRYPT_ML_KEM_NewCtx();
    if (newCtx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return NULL;
    }
    if (ctx->info != NULL) {
        newCtx->info = ctx->info;
    }
    if (ctx->ek != NULL) {
        newCtx->ek = BSL_SAL_Dump(ctx->ek, ctx->ekLen);
        if (newCtx->ek == NULL) {
            CRYPT_ML_KEM_FreeCtx(newCtx);
            return NULL;
        }
        newCtx->ekLen = ctx->ekLen;
    }
    if (ctx->dk != NULL) {
        newCtx->dk = BSL_SAL_Dump(ctx->dk, ctx->dkLen);
        if (newCtx->dk == NULL) {
            CRYPT_ML_KEM_FreeCtx(newCtx);
            return NULL;
        }
        newCtx->dkLen = ctx->dkLen;
    }
    newCtx->libCtx = ctx->libCtx;
    return newCtx;
}

static int32_t MlKemGetEncapsKeyLen(CRYPT_ML_KEM_Ctx *ctx, void *val, uint32_t len)
{
    if (ctx->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
        return CRYPT_INVALID_ARG;
    }
    *(uint32_t*)val = ctx->info->encapsKeyLen;
    return CRYPT_SUCCESS;
}

static int32_t MlKemGetDecapsKeyLen(CRYPT_ML_KEM_Ctx *ctx, void *val, uint32_t len)
{
    if (ctx->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
        return CRYPT_INVALID_ARG;
    }
    *(uint32_t*)val = ctx->info->decapsKeyLen;
    return CRYPT_SUCCESS;
}

static int32_t MlKemGetCipherTextLen(CRYPT_ML_KEM_Ctx *ctx, void *val, uint32_t len)
{
    if (ctx->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
        return CRYPT_INVALID_ARG;
    }
    *(uint32_t*)val = ctx->info->cipherLen;
    return CRYPT_SUCCESS;
}

static int32_t MlKemGetSharedLen(CRYPT_ML_KEM_Ctx *ctx, void *val, uint32_t len)
{
    if (ctx->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (len != sizeof(uint32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
        return CRYPT_INVALID_ARG;
    }
    *(uint32_t*)val = ctx->info->sharedLen;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_SetEncapsKey(CRYPT_ML_KEM_Ctx *ctx, const CRYPT_KemEncapsKey *ek)
{
    if (ctx == NULL || ctx->info == NULL || ek == NULL || ek->data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ek->len != ctx->info->encapsKeyLen) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYLEN_ERROR);
        return CRYPT_MLKEM_KEYLEN_ERROR;
    }

    uint8_t *data = BSL_SAL_Dump(ek->data, ek->len);
    if (data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    if (ctx->ek != NULL) {
        BSL_SAL_Free(ctx->ek);
    }
    ctx->ek = data;
    ctx->ekLen = ek->len;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_GetEncapsKey(const CRYPT_ML_KEM_Ctx *ctx, CRYPT_KemEncapsKey *ek)
{
    if (ctx == NULL || ctx->info == NULL || ek == NULL || ek->data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->ek == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_SET);
        return CRYPT_MLKEM_KEY_NOT_SET;
    }

    if (memcpy_s(ek->data, ek->len, ctx->ek, ctx->ekLen) != EOK) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYLEN_ERROR);
        return CRYPT_MLKEM_KEYLEN_ERROR;
    }
    ek->len = ctx->ekLen;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_SetDecapsKey(CRYPT_ML_KEM_Ctx *ctx, const CRYPT_KemDecapsKey *dk)
{
    if (ctx == NULL || ctx->info == NULL || dk == NULL || dk->data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (dk->len != ctx->info->decapsKeyLen) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYLEN_ERROR);
        return CRYPT_MLKEM_KEYLEN_ERROR;
    }

    uint8_t *data = BSL_SAL_Dump(dk->data, dk->len);
    if (data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    if (ctx->dk != NULL) {
        BSL_SAL_CleanseData(ctx->dk, ctx->dkLen);
        BSL_SAL_Free(ctx->dk);
    }
    ctx->dk = data;
    ctx->dkLen = dk->len;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_GetDecapsKey(const CRYPT_ML_KEM_Ctx *ctx, CRYPT_KemDecapsKey *dk)
{
    if (ctx == NULL || ctx->info == NULL || dk == NULL || dk->data == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if (ctx->dk == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_SET);
        return CRYPT_MLKEM_KEY_NOT_SET;
    }

    if (memcpy_s(dk->data, dk->len, ctx->dk, ctx->dkLen) != EOK) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYLEN_ERROR);
        return CRYPT_MLKEM_KEYLEN_ERROR;
    }
    dk->len = ctx->dkLen;
    return CRYPT_SUCCESS;
}

#ifdef HITLS_BSL_PARAMS
int32_t CRYPT_ML_KEM_SetEncapsKeyEx(CRYPT_ML_KEM_Ctx *ctx, const BSL_Param *para)
{
    if (para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    CRYPT_KemEncapsKey pub = {0};
    (void)GetConstParamValue(para, CRYPT_PARAM_ML_KEM_PUBKEY, &pub.data, &pub.len);
    return CRYPT_ML_KEM_SetEncapsKey(ctx, &pub);
}

int32_t CRYPT_ML_KEM_GetEncapsKeyEx(const CRYPT_ML_KEM_Ctx *ctx, BSL_Param *para)
{
    if (para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    CRYPT_KemEncapsKey pub = {0};
    BSL_Param *paramPub = GetParamValue(para, CRYPT_PARAM_ML_KEM_PUBKEY, &pub.data, &(pub.len));
    int32_t ret = CRYPT_ML_KEM_GetEncapsKey(ctx, &pub);
    if (ret != CRYPT_SUCCESS) {
        return ret;
    }
    paramPub->useLen = pub.len;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_SetDecapsKeyEx(CRYPT_ML_KEM_Ctx *ctx, const BSL_Param *para)
{
    if (para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    CRYPT_KemDecapsKey prv = {0};
    (void)GetConstParamValue(para, CRYPT_PARAM_ML_KEM_PRVKEY, &prv.data, &prv.len);
    return CRYPT_ML_KEM_SetDecapsKey(ctx, &prv);
}

int32_t CRYPT_ML_KEM_GetDecapsKeyEx(const CRYPT_ML_KEM_Ctx *ctx, BSL_Param *para)
{
    if (para == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    CRYPT_KemDecapsKey prv = {0};
    BSL_Param *paramPrv = GetParamValue(para, CRYPT_PARAM_ML_KEM_PRVKEY, &prv.data, &(prv.len));
    int32_t ret = CRYPT_ML_KEM_GetDecapsKey(ctx, &prv);
    if (ret != CRYPT_SUCCESS) {
        return ret;
    }
    paramPrv->useLen = prv.len;
    return CRYPT_SUCCESS;
}
#endif

static int32_t MlKemCmpKey(uint8_t *a, uint32_t aLen, uint8_t *b, uint32_t bLen)
{
    if (aLen != bLen) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_EQUAL);
        return CRYPT_MLKEM_KEY_NOT_EQUAL;
    }
    if (a != NULL && b != NULL) {
        if (memcmp(a, b, aLen) != 0) {
            BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_EQUAL);
            return CRYPT_MLKEM_KEY_NOT_EQUAL;
        }
    }
    if ((a != NULL) != (b != NULL)) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_EQUAL);
        return CRYPT_MLKEM_KEY_NOT_EQUAL;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_Cmp(const CRYPT_ML_KEM_Ctx *a, const CRYPT_ML_KEM_Ctx *b)
{
    if (a == NULL || b == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (a->info != b->info) {  // The value of info must be one of the ML_KEM_INFO arrays.
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEY_NOT_EQUAL);
        return CRYPT_MLKEM_KEY_NOT_EQUAL;
    }
 
    if (MlKemCmpKey(a->ek, a->ekLen, b->ek, b->ekLen) != CRYPT_SUCCESS) {
        return CRYPT_MLKEM_KEY_NOT_EQUAL;
    }
    if (MlKemCmpKey(a->dk, a->dkLen, b->dk, b->dkLen) != CRYPT_SUCCESS) {
        return CRYPT_MLKEM_KEY_NOT_EQUAL;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_GetSecBits(const CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx == NULL || ctx->info == NULL) {
        return 0;
    }
    return (int32_t)ctx->info->secBits;
}

static int32_t CRYPT_ML_KEM_GetLen(const CRYPT_ML_KEM_Ctx *ctx, GetLenFunc func, void *val, uint32_t len)
{
    if (val == NULL || len != sizeof(int32_t)) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    *(int32_t *)val = func(ctx);
    return CRYPT_SUCCESS;
}

static int32_t MlKemCleanPubKey(CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx->ek != NULL) {
        BSL_SAL_CleanseData(ctx->ek, ctx->ekLen);
        BSL_SAL_FREE(ctx->ek);
        ctx->ekLen = 0;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_Ctrl(CRYPT_ML_KEM_Ctx *ctx, int32_t opt, void *val, uint32_t len)
{
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (opt == CRYPT_CTRL_GET_SECBITS) {
        return CRYPT_ML_KEM_GetLen(ctx, (GetLenFunc)CRYPT_ML_KEM_GetSecBits, val, len);
    }
    if (opt == CRYPT_CTRL_CLEAN_PUB_KEY) {
        return MlKemCleanPubKey(ctx);
    }
    if (val == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    switch (opt) {
        case CRYPT_CTRL_SET_PARA_BY_ID:
            return MlKemSetAlgInfo(ctx, val, len);
        case CRYPT_CTRL_GET_PUBKEY_LEN:
            return MlKemGetEncapsKeyLen(ctx, val, len);
        case CRYPT_CTRL_GET_PRVKEY_LEN:
            return MlKemGetDecapsKeyLen(ctx, val, len);
        case CRYPT_CTRL_GET_CIPHERTEXT_LEN:
            return MlKemGetCipherTextLen(ctx, val, len);
        case CRYPT_CTRL_GET_SHARED_KEY_LEN:
            return MlKemGetSharedLen(ctx, val, len);
        default:
            BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_CTRL_NOT_SUPPORT);
            return CRYPT_MLKEM_CTRL_NOT_SUPPORT;
    }
}

static int32_t MlKemCreateKeyBuf(CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx->dk == NULL) {
        uint8_t *dk = BSL_SAL_Malloc(ctx->info->decapsKeyLen);
        if (dk == NULL) {
            BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
            return CRYPT_MEM_ALLOC_FAIL;
        }
        ctx->dk = dk;
        ctx->dkLen = ctx->info->decapsKeyLen;
    }
    if (ctx->ek == NULL) {
        uint8_t *ek = BSL_SAL_Malloc(ctx->info->encapsKeyLen);
        if (ek == NULL) {
            BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
            return CRYPT_MEM_ALLOC_FAIL;
        }
        ctx->ek = ek;
        ctx->ekLen = ctx->info->encapsKeyLen;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_GenKey(CRYPT_ML_KEM_Ctx *ctx)
{
    if (ctx == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (MlKemCreateKeyBuf(ctx) != CRYPT_SUCCESS) {
        return CRYPT_MEM_ALLOC_FAIL;
    }
    uint8_t d[MLKEM_SEED_LEN];
    uint8_t z[MLKEM_SEED_LEN];
    int32_t ret = CRYPT_RandEx(ctx->libCtx, d, MLKEM_SEED_LEN);
    RETURN_RET_IF(ret != CRYPT_SUCCESS, ret);
    ret = CRYPT_RandEx(ctx->libCtx, z, MLKEM_SEED_LEN);
    RETURN_RET_IF(ret != CRYPT_SUCCESS, ret);

    ret = MLKEM_KeyGenInternal(ctx, d, z);
    BSL_SAL_CleanseData(d, MLKEM_SEED_LEN);
    BSL_SAL_CleanseData(z, MLKEM_SEED_LEN);
    return ret;
}

static int32_t EncCapsInputCheck(const CRYPT_ML_KEM_Ctx *ctx, uint8_t *ct, uint32_t *ctLen,
    uint8_t *sk, uint32_t *skLen)
{
    if (ctx == NULL || ctx->ek == NULL || ct == NULL || ctLen == NULL ||
        sk == NULL || skLen == NULL) {
        return CRYPT_NULL_INPUT;
    }
    if (ctx->info == NULL) {
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (*ctLen < ctx->info->cipherLen || *skLen < MLKEM_SHARED_KEY_LEN) {
        return CRYPT_MLKEM_LEN_NOT_ENOUGH;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_Encaps(const CRYPT_ML_KEM_Ctx *ctx, uint8_t *cipher, uint32_t *cipherLen,
    uint8_t *share, uint32_t *shareLen)
{
    int32_t ret = EncCapsInputCheck(ctx, cipher, cipherLen, share, shareLen);
    RETURN_RET_IF(ret != CRYPT_SUCCESS, ret);

    uint8_t m[MLKEM_SEED_LEN];
    ret = CRYPT_RandEx(ctx->libCtx, m, MLKEM_SEED_LEN);
    RETURN_RET_IF(ret != CRYPT_SUCCESS, ret);

    ret = MLKEM_EncapsInternal(ctx, cipher, cipherLen, share, shareLen, m);
    BSL_SAL_CleanseData(m, MLKEM_SEED_LEN);
    return ret;
}

static int32_t DecCapsInputCheck(const CRYPT_ML_KEM_Ctx *ctx, uint8_t *ct, uint32_t ctLen,
    uint8_t *sk, uint32_t *skLen)
{
    if (ctx == NULL || ctx->dk == NULL || ct == NULL || sk == NULL || skLen == NULL) {
        return CRYPT_NULL_INPUT;
    }
    if (ctx->info == NULL) {
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (ctLen != ctx->info->cipherLen || *skLen < MLKEM_SHARED_KEY_LEN) {
        return CRYPT_MLKEM_LEN_NOT_ENOUGH;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_Decaps(const CRYPT_ML_KEM_Ctx *ctx, uint8_t *cipher, uint32_t cipherLen,
    uint8_t *share, uint32_t *shareLen)
{
    int32_t ret = DecCapsInputCheck(ctx, cipher, cipherLen, share, shareLen);
    RETURN_RET_IF(ret != CRYPT_SUCCESS, ret);

    return MLKEM_DecapsInternal(ctx, cipher, cipherLen, share, shareLen);
}

#ifdef HITLS_CRYPTO_MLKEM_CHECK

static int32_t MlKemKeyPairCheck(const CRYPT_ML_KEM_Ctx *pubKey, const CRYPT_ML_KEM_Ctx *prvKey)
{
    if (pubKey == NULL || prvKey == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (pubKey->info == NULL || prvKey->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (pubKey->info->bits != prvKey->info->bits) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_PAIRWISE_CHECK_FAIL);
        return CRYPT_MLKEM_PAIRWISE_CHECK_FAIL;
    }
    uint32_t cipherLen = pubKey->info->cipherLen;
    uint8_t *ciphertext = BSL_SAL_Malloc(pubKey->info->cipherLen);
    if (ciphertext == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    int32_t ret;
    uint32_t sharedLen1 = MLKEM_SHARED_KEY_LEN;
    uint8_t sharedKey1[MLKEM_SHARED_KEY_LEN];
    uint32_t sharedLen2 = MLKEM_SHARED_KEY_LEN;
    uint8_t sharedKey2[MLKEM_SHARED_KEY_LEN];
    GOTO_ERR_IF(CRYPT_ML_KEM_Encaps(pubKey, ciphertext, &cipherLen, sharedKey1, &sharedLen1), ret);
    GOTO_ERR_IF(CRYPT_ML_KEM_Decaps(prvKey, ciphertext, cipherLen, sharedKey2, &sharedLen2), ret);
    if (sharedLen1 != sharedLen2 || memcmp(sharedKey1, sharedKey2, sharedLen1) != 0) {
        ret = CRYPT_MLKEM_PAIRWISE_CHECK_FAIL;
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_PAIRWISE_CHECK_FAIL);
    }
ERR:
    BSL_SAL_CleanseData(sharedKey1, MLKEM_SHARED_KEY_LEN);
    BSL_SAL_ClearFree(ciphertext, pubKey->info->cipherLen);
    return ret;
}

static int32_t MlKemPrvKeyCheck(const CRYPT_ML_KEM_Ctx *prvKey)
{
    if (prvKey == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (prvKey->info == NULL) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_KEYINFO_NOT_SET);
        return CRYPT_MLKEM_KEYINFO_NOT_SET;
    }
    if (prvKey->dk == NULL || prvKey->dkLen != prvKey->info->decapsKeyLen) {
        BSL_ERR_PUSH_ERROR(CRYPT_MLKEM_INVALID_PRVKEY);
        return CRYPT_MLKEM_INVALID_PRVKEY;
    }
    return CRYPT_SUCCESS;
}

int32_t CRYPT_ML_KEM_Check(uint32_t checkType, const CRYPT_ML_KEM_Ctx *pkey1, const CRYPT_ML_KEM_Ctx *pkey2)
{
    switch (checkType) {
        case CRYPT_PKEY_CHECK_KEYPAIR:
            return MlKemKeyPairCheck(pkey1, pkey2);
        case CRYPT_PKEY_CHECK_PRVKEY:
            return MlKemPrvKeyCheck(pkey1);
        default:
            BSL_ERR_PUSH_ERROR(CRYPT_INVALID_ARG);
            return CRYPT_INVALID_ARG;
    }
}

#endif // HITLS_CRYPTO_MLKEM_CHECK

#endif