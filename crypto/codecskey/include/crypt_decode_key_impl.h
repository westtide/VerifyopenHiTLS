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

#ifndef CRYPT_DECODE_KEY_IMPL_H
#define CRYPT_DECODE_KEY_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hitls_build.h"

#ifdef HITLS_CRYPTO_CODECSKEY
#include <stdint.h>
#include "bsl_params.h"

typedef struct {
    const char *outFormat;
    const char *outType;
} DECODER_CommonCtx;

int32_t DECODER_CommonGetParam(const DECODER_CommonCtx *commonCtx, BSL_Param *param);

void *DECODER_EPKI2PKI_NewCtx(void *provCtx);
int32_t DECODER_EPKI2PKI_GetParam(void *ctx, BSL_Param *param);
int32_t DECODER_EPKI2PKI_SetParam(void *ctx, const BSL_Param *param);
int32_t DECODER_EPKI2PKI_Decode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
void DECODER_EPKI2PKI_FreeOutData(void *ctx, BSL_Param *outParam);
void DECODER_EPKI2PKI_FreeCtx(void *ctx);

int32_t DECODER_DER2KEY_GetParam(void *ctx, BSL_Param *param);
int32_t DECODER_DER2KEY_SetParam(void *ctx, const BSL_Param *param);
void DECODER_DER2KEY_FreeOutData(void *ctx, BSL_Param *outParam);
void DECODER_DER2KEY_FreeCtx(void *ctx);

#ifdef HITLS_CRYPTO_RSA
void *DECODER_RsaDer2KeyNewCtx(void *provCtx);
int32_t DECODER_RsaPrvKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_RsaPubKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_RsaSubPubKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_RsaSubPubKeyWithOutSeqDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_RsaPkcs8Der2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
#endif

#ifdef HITLS_CRYPTO_ECDSA
void *DECODER_EcdsaDer2KeyNewCtx(void *provCtx);
int32_t DECODER_EcdsaPrvKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_EcdsaSubPubKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_EcdsaSubPubKeyWithOutSeqDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_EcdsaPkcs8Der2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
#endif

#ifdef HITLS_CRYPTO_SM2
void *DECODER_Sm2Der2KeyNewCtx(void *provCtx);
int32_t DECODER_Sm2PrvKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_Sm2SubPubKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_Sm2SubPubKeyWithOutSeqDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_Sm2Pkcs8Der2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
#endif

#ifdef HITLS_CRYPTO_ED25519
void *DECODER_Ed25519Der2KeyNewCtx(void *provCtx);
int32_t DECODER_Ed25519SubPubKeyDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_Ed25519SubPubKeyWithOutSeqDer2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
int32_t DECODER_Ed25519Pkcs8Der2KeyDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
#endif

#ifdef HITLS_BSL_PEM
void *DECODER_Pem2DerNewCtx(void *provCtx);
int32_t DECODER_Pem2DerGetParam(void *ctx, BSL_Param *param);
int32_t DECODER_Pem2DerSetParam(void *ctx, const BSL_Param *param);
int32_t DECODER_Pem2DerDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
void DECODER_Pem2DerFreeOutData(void *ctx, BSL_Param *outParam);
void DECODER_Pem2DerFreeCtx(void *ctx);
#endif

void *DECODER_LowKeyObject2PkeyObjectNewCtx(void *provCtx);
int32_t DECODER_LowKeyObject2PkeyObjectSetParam(void *ctx, const BSL_Param *param);
int32_t DECODER_LowKeyObject2PkeyObjectGetParam(void *ctx, BSL_Param *param);
int32_t DECODER_LowKeyObject2PkeyObjectDecode(void *ctx, const BSL_Param *inParam, BSL_Param **outParam);
void DECODER_LowKeyObject2PkeyObjectFreeOutData(void *ctx, BSL_Param *outParam);
void DECODER_LowKeyObject2PkeyObjectFreeCtx(void *ctx);

#endif /* HITLS_CRYPTO_CODECSKEY */

#ifdef __cplusplus
}
#endif

#endif /* CRYPT_DECODE_KEY_IMPL_H */
