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

#ifndef UIO_UDP_H
#define UIO_UDP_H

#include "hitls_build.h"
#ifdef HITLS_BSL_UIO_UDP

#include "bsl_uio.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t UdpWrite(BSL_UIO *uio, const void *buf, uint32_t len, uint32_t *writeLen);
int32_t UdpRead(BSL_UIO *uio, void *buf, uint32_t len, uint32_t *readLen);

#ifdef __cplusplus
}
#endif

#endif /* HITLS_BSL_UIO_UDP */

#endif // UIO_UDP_H
