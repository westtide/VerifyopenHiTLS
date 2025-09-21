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

#ifndef CRYPT_SHA2_H
#define CRYPT_SHA2_H

#include "hitls_build.h"
#ifdef HITLS_CRYPTO_SHA2

#include <stdint.h>
#include <stdlib.h>
#include "crypt_types.h"
#include "bsl_params.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

/** @defgroup LLF SHA2 Low level function */

#ifdef HITLS_CRYPTO_SHA224
#define CRYPT_SHA2_224_BLOCKSIZE  64
#define CRYPT_SHA2_224_DIGESTSIZE 28
#endif // HITLS_CRYPTO_SHA224

#ifdef HITLS_CRYPTO_SHA256
#define CRYPT_SHA2_256_BLOCKSIZE  64
#define CRYPT_SHA2_256_DIGESTSIZE 32
#endif // HITLS_CRYPTO_SHA256

#ifdef HITLS_CRYPTO_SHA384
#define CRYPT_SHA2_384_BLOCKSIZE  128
#define CRYPT_SHA2_384_DIGESTSIZE 48
#endif // HITLS_CRYPTO_SHA384

#ifdef HITLS_CRYPTO_SHA512
#define CRYPT_SHA2_512_BLOCKSIZE  128
#define CRYPT_SHA2_512_DIGESTSIZE 64
#endif // HITLS_CRYPTO_SHA512

#define CRYPT_SHA2_224_Squeeze NULL
#define CRYPT_SHA2_256_Squeeze NULL
#define CRYPT_SHA2_384_Squeeze NULL
#define CRYPT_SHA2_512_Squeeze NULL

#ifdef HITLS_CRYPTO_SHA224

typedef struct CryptSha256Ctx CRYPT_SHA2_224_Ctx;

#define CRYPT_SHA2_224_NewCtx CRYPT_SHA2_256_NewCtx
#define CRYPT_SHA2_224_NewCtxEx CRYPT_SHA2_256_NewCtxEx
#define CRYPT_SHA2_224_FreeCtx CRYPT_SHA2_256_FreeCtx
#define CRYPT_SHA2_224_Deinit CRYPT_SHA2_256_Deinit
#define CRYPT_SHA2_224_CopyCtx CRYPT_SHA2_256_CopyCtx
#define CRYPT_SHA2_224_DupCtx CRYPT_SHA2_256_DupCtx
#define CRYPT_SHA2_224_Update CRYPT_SHA2_256_Update
#define CRYPT_SHA2_224_Final CRYPT_SHA2_256_Final

/**
 * @defgroup CRYPT_SHA2_224_Init
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_224_Init(CRYPT_SHA2_224_Ctx *ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the SHA224 ctx for a digest operation.
 *
 * @par Description
 * CRYPT_SHA2_224_Init function initializes the ctx for a digest operation. This function must be called before
 * CRYPT_SHA2_224_Update or CRYPT_SHA2_224_Final operations. This function will not allocate memory for any of the
 * ctx variables. Instead the caller is expected to pass a ctx pointer pointing to a valid memory location
 * (either locally or dynamically allocated).
 *
 * @param[in] ctx The sha224 ctx
 * @param *param [in] Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS ctx is initialized
 * @retval #CRYPT_NULL_INPUT ctx is NULL
 */
int32_t CRYPT_SHA2_224_Init(CRYPT_SHA2_224_Ctx *ctx, BSL_Param *param);

#ifdef HITLS_CRYPTO_PROVIDER
/**
 * @ingroup SHA224
 * @brief SHA224 get param function
 * @param ctx [in]   Pointer to the SHA224 context.
 * @param param [in]   Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS       Success.
 * @retval #CRYPT_NULL_INPUT    Pointer param is NULL
 * @retval #CRYPT_INVALID_ARG   Pointer param is invalid
 */
int32_t CRYPT_SHA2_224_GetParam(CRYPT_SHA2_224_Ctx *ctx, BSL_Param *param);
#else
#define CRYPT_SHA2_224_GetParam NULL
#endif

#endif // HITLS_CRYPTO_SHA224

#ifdef HITLS_CRYPTO_SHA256

typedef struct CryptSha256Ctx CRYPT_SHA2_256_Ctx;

/**
 * @ingroup SHA2_256
 * @brief Generate md context.
 *
 * @retval Success: sha256 ctx.
 *         Fails: NULL.
 */
CRYPT_SHA2_256_Ctx *CRYPT_SHA2_256_NewCtx(void);

/**
 * @ingroup SHA2_256
 * @brief Generate md context.
 *
 * @param libCtx [IN] library context
 * @param algId [IN] algorithm id
 *
 * @retval Success: sha256 ctx.
 *         Fails: NULL.
 */
