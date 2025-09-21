#!/bin/bash

# ivette 是frama-c的GUI工具，若使用命令行模式，请使用frama-c -wp

ivette -wp \
  -rte \
  -wp-out ./_wp_out_sm3 \
  -wp-par 4 \
  -wp-timeout 60 \
  -wp-prover "z3,cvc5,alt-ergo" \
  -wp-split  \
  -wp-steps 50000 \
  -cpp-extra-args="-DHITLS_CRYPTO_SM3 -I ./bsl/err/include -I ./crypto/include -I ./config -I ./config/macro_config -I ./include -I ./include/bsl -I ./include/crypto  -I ./sm3/include" \
  sm3/src/noasm_sm3_ghost.h \
  sm3/src/noasm_sm3_count1.c

# -wp: 启用WP插件
# -rte: 检查运行时错误
# -wp-print: 打印生成的VC
# -wp-verbose: 设置WP插件的详细级别为
# -wp-out: 指定WP输出目录
# -wp-par: 使用k个并行进程进行验证
# -wp-timeout: 设置每个证明任务的超时时间(秒)，默认10秒，这里设为120秒
# -wp-steps: 设置SMT求解器的最大步数，默认值较小，这里设为50000
# -wp-prover: 指定使用的自动定理证明器
# -wp-split: 启用分割VC以提高证明效率
# -cpp-extra-args="...": 传递给C预处理器的额外参数，包括宏定义和包含路径
