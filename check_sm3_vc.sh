#!/bin/bash

# 快速诊断脚本 - 生成证明条件并进行快速检查
# 用于检查生成了哪些VC以及它们的复杂程度

frama-c -wp \
  -rte \
  -wp-out ./_wp_out_sm3_check \
  -wp-print \
  -wp-timeout 5 \
  -wp-prover z3 \
  -cpp-extra-args="-DHITLS_CRYPTO_SM3 -I ./bsl/err/include -I ./crypto/include -I ./config -I ./config/macro_config -I ./include -I ./include/bsl -I ./include/crypto  -I ./sm3/include" \
  sm3/src/noasm_sm3_ghost.h \
  sm3/src/noasm_sm3_count1.c

echo "证明条件已生成到 _wp_out_sm3_check 目录"
echo "可以查看 .why 文件了解证明目标的复杂程度"
echo "使用短超时(5秒)快速识别困难的证明目标"

# 选项说明：
# -wp-print: 打印生成的证明条件
# -wp-timeout 5: 设置很短的超时时间，快速识别困难的证明目标