CRYPT_SHA2_256_Ctx *CRYPT_SHA2_256_NewCtxEx(void *libCtx, int32_t algId);

/**
 * @ingroup SHA2_256
 * @brief free md context.
 *
 * @param ctx [IN] md handle
 */
void CRYPT_SHA2_256_FreeCtx(CRYPT_SHA2_256_Ctx *ctx);

/**
 * @defgroup CRYPT_SHA2_256_Init
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_256_Init(CRYPT_SHA2_256_Ctx *ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the SHA256 ctx for a digest operation.
 *
 * @par Description
 * CRYPT_SHA2_256_Init function initializes the ctx for
 * a digest operation. This function must be called before
 * CRYPT_SHA2_256_Update or CRYPT_SHA2_256_Final operations. This function will not
 * allocate memory for any of the ctx variables. Instead the caller is
 * expected to pass a ctx pointer pointing to a valid memory location
 * (either locally or dynamically allocated).
 *
 * @param[in] ctx The sha256 ctx
 * @param *param [in] Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS ctx is initialized
 * @retval #CRYPT_NULL_INPUT ctx is NULL
 */
int32_t CRYPT_SHA2_256_Init(CRYPT_SHA2_256_Ctx *ctx, BSL_Param *param);

/**
 * @defgroup CRYPT_SHA2_256_Update
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_256_Update(CRYPT_SHA2_256_Ctx *ctx, const uint8_t *data, usize_t nbytes)
 * @endcode
 *
 * @par Purpose
 * This is used to perform sha256 digest operation on chunks of data.
 *
 * @par Description
 * CRYPT_SHA2_256_Update function performs digest operation on
 * chunks of data. This method of digesting is used when data is
 * present in multiple buffers or not available all at once.
 * CRYPT_SHA2_256_Init must have been called before calling this
 * function.
 *
 * @param[in] ctx The sha256 ctx
 * @param[in] data The input data
 * @param[in] nbytes The input data length
 *
 * @retval #CRYPT_SUCCESS If partial digest is calculated
 * @retval #CRYPT_NULL_INPUT input arguments is NULL
 * @retval #CRYPT_SHA2_ERR_OVERFLOW input message is overflow
 */
int32_t CRYPT_SHA2_256_Update(CRYPT_SHA2_256_Ctx *ctx, const uint8_t *data, uint32_t nbytes);

/**
 * @defgroup CRYPT_SHA2_256_Final
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_256_Final(CRYPT_SHA2_256_Ctx *ctx, uint8_t *digest, uint32_t *len)
 * @endcode
 *
 * @par Purpose
 * This is used to complete sha256 digest operation on remaining data, and is
 * called at the end of digest operation.
 *
 * @par Description
 * CRYPT_SHA2_256_Final function completes digest operation on remaining data, and
 * is called at the end of digest operation.
 * CRYPT_SHA2_256_Init must have been called before calling this function. This
 * function calculates the digest. The memory for digest must
 * already have been allocated.
 *
 * @param[in] ctx The sha256 ctx
 * @param[out] digest The digest
 *
 * @retval #CRYPT_SUCCESS If partial digest is calculated
 * @retval #CRYPT_NULL_INPUT input arguments is NULL
 * @retval #CRYPT_SHA2_ERR_OVERFLOW input message is overflow
 * @retval #CRYPT_SHA2_OUT_BUFF_LEN_NOT_ENOUGH output buffer is not enough
 */
int32_t CRYPT_SHA2_256_Final(CRYPT_SHA2_256_Ctx *ctx, uint8_t *digest, uint32_t *outlen);

/**
 * @ingroup LLF Low Level Functions
 *
 * @brief SHA256 deinit function
 *
 * @param[in,out] ctx The SHA256 ctx
 *
 * @retval #CRYPT_SUCCESS       initialization succeeded.
 * @retval #CRYPT_NULL_INPUT    Pointer ctx is NULL
 */
int32_t CRYPT_SHA2_256_Deinit(CRYPT_SHA2_256_Ctx *ctx);

/**
 * @ingroup SHA256
 * @brief SHA256 copy CTX function
 * @param dst [out]  Pointer to the new SHA256 context.
 * @param src [in]   Pointer to the original SHA256 context.
 */
int32_t CRYPT_SHA2_256_CopyCtx(CRYPT_SHA2_256_Ctx *dst, const CRYPT_SHA2_256_Ctx *src);

/**
 * @ingroup SHA256
 * @brief SHA256 dup CTX function
 * @param src [in]   Pointer to the original SHA256 context.
 */
CRYPT_SHA2_256_Ctx *CRYPT_SHA2_256_DupCtx(const CRYPT_SHA2_256_Ctx *src);

