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
#if defined(HITLS_CRYPTO_EAL) && defined(HITLS_CRYPTO_MAC)

#include <stdio.h>
#include <stdlib.h>
#include "crypt_eal_mac.h"
#include "securec.h"
#include "bsl_err_internal.h"
#include "bsl_sal.h"
#include "crypt_local_types.h"
#include "crypt_eal_mac.h"
#include "crypt_algid.h"
#include "crypt_errno.h"
#include "crypt_ealinit.h"
#include "eal_mac_local.h"
#include "eal_common.h"
#ifdef HITLS_CRYPTO_PROVIDER
#include "crypt_eal_implprovider.h"
#include "crypt_provider.h"
#endif

#define MAC_TYPE_INVALID 0

static CRYPT_EAL_MacCtx *MacNewCtxInner(int32_t algId, CRYPT_EAL_LibCtx *libCtx, const char *attrName)
{
    CRYPT_EAL_MacCtx *macCtx = BSL_SAL_Malloc(sizeof(CRYPT_EAL_MacCtx));
    if (macCtx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, algId, CRYPT_MEM_ALLOC_FAIL);
        return NULL;
    }
    (void)memset_s(macCtx, sizeof(CRYPT_EAL_MacCtx), 0, sizeof(CRYPT_EAL_MacCtx));
    void *provCtx = NULL;
    EAL_MacMethod *method = EAL_MacFindMethodEx(algId, libCtx, attrName, &macCtx->macMeth, &provCtx);
    if (method == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, algId, CRYPT_EAL_ERR_METH_NULL_MEMBER);
        BSL_SAL_Free(macCtx);
        return NULL;
    }

    if (macCtx->macMeth.newCtx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, algId, CRYPT_NULL_INPUT);
        BSL_SAL_Free(macCtx);
        return NULL;
    }
    void *data = macCtx->macMeth.newCtx(provCtx, algId);
    if (data == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, algId, CRYPT_MEM_ALLOC_FAIL);
        BSL_SAL_Free(macCtx);
        return NULL;
    }

    macCtx->ctx = data;
    macCtx->id = algId;
    macCtx->state = CRYPT_MAC_STATE_NEW;
    return macCtx;
}

CRYPT_EAL_MacCtx *CRYPT_EAL_ProviderMacNewCtx(CRYPT_EAL_LibCtx *libCtx, int32_t algId, const char *attrName)
{
    return MacNewCtxInner(algId, libCtx, attrName);
}

CRYPT_EAL_MacCtx *CRYPT_EAL_MacNewCtx(CRYPT_MAC_AlgId id)
{
#if defined(HITLS_CRYPTO_ASM_CHECK)
    if (CRYPT_ASMCAP_Mac(id) != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, id, CRYPT_EAL_ALG_ASM_NOT_SUPPORT);
        return NULL;
    }
#endif
    return MacNewCtxInner(id, NULL, NULL);
}

void CRYPT_EAL_MacFreeCtx(CRYPT_EAL_MacCtx *ctx)
{
    if (ctx == NULL) {
        return;
    }
    if (ctx->macMeth.freeCtx != NULL) {
        ctx->macMeth.freeCtx(ctx->ctx);
        EAL_EventReport(CRYPT_EVENT_ZERO, CRYPT_ALGO_MAC, ctx->id, CRYPT_SUCCESS);
    } else {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
    }
    BSL_SAL_Free(ctx);
    return;
}

