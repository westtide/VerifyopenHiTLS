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
#if defined(HITLS_CRYPTO_EAL) && defined(HITLS_CRYPTO_MD)

#include <stdio.h>
#include <stdlib.h>
#include "securec.h"
#include "bsl_err_internal.h"
#include "bsl_sal.h"
#include "crypt_local_types.h"
#include "crypt_eal_md.h"
#include "crypt_algid.h"
#include "crypt_errno.h"
#include "eal_md_local.h"
#include "eal_common.h"
#include "crypt_ealinit.h"
#ifdef HITLS_CRYPTO_PROVIDER
#include "crypt_eal_implprovider.h"
#include "crypt_provider.h"
#endif

static CRYPT_EAL_MdCTX *MdNewCtxInner(CRYPT_MD_AlgId id, CRYPT_EAL_LibCtx *libCtx, const char *attrName)
{
    CRYPT_EAL_MdCTX *ctx = BSL_SAL_Malloc(sizeof(CRYPT_EAL_MdCTX));
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, id, CRYPT_MEM_ALLOC_FAIL);
        return NULL;
    }
    void *provCtx = NULL;
    // The ctx->method will be overwritten if the method is found.
    (void)memset_s(&ctx->method, sizeof(ctx->method), 0, sizeof(ctx->method));
    EAL_MdMethod *method = EAL_MdFindMethodEx(id, libCtx, attrName, &ctx->method, &provCtx);
    if (method == NULL) {
        BSL_SAL_Free(ctx);
        return NULL;
    }

    if (ctx->method.newCtx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, id, CRYPT_NULL_INPUT);
        BSL_SAL_Free(ctx);
        return NULL;
    }
    void *data = ctx->method.newCtx(provCtx, id);
    if (data == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, id, CRYPT_MEM_ALLOC_FAIL);
        BSL_SAL_Free(ctx);
        return NULL;
    }

    ctx->id = id;
    ctx->state = CRYPT_MD_STATE_NEW;
    ctx->data = data;
    return ctx;
}

CRYPT_EAL_MdCTX *CRYPT_EAL_ProviderMdNewCtx(CRYPT_EAL_LibCtx *libCtx, int32_t algId, const char *attrName)
{
    return MdNewCtxInner(algId, libCtx, attrName);
}

CRYPT_EAL_MdCTX *CRYPT_EAL_MdNewCtx(CRYPT_MD_AlgId id)
{
#ifdef HITLS_CRYPTO_ASM_CHECK
    if (CRYPT_ASMCAP_Md(id) != CRYPT_SUCCESS) {
        BSL_ERR_PUSH_ERROR(CRYPT_EAL_ALG_ASM_NOT_SUPPORT);
        return NULL;
    }
#endif

    return MdNewCtxInner(id, NULL, NULL);
}

bool CRYPT_EAL_MdIsValidAlgId(CRYPT_MD_AlgId id)
{
    return EAL_MdFindDefaultMethod(id) != NULL;
}

int32_t CRYPT_EAL_MdGetId(CRYPT_EAL_MdCTX *ctx)
{
    if (ctx == NULL) {
        return CRYPT_MD_MAX;
    }
    return ctx->id;
}

int32_t CRYPT_EAL_MdCopyCtx(CRYPT_EAL_MdCTX *to, const CRYPT_EAL_MdCTX *from)
{
    if (to == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (from == NULL || from->method.dupCtx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if (to->data != NULL) {
        if (to->method.freeCtx == NULL) {
            EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_INVALID_ARG);
            return CRYPT_INVALID_ARG;
        }
        to->method.freeCtx(to->data);
        to->data = NULL;
    }
    void *data = from->method.dupCtx(from->data);
    if (data == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, from->id, CRYPT_MEM_ALLOC_FAIL);
        return CRYPT_MEM_ALLOC_FAIL;
    }
    *to = *from;
    to->data = data;
    return CRYPT_SUCCESS;
}

CRYPT_EAL_MdCTX *CRYPT_EAL_MdDupCtx(const CRYPT_EAL_MdCTX *ctx)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return NULL;
    }
    if (ctx->method.dupCtx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_NULL_INPUT);
        return NULL;
    }

    CRYPT_EAL_MdCTX *newCtx = BSL_SAL_Malloc(sizeof(CRYPT_EAL_MdCTX));
    if (newCtx == NULL) {
        BSL_ERR_PUSH_ERROR(BSL_MALLOC_FAIL);
        return NULL;
    }
    *newCtx = *ctx;
    newCtx->data = ctx->method.dupCtx(ctx->data);
    if (newCtx->data == NULL) {
        BSL_ERR_PUSH_ERROR(BSL_MALLOC_FAIL);
        BSL_SAL_Free(newCtx);
        return NULL;
    }
    return newCtx;
}