#ifdef HITLS_CRYPTO_PROVIDER
/**
 * @ingroup SHA256
 * @brief SHA256 get param function
 * @param ctx [in]   Pointer to the SHA256 context.
 * @param param [in]   Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS       Success.
 * @retval #CRYPT_NULL_INPUT    Pointer param is NULL
 * @retval #CRYPT_INVALID_ARG   Pointer param is invalid
 */
int32_t CRYPT_SHA2_256_GetParam(CRYPT_SHA2_256_Ctx *ctx, BSL_Param *param);
#else
#define CRYPT_SHA2_256_GetParam NULL
#endif
#endif // HITLS_CRYPTO_SHA256

#ifdef HITLS_CRYPTO_SHA384

typedef struct CryptSha2512Ctx CRYPT_SHA2_384_Ctx;

#define CRYPT_SHA2_384_NewCtx CRYPT_SHA2_512_NewCtx
#define CRYPT_SHA2_384_NewCtxEx CRYPT_SHA2_512_NewCtxEx
#define CRYPT_SHA2_384_FreeCtx CRYPT_SHA2_512_FreeCtx
#define CRYPT_SHA2_384_Deinit CRYPT_SHA2_512_Deinit
#define CRYPT_SHA2_384_CopyCtx CRYPT_SHA2_512_CopyCtx
#define CRYPT_SHA2_384_DupCtx CRYPT_SHA2_512_DupCtx
#define CRYPT_SHA2_384_Update CRYPT_SHA2_512_Update
#define CRYPT_SHA2_384_Final CRYPT_SHA2_512_Final

/**
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_384_Init(CRYPT_SHA2_384_Ctx *ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the SHA384 ctx for a digest operation.
 *
 * @par Description
 * CRYPT_SHA2_384_Init function initializes the ctx for a digest operation. This function must be called before
 * CRYPT_SHA2_384_Update or CRYPT_SHA2_384_Final operations. This function will not allocate memory for any of the
 * ctx variables. Instead the caller is expected to pass a ctx pointer pointing to a valid memory location
 * (either locally or dynamically allocated).
 *
 * @param[in,out] ctx The sha384 ctx
 * @param *param [in] Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS ctx is initialized
 * @retval #CRYPT_NULL_INPUT ctx is NULL
 */
int32_t CRYPT_SHA2_384_Init(CRYPT_SHA2_384_Ctx *ctx, BSL_Param *param);

#ifdef HITLS_CRYPTO_PROVIDER
/**
 * @ingroup SHA512
 * @brief SHA512 get param function
 * @param ctx [in]   Pointer to the SHA512 context.
 * @param param [in]   Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS       Success.
 * @retval #CRYPT_NULL_INPUT    Pointer param is NULL
 * @retval #CRYPT_INVALID_ARG   Pointer param is invalid
 */
int32_t CRYPT_SHA2_384_GetParam(CRYPT_SHA2_384_Ctx *ctx, BSL_Param *param);
#else
#define CRYPT_SHA2_384_GetParam NULL
#endif

#endif // HITLS_CRYPTO_SHA384

#ifdef HITLS_CRYPTO_SHA512

typedef struct CryptSha2512Ctx CRYPT_SHA2_512_Ctx;

/**
 * @ingroup SHA2_512
 * @brief Generate md context.
 *
 * @retval Success: sha512 ctx.
 *         Fails: NULL.
 */
CRYPT_SHA2_512_Ctx *CRYPT_SHA2_512_NewCtx(void);

/**
 * @ingroup SHA2_512
 * @brief Generate md context.
 *
 * @param libCtx [IN] library context
 * @param algId [IN] algorithm id
 *
 * @retval Success: sha512 ctx.
 *         Fails: NULL.
 */
CRYPT_SHA2_512_Ctx *CRYPT_SHA2_512_NewCtxEx(void *libCtx, int32_t algId);

/**
 * @ingroup SHA2_512
 * @brief free md context.
 *
 * @param ctx [IN] md handle
 */
void CRYPT_SHA2_512_FreeCtx(CRYPT_SHA2_512_Ctx *ctx);

/**
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_512_Init(CRYPT_SHA2_512_Ctx *ctx)
 * @endcode
 *
 * @par Purpose
 * This is used to initialize the SHA512 ctx for a digest operation.
 *
 * @par Description
 * CRYPT_SHA2_512_Init function initializes the ctx for a digest operation. This function must be called before
 * CRYPT_SHA2_512_Update or CRYPT_SHA2_512_Final operations. This function will not allocate memory for any of the
 * ctx variable. Instead the caller is expected to pass a ctx pointer pointing to a valid memory location
 * (either locally or dynamically allocated).
 *
 * @param[in,out] ctx The sha512 ctx
 * @param *param [in] Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS ctx is initialized
 * @retval #CRYPT_NULL_INPUT ctx is NULL
 */