int32_t CRYPT_EAL_MacInit(CRYPT_EAL_MacCtx *ctx, const uint8_t *key, uint32_t len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if (ctx->macMeth.init == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    int32_t ret = ctx->macMeth.init(ctx->ctx, key, len, NULL);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MAC_STATE_INIT;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MacUpdate(CRYPT_EAL_MacCtx *ctx, const uint8_t *in, uint32_t len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if ((ctx->state == CRYPT_MAC_STATE_FINAL) || (ctx->state == CRYPT_MAC_STATE_NEW)) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    if (ctx->macMeth.update == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    int32_t ret = ctx->macMeth.update(ctx->ctx, in, len);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MAC_STATE_UPDATE;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MacFinal(CRYPT_EAL_MacCtx *ctx, uint8_t *out, uint32_t *len)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if ((ctx->state == CRYPT_MAC_STATE_NEW) || (ctx->state == CRYPT_MAC_STATE_FINAL)) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    if (ctx->macMeth.final == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }

    int32_t ret = ctx->macMeth.final(ctx->ctx, out, len);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MAC_STATE_FINAL;
    return CRYPT_SUCCESS;
}

void CRYPT_EAL_MacDeinit(CRYPT_EAL_MacCtx *ctx)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return;
    }
    if (ctx->macMeth.deinit == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return;
    }
    int32_t ret = ctx->macMeth.deinit(ctx->ctx);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, ret);
        return;
    }

    ctx->state = CRYPT_MAC_STATE_NEW;
    return;
}

int32_t CRYPT_EAL_MacReinit(CRYPT_EAL_MacCtx *ctx)
{
    if (ctx == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if (ctx->state == CRYPT_MAC_STATE_NEW) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    if (ctx->macMeth.reinit == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ALG_NOT_SUPPORT);
        return CRYPT_EAL_ALG_NOT_SUPPORT;
    }
    int32_t ret = ctx->macMeth.reinit(ctx->ctx);
    if (ret != CRYPT_SUCCESS) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, ret);
        return ret;
    }
    ctx->state = CRYPT_MAC_STATE_INIT;
    return CRYPT_SUCCESS;
}

int32_t CRYPT_EAL_MacCtrl(CRYPT_EAL_MacCtx *ctx, int32_t cmd, void *val, uint32_t valLen)
{
    if (ctx == NULL || ctx->macMeth.ctrl == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }

    if (cmd == CRYPT_CTRL_GET_MACLEN) {
        return ctx->macMeth.ctrl(ctx->ctx, cmd, val, valLen);
    }

    if (ctx->state != CRYPT_MAC_STATE_INIT) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, ctx->id, CRYPT_EAL_ERR_STATE);
        return CRYPT_EAL_ERR_STATE;
    }

    return ctx->macMeth.ctrl(ctx->ctx, cmd, val, valLen);
}

uint32_t CRYPT_EAL_GetMacLen(const CRYPT_EAL_MacCtx *ctx)
{
    uint32_t result = 0;
    int32_t ret = CRYPT_EAL_MacCtrl((CRYPT_EAL_MacCtx *)(uintptr_t)ctx,
        CRYPT_CTRL_GET_MACLEN, &result, sizeof(uint32_t));
    return (ret == CRYPT_SUCCESS) ? result : 0;
}

bool CRYPT_EAL_MacIsValidAlgId(CRYPT_MAC_AlgId id)
{
    // 1. Check if the dependency method is valid
    EAL_MdMethod mdMethod = {0};
    EAL_MacDepMethod depMeth = {.method = {.md = &mdMethod}};
    int32_t ret = EAL_MacFindDepMethod(id, NULL, NULL, &depMeth, NULL);
    if (ret != CRYPT_SUCCESS) {
        return false;
    }
    // 2. Check if the mac method is valid
    return EAL_MacFindDefaultMethod(id) != NULL;
}

int32_t CRYPT_EAL_MacSetParam(CRYPT_EAL_MacCtx *ctx, const BSL_Param *param)
{
    if (ctx == NULL || param == NULL || ctx->macMeth.setParam == NULL) {
        EAL_ERR_REPORT(CRYPT_EVENT_ERR, CRYPT_ALGO_MAC, CRYPT_MAC_MAX, CRYPT_NULL_INPUT);
        return CRYPT_NULL_INPUT;
    }
    return ctx->macMeth.setParam(ctx->ctx, param);
}

#endif
