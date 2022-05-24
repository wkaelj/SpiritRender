file(REMOVE_RECURSE
  "liblibs.pdb"
  "liblibs.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/libs.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
