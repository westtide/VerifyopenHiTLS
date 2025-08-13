file(REMOVE_RECURSE
  "libhitls_bsl.dylib"
  "libhitls_bsl.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/hitls_bsl-shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