void CRYPT_EAL_MdFreeCtx(CRYPT_EAL_MdCTX *ctx)
{
    if (ctx == NULL) {
        return;
    }
    if (ctx->method.freeCtx != NULL) {
        ctx->method.freeCtx(ctx->data);
        EAL_EventReport(CRYPT_EVENT_ZERO, CRYPT_ALGO_MD, ctx->id, CRYPT_SUCCESS);
    } else {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
    }
    BSL_SAL_FREE(ctx);
    return;
}

int32_t CRYPT_EAL_MdInit(CRYPT_EAL_MdCTX *ctx)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->method.init == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    int32_t ret = ctx->method.init(ctx->data, NULL);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MD_STATE_INIT;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MdUpdate(CRYPT_EAL_MdCTX *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->method.update == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    if ((ctx->state == CRYPT_MD_STATE_FINAL) || (ctx->state == CRYPT_MD_STATE_NEW)
        || (ctx->state == CRYPT_MD_STATE_SQUEEZE)) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    int32_t ret = ctx->method.update(ctx->data, data, len);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MD_STATE_UPDATE;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MdFinal(CRYPT_EAL_MdCTX *ctx, uint8_t *out, uint32_t *len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->method.final == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    if ((ctx->state == CRYPT_MD_STATE_NEW) || (ctx->state == CRYPT_MD_STATE_FINAL) ||
        (ctx->state == CRYPT_MD_STATE_SQUEEZE)) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    // The validity of the buffer length that carries the output result (len > ctx->method->mdSize)
    // is determined by the algorithm bottom layer and is not verified here.
    int32_t ret = ctx->method.final(ctx->data, out, len);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MD_STATE_FINAL;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MdSqueeze(CRYPT_EAL_MdCTX *ctx, uint8_t *out, uint32_t len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    if (ctx->method.squeeze == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }
    if ((ctx->state == CRYPT_MD_STATE_NEW) || (ctx->state == CRYPT_MD_STATE_FINAL)) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    int32_t ret = ctx->method.squeeze(ctx->data, out, len);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MD_STATE_SQUEEZE;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MdDeinit(CRYPT_EAL_MdCTX *ctx)
{
    if (ctx == NULL || ctx->method.deinit == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, CRYPT_MD_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    int32_t ret = ctx->method.deinit(ctx->data);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MD_STATE_NEW;
    return CRYPT_SUCCESS;
}

typedef struct {
    CRYPT_MD_AlgId id;
    uint32_t digestSize;
} CRYPT_MD_DigestSizeMap;

static const CRYPT_MD_DigestSizeMap g_mdDigestSizeMap[] = {
    {CRYPT_MD_SHA1, 20},
    {CRYPT_MD_SHA224, 28},
    {CRYPT_MD_SHA256, 32},
    {CRYPT_MD_SHA384, 48},
    {CRYPT_MD_SHA512, 64},
    {CRYPT_MD_SHA3_224, 28},
    {CRYPT_MD_SHA3_256, 32},
    {CRYPT_MD_SHA3_384, 48},
    {CRYPT_MD_SHA3_512, 64},
    {CRYPT_MD_SHAKE128, 0},
    {CRYPT_MD_SHAKE256, 0},
    {CRYPT_MD_SM3, 32},
    {CRYPT_MD_MD5, 16},
};

uint32_t CRYPT_EAL_MdGetDigestSize(CRYPT_MD_AlgId id)
{
    for (uint32_t i = 0; i < sizeof(g_mdDigestSizeMap) / sizeof(g_mdDigestSizeMap[0]); i++) {
        if (g_mdDigestSizeMap[i].id == id) {
            return g_mdDigestSizeMap[i].digestSize;
        }
    }
    EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MD, id, CRYPT_EAL_ERR_ALGID);
    return 0;
}

int32_t CRYPT_EAL_Md(CRYPT_MD_AlgId id, const uint8_t *in, uint32_t inLen, uint8_t *out, uint32_t *outLen)
{
    return EAL_Md(id, NULL, NULL, in, inLen, out, outLen);
}
#endif
