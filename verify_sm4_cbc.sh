ivette -wp -eva -rte\
    sm4/src/noasm_sm4_cbc.c \
    sm4/src/crypt_sm4.c \
    sm4/src/sm4_key.c \
    -cpp-extra-args="\
    -DHITLS_CRYPTO_SM4 \
    -DHITLS_CRYPTO_CBC \
    -DHITLS_CRYPTO_MODES \
    -I ./sm4/include/ \
    -I ./include/bsl \
    -I ./include/crypto \
    -I ./bsl/err/include \
    -I ./config/macro_config \
    -I ./config \
    -I ./include \
    -I ./crypto/modes/include \
    -I ./crypto/modes/src \
    -I ./crypto/include \
    -I ./crypto/eal/src \
    -I ./platform/Secure_C/include" \
    -wp-out ./_wp_out_sm4_cbc \
    -wp-par 4 \
    -wp-timeout 60\
    -wp-prover "z3,cvc5,alt-ergo" \