int32_t CRYPT_SHA2_512_Init(CRYPT_SHA2_512_Ctx *ctx, BSL_Param *param);

/**
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_512_Update(CRYPT_SHA2_512_Ctx *ctx, const uint8_t *data, usize_t nbytes)
 * @endcode
 *
 * @par Purpose
 * This is used to perform sha512 digest operation on chunks of data.
 *
 * @par Description
 * CRYPT_SHA2_512_Update function performs digest operation on chunks of data. This method of digesting is used when
 * data is present in multiple buffers or not available all at once. CRYPT_SHA2_512_Init must have been called before
 * calling this function.
 *
 * @param[in,out] ctx The sha512 ctx
 * @param[in] data The input data
 * @param[in] nbytes The input data length
 *
 * @retval #CRYPT_SUCCESS If partial digest is calculated
 * @retval #CRYPT_NULL_INPUT input arguments is NULL
 * @retval #CRYPT_SHA2_INPUT_OVERFLOW input message is overflow
 * @retval #CRYPT_SECUREC_FAIL secure c function fail.
 */
int32_t CRYPT_SHA2_512_Update(CRYPT_SHA2_512_Ctx *ctx, const uint8_t *data, uint32_t nbytes);

/**
 * @ingroup LLF Low Level Functions
 * @par Prototype
 * @code
 * int32_t CRYPT_SHA2_512_Final(CRYPT_SHA2_512_Ctx *ctx, uint8_t *digest, uint32_t *len)
 * @endcode
 *
 * @par Purpose
 * This is used to complete sha512 digest operation on remaining data, and is called at the end of digest operation.
 *
 * @par Description
 * CRYPT_SHA2_512_Final function completes digest operation on remaining data, and is called at the end of digest
 * operation. CRYPT_SHA2_512_Init must have been called before calling this function. This function calculates the
 * digest. The memory for digest must already have been allocated.
 *
 * @param[in,out] ctx The sha512 ctx
 * @param[out] digest The digest
 * @param[in,out] len length of buffer
 *
 * @retval #CRYPT_SUCCESS If partial digest is calculated
 * @retval #CRYPT_NULL_INPUT input arguments is NULL
 * @retval #CRYPT_SHA2_INPUT_OVERFLOW input message is overflow
 * @retval #CRYPT_SHA2_OUT_BUFF_LEN_NOT_ENOUGH output buffer is not enough
 */
int32_t CRYPT_SHA2_512_Final(CRYPT_SHA2_512_Ctx *ctx, uint8_t *digest, uint32_t *len);

/**
 * @ingroup LLF Low Level Functions
 *
 * @brief SHA512 deinit function
 *
 * @param[in,out] ctx The SHA512 ctx
 *
 * @retval #CRYPT_SUCCESS       initialization succeeded.
 * @retval #CRYPT_NULL_INPUT    Pointer ctx is NULL
 */
int32_t CRYPT_SHA2_512_Deinit(CRYPT_SHA2_512_Ctx *ctx);

/**
 * @ingroup SHA512
 * @brief SHA512 copy CTX function
 * @param dst [out]  Pointer to the new SHA512 context.
 * @param src [in]   Pointer to the original SHA512 context.
 */
int32_t CRYPT_SHA2_512_CopyCtx(CRYPT_SHA2_512_Ctx *dst, const CRYPT_SHA2_512_Ctx *src);

/**
 * @ingroup SHA512
 * @brief SHA512 dup CTX function
 * @param src [in]   Pointer to the original SHA512 context.
 */
CRYPT_SHA2_512_Ctx *CRYPT_SHA2_512_DupCtx(const CRYPT_SHA2_512_Ctx *src);

#ifdef HITLS_CRYPTO_PROVIDER
/**
 * @ingroup SHA512
 * @brief SHA512 get param function
 * @param ctx [in]   Pointer to the SHA512 context.
 * @param param [in]   Pointer to the parameter.
 *
 * @retval #CRYPT_SUCCESS       Success.
 * @retval #CRYPT_NULL_INPUT    Pointer param is NULL
 * @retval #CRYPT_INVALID_ARG   Pointer param is invalid
 */
int32_t CRYPT_SHA2_512_GetParam(CRYPT_SHA2_512_Ctx *ctx, BSL_Param *param);
#else
#define CRYPT_SHA2_512_GetParam NULL
#endif
#endif // HITLS_CRYPTO_SHA512

#ifdef __cplusplus
}
#endif

#endif // HITLS_CRYPTO_SHA2

#endif // CRYPT_SHA2_H
