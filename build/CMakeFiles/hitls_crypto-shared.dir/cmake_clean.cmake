file(REMOVE_RECURSE
  "libhitls_crypto.dylib"
  "libhitls_crypto.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/hitls_crypto-shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
