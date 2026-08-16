set RELEASE_FLAGS=-O3 -s -flto
set DEBUG_FLAGS=-g3 -ggdb

set FLAGS=%DEBUG_FLAGS%

rem gcc src/mdl2emdl.c -I./src -o mdl2emdl %FLAGS%
rem gcc src/mdl2emdlv5.c -I./src -o mdl2emdl %FLAGS%
rem gcc src/vrle_encode.c -I./src -o vrle_encode %FLAGS%
rem gcc src/minizcomp.c src/miniz.c -I./src -o minizcomp %FLAGS%
rem 
rem gcc src/gif2egif.c src/kwaslib/crc32.c -I./src -o gif2egif %FLAGS%
rem gcc src/gif2epep.c -I./src -o gif2epep %FLAGS%
rem gcc src/gif_bitplanes.c -I./src -o gif_bitplanes %FLAGS%
rem gcc src/png2pep.c -I./src -o png2pep %FLAGS%
gcc src/gif_to_indices.c -I./src -o gif_to_indices %FLAGS%
gcc src/gif2gpng.c -I./src -o gif2gpng %FLAGS%

pause