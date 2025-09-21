/*@
  // 内存安全：确保 state 指针指向一个有效的、可写的8个uint32_t大小的内存区域
  requires \valid(state + (0..7));
  // 内存安全：只有在有数据块需要处理时，才要求 data 指针是可读的
  requires blockCnt >= 0 ==> \valid_read(data + (0 .. blockCnt * 64 - 1));
  // 内存安全：确保 state 和 data 指向的内存区域没有重叠，防止数据损坏
  requires \separated(state + (0..7), data + (0.. blockCnt * 64 - 1));
  // 内存安全：该函数唯一允许修改的内存是 state 数组
  assigns state[0..7];
*/
void SM3_Compress(uint32_t state[8], const uint8_t *data, uint32_t blockCnt){

}

