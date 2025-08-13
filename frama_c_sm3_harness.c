#include <stddef.h>
#include <string.h>
#include "sm3/include/crypt_sm3.h"
#include "sm3/src/sm3_local.h" // For CRYPT_SM3_Ctx definition and struct access

// Include the ACSL annotations
#include "sm3_acsl.h"

// Dummy implementations for functions that are not part of the core logic
// but are needed for linking.
#include "bsl/bsl_err.h"
void BSL_ERR_PUSH_ERROR(long err){}

#include "securec.h"
errno_t memset_s(void *dest, size_t destMax, int c, size_t count) {
    if (dest == NULL || destMax < count) return -1;
    memset(dest, c, count);
    return 0;
}

errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count) {
    if (dest == NULL || src == NULL || destMax < count) return -1;
    memcpy(dest, src, count);
    return 0;
}

// Main harness function for Frama-C
void main_test_sm3() {
    CRYPT_SM3_Ctx ctx;
    unsigned char hash_output[CRYPT_SM3_DIGESTSIZE];
    uint32_t hash_len = CRYPT_SM3_DIGESTSIZE;

    const char* message1 = "abc";
    const char* message2 = "defghijklmnopqrstuvwxyz";

    // Initialize
    CRYPT_SM3_Init(&ctx, NULL);

    // Update with first message part
    CRYPT_SM3_Update(&ctx, (const uint8_t*)message1, strlen(message1));

    // Update with second message part
    CRYPT_SM3_Update(&ctx, (const uint8_t*)message2, strlen(message2));

    // Finalize
    CRYPT_SM3_Final(&ctx, hash_output, &hash_len);
}

// We need a main function for Frama-C to start analysis
int main() {
    main_test_sm3();
    return 0;
}
