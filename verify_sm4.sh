#!/bin/bash

# ivette 是frama-c的GUI工具，若使用命令行模式，请使用frama-c -wp
ivette -wp \
  -rte \
  -wp-timeout 60 \
  -wp-split  \
  -wp-out ./_wp_out_sm4 \
  -wp-par 4 \
  -wp-prover z3,cvc5,alt-ergo \
  -cpp-extra-args="-DHITLS_CRYPTO_SM4 -I ./bsl/err/include -I ./config  -I ./crypto/include -I ./config/macro_config -I ./include -I ./include/bsl -I ./include/crypto -I ./sm4/include -I ./platform/Secure_C/include"  \
  sm4/src/crypt_sm4.c

# -wp: 启用WP插件
# -rte: 检查运行时错误
# -wp-print: 打印生成的VC
# -wp-verbose: 设置WP插件的详细级别为
# -wp-out: 指定WP输出目录
# -wp-par: 使用k个并行进程进行验证
# -wp-timeout: 设置每个证明任务的超时时间
# -wp-prover: 指定使用的自动定理证明器
# -wp-split: 启用分割VC以提高证明效率
# -cpp-extra-args="...": 传递给C预处理器的额外参数，包括宏定义和包含路径