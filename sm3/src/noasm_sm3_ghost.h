#include <stdint.h>

/*@
    // 建立 Ghost 模型，包含所有 SM3 运算的精确、具体的逻辑定义，
  axiomatic SM3Types {
    // 抽象的逻辑类型 SM3_State，用于表示8个32位寄存器
    type SM3_State;
    
    // 声明类型的访问函数:用于从 SM3_State 对象中获取每个寄存器的值
    logic integer SM3_State_a(SM3_State s);
    logic integer SM3_State_b(SM3_State s);
    logic integer SM3_State_c(SM3_State s);
    logic integer SM3_State_d(SM3_State s);
    logic integer SM3_State_e(SM3_State s);
    logic integer SM3_State_f(SM3_State s);
    logic integer SM3_State_g(SM3_State s);
    logic integer SM3_State_h(SM3_State s);

    // 构造函数:用于从8个整数值创建一个 SM3_State 对象
    logic SM3_State SM3_State_make(integer a, integer b, integer c, integer d,
                                   integer e, integer f, integer g, integer h);

    // 定义公理:关联构造器函数和访问函数
    axiom make_get_relation:
        \forall integer a, b, c, d, e, f, g, h;
            SM3_State_a(SM3_State_make(a, b, c, d, e, f, g, h)) == a &&
            SM3_State_b(SM3_State_make(a, b, c, d, e, f, g, h)) == b &&
            SM3_State_c(SM3_State_make(a, b, c, d, e, f, g, h)) == c &&
            SM3_State_d(SM3_State_make(a, b, c, d, e, f, g, h)) == d &&
            SM3_State_e(SM3_State_make(a, b, c, d, e, f, g, h)) == e &&
            SM3_State_f(SM3_State_make(a, b, c, d, e, f, g, h)) == f &&
            SM3_State_g(SM3_State_make(a, b, c, d, e, f, g, h)) == g &&
            SM3_State_h(SM3_State_make(a, b, c, d, e, f, g, h)) == h;
    }

    // 32位循环左移
    logic integer ROTL32_Logic(integer x, integer n) = ((x << n) | (x >> (32 - n))) & 0xFFFFFFFF;
    // 置换函数P0: 由ROTL32实现
    logic integer P0_Logic(integer x) = (x ^ ROTL32_Logic(x, 9) ^ ROTL32_Logic(x, 17)) & 0xFFFFFFFF;
    // 置换函数P1: 由ROTL32实现
    logic integer P1_Logic(integer x) = (x ^ ROTL32_Logic(x, 15) ^ ROTL32_Logic(x, 23)) & 0xFFFFFFFF;
    // 以下是SM3中的四个布尔函数FF和GG，FF函数拆分为FF0和FF1
    logic integer FF0_Logic(integer x, integer y, integer z) = (x ^ y ^ z) & 0xFFFFFFFF;
    logic integer FF1_Logic(integer x, integer y, integer z) = ((x & y) | (x & z) | (y & z)) & 0xFFFFFFFF;
    // GG函数拆分为GG0和GG1
    logic integer GG0_Logic(integer x, integer y, integer z) = (x ^ y ^ z) & 0xFFFFFFFF;
    logic integer GG1_Logic(integer x, integer y, integer z) = ((x & y) | (~x & z)) & 0xFFFFFFFF;
    logic integer EXPAND_Logic(integer W1, integer W2, integer W3,
                       integer W4, integer W5) =
    (P1_Logic(W1 ^ W2 ^ ROTL32_Logic(W3, 15)) ^ ROTL32_Logic(W4, 7) ^ W5) & 0xFFFFFFFF;


    // 将C语言的 uint32_t[8] 数组转换为 SM3_State 类型
    logic SM3_State State_from_array(uint32_t* s) =
        SM3_State_make(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);

    // 谓词：判断一个 SM3_State 逻辑值是否与一个C数组的值相等
    predicate State_equals_array(SM3_State ls, uint32_t* cs) =
        \valid_read(cs + (0..7)) &&
        SM3_State_a(ls) == cs[0] && SM3_State_b(ls) == cs[1] &&
        SM3_State_c(ls) == cs[2] && SM3_State_d(ls) == cs[3] &&
        SM3_State_e(ls) == cs[4] && SM3_State_f(ls) == cs[5] &&
        SM3_State_g(ls) == cs[6] && SM3_State_h(ls) == cs[7];

    // 逻辑函数：选择常量K
    logic integer K_logic(integer j) =
        j == 0  ? 0x79cc4519 : j == 1  ? 0xf3988a32 : j == 2  ? 0xe7311465 :
        j == 3  ? 0xce6228cb : j == 4  ? 0x9cc45197 : j == 5  ? 0x3988a32f :
        j == 6  ? 0x7311465e : j == 7  ? 0xe6228cbc : j == 8  ? 0xcc451979 :
        j == 9  ? 0x988a32f3 : j == 10 ? 0x311465e7 : j == 11 ? 0x6228cbce :
        j == 12 ? 0xc451979c : j == 13 ? 0x88a32f39 : j == 14 ? 0x11465e73 :
        j == 15 ? 0x228cbce6 : j == 16 ? 0x9d8a7a87 : j == 17 ? 0x3b14f50f :
        j == 18 ? 0x7629ea1e : j == 19 ? 0xec53d43c : j == 20 ? 0xd8a7a879 :
        j == 21 ? 0xb14f50f3 : j == 22 ? 0x629ea1e7 : j == 23 ? 0xc53d43ce :
        j == 24 ? 0x8a7a879d : j == 25 ? 0x14f50f3b : j == 26 ? 0x29ea1e76 :
        j == 27 ? 0x53d43cec : j == 28 ? 0xa7a879d8 : j == 29 ? 0x4f50f3b1 :
        j == 30 ? 0x9ea1e762 : j == 31 ? 0x3d43cec5 : j == 32 ? 0x7a879d8a :
        j == 33 ? 0xf50f3b14 : j == 34 ? 0xea1e7629 : j == 35 ? 0xd43cec53 :
        j == 36 ? 0xa879d8a7 : j == 37 ? 0x50f3b14f : j == 38 ? 0xa1e7629e :
        j == 39 ? 0x43cec53d : j == 40 ? 0x879d8a7a : j == 41 ? 0x0f3b14f5 :
        j == 42 ? 0x1e7629ea : j == 43 ? 0x3cec53d4 : j == 44 ? 0x79d8a7a8 :
        j == 45 ? 0xf3b14f50 : j == 46 ? 0xe7629ea1 : j == 47 ? 0xcec53d43 :
        j == 48 ? 0x9d8a7a87 : j == 49 ? 0x3b14f50f : j == 50 ? 0x7629ea1e :
        j == 51 ? 0xec53d43c : j == 52 ? 0xd8a7a879 : j == 53 ? 0xb14f50f3 :
        j == 54 ? 0x629ea1e7 : j == 55 ? 0xc53d43ce : j == 56 ? 0x8a7a879d :
        j == 57 ? 0x14f50f3b : j == 58 ? 0x29ea1e76 : j == 59 ? 0x53d43cec :
        j == 60 ? 0xa7a879d8 : j == 61 ? 0x4f50f3b1 : j == 62 ? 0x9ea1e762 :
        j == 63 ? 0x3d43cec5 : 0;

    // 读取大端序的32位整数
    logic integer get_uint32_be_logic(uint8_t* block, integer offset) =
    ((integer)block[offset]     << 24) |
    ((integer)block[offset + 1] << 16) |
    ((integer)block[offset + 2] << 8)  |
    ((integer)block[offset + 3]);

    // 递归定义消息扩展 Wj
    logic integer W_logic(integer j, uint8_t* block) =
        0 <= j <= 15 ? get_uint32_be_logic(block, j * 4) :
        16 <= j <= 67 ?
            (P1_Logic(W_logic(j - 16, block) ^ 
            W_logic(j - 9, block) ^ 
            ROTL32_Logic(W_logic(j - 3, block), 15)) ^
            ROTL32_Logic(W_logic(j - 13, block), 7) ^
            W_logic(j - 6, block)) & 0xFFFFFFFF
        : 0;
    // 递归定义消息扩展 Wj’
    logic integer W_prime_logic(integer j, uint8_t* block) =
        W_logic(j, block) ^ W_logic(j + 4, block);
    
    // 逻辑函数实现单轮压缩
    logic SM3_State SM3_Single_Round_Logic(SM3_State s, integer j, uint8_t* block) =
        \let a = SM3_State_a(s);
        \let b = SM3_State_b(s);
        \let c = SM3_State_c(s);
        \let d = SM3_State_d(s);
        \let e = SM3_State_e(s);
        \let f = SM3_State_f(s);
        \let g = SM3_State_g(s);
        \let h = SM3_State_h(s);
        \let a12 = ROTL32_Logic(a, 12);
        \let ss1 = ROTL32_Logic((a12 + e + K_logic(j)) & 0xFFFFFFFF, 7);
        \let ss2 = ss1 ^ a12;
        \let ff_val = (j < 16 ? FF0_Logic(a, b, c) : FF1_Logic(a, b, c));
        \let gg_val = (j < 16 ? GG0_Logic(e, f, g) : GG1_Logic(e, f, g));
        \let tt1 = (ff_val + d + ss2 + W_prime_logic(j, block)) & 0xFFFFFFFF;
        \let tt2 = (gg_val + h + ss1 + W_logic(j, block)) & 0xFFFFFFFF;
        SM3_State_make(               // 并行赋值，根据当前轮状态计算出下一轮状态
            tt1,                      // A <- TT1
            a,                        // B <- A
            ROTL32_Logic(b, 9),       // C <- ROTL(B, 9)
            c,                        // D <- C
            P0_Logic(tt2),            // E <- P0(TT2)
            e,                        // F <- E
            ROTL32_Logic(f, 19),      // G <- ROTL(F, 19)
            g                         // H <- G
        );

    // 64 轮迭代，递归实现
    logic SM3_State SM3_Permutation_Recursive(SM3_State s, uint8_t* block, integer j) =
        j == 64 ? s : SM3_Permutation_Recursive(SM3_Single_Round_Logic(s, j, block), block, j + 1);

    // 完整的单块压缩函数CF：64轮迭代 + 最终异或
    logic SM3_State SM3_Process_Block(SM3_State initial_s, uint8_t* block) =
        // 64轮迭代
        \let final_perm_state = SM3_Permutation_Recursive(initial_s, block, 0);
        // 最终异或 V(i+1) <- ABCDEFGH XOR V(i)
        SM3_State_make(
        (SM3_State_a(initial_s) ^ SM3_State_a(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_b(initial_s) ^ SM3_State_b(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_c(initial_s) ^ SM3_State_c(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_d(initial_s) ^ SM3_State_d(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_e(initial_s) ^ SM3_State_e(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_f(initial_s) ^ SM3_State_f(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_g(initial_s) ^ SM3_State_g(final_perm_state)) & 0xFFFFFFFF,
        (SM3_State_h(initial_s) ^ SM3_State_h(final_perm_state)) & 0xFFFFFFFF
        );
    
    // 多块数据的完整压缩函数CF
    logic SM3_State SM3_Compression_Full(SM3_State s, uint8_t* data, integer num_blocks) =
    num_blocks == 0 ? s :
    SM3_Compression_Full(SM3_Process_Block(s, data), data + 64, num_blocks - 1);


*/
