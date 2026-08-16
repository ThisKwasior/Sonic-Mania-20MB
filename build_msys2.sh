TARGET=x86_64-w64-mingw32-
CC=${TARGET}gcc
CXX=${TARGET}g++
AR=ar
WINDRES=windres
STRIP=strip
LD=ld

DATA_DIR="./Data-rsdk"
DEPS_DIR="./deps"
RSDK_DIR="./Sonic-Mania-Decompilation/dependencies/RSDKv5"
MANIA_DIR="./Sonic-Mania-Decompilation"

OGG_SOURCES=(
    $DEPS_DIR/libogg/src/framing.c
    $DEPS_DIR/libogg/src/bitwise.c
)

OPUS_SOURCES=(
    # opus
    $DEPS_DIR/opus/src/opus.c
    $DEPS_DIR/opus/src/opus_decoder.c
    $DEPS_DIR/opus/src/opus_encoder.c
    $DEPS_DIR/opus/src/extensions.c
    $DEPS_DIR/opus/src/opus_multistream.c
    $DEPS_DIR/opus/src/opus_multistream_encoder.c
    $DEPS_DIR/opus/src/opus_multistream_decoder.c
    $DEPS_DIR/opus/src/repacketizer.c
    $DEPS_DIR/opus/src/opus_projection_encoder.c
    $DEPS_DIR/opus/src/opus_projection_decoder.c
    $DEPS_DIR/opus/src/mapping_matrix.c
    
    #silk fixed-point
    $DEPS_DIR/opus/silk/CNG.c
    $DEPS_DIR/opus/silk/code_signs.c
    $DEPS_DIR/opus/silk/init_decoder.c
    $DEPS_DIR/opus/silk/decode_core.c
    $DEPS_DIR/opus/silk/decode_frame.c
    $DEPS_DIR/opus/silk/decode_parameters.c
    $DEPS_DIR/opus/silk/decode_indices.c
    $DEPS_DIR/opus/silk/decode_pulses.c
    $DEPS_DIR/opus/silk/decoder_set_fs.c
    $DEPS_DIR/opus/silk/dec_API.c
    $DEPS_DIR/opus/silk/enc_API.c
    $DEPS_DIR/opus/silk/encode_indices.c
    $DEPS_DIR/opus/silk/encode_pulses.c
    $DEPS_DIR/opus/silk/gain_quant.c
    $DEPS_DIR/opus/silk/interpolate.c
    $DEPS_DIR/opus/silk/LP_variable_cutoff.c
    $DEPS_DIR/opus/silk/NLSF_decode.c
    $DEPS_DIR/opus/silk/NSQ.c
    $DEPS_DIR/opus/silk/NSQ_del_dec.c
    $DEPS_DIR/opus/silk/PLC.c
    $DEPS_DIR/opus/silk/shell_coder.c
    $DEPS_DIR/opus/silk/tables_gain.c
    $DEPS_DIR/opus/silk/tables_LTP.c
    $DEPS_DIR/opus/silk/tables_NLSF_CB_NB_MB.c
    $DEPS_DIR/opus/silk/tables_NLSF_CB_WB.c
    $DEPS_DIR/opus/silk/tables_other.c
    $DEPS_DIR/opus/silk/tables_pitch_lag.c
    $DEPS_DIR/opus/silk/tables_pulses_per_block.c
    $DEPS_DIR/opus/silk/VAD.c
    $DEPS_DIR/opus/silk/control_audio_bandwidth.c
    $DEPS_DIR/opus/silk/quant_LTP_gains.c
    $DEPS_DIR/opus/silk/VQ_WMat_EC.c
    $DEPS_DIR/opus/silk/HP_variable_cutoff.c
    $DEPS_DIR/opus/silk/NLSF_encode.c
    $DEPS_DIR/opus/silk/NLSF_VQ.c
    $DEPS_DIR/opus/silk/NLSF_unpack.c
    $DEPS_DIR/opus/silk/NLSF_del_dec_quant.c
    $DEPS_DIR/opus/silk/process_NLSFs.c
    $DEPS_DIR/opus/silk/stereo_LR_to_MS.c
    $DEPS_DIR/opus/silk/stereo_MS_to_LR.c
    $DEPS_DIR/opus/silk/check_control_input.c
    $DEPS_DIR/opus/silk/control_SNR.c
    $DEPS_DIR/opus/silk/init_encoder.c
    $DEPS_DIR/opus/silk/control_codec.c
    $DEPS_DIR/opus/silk/A2NLSF.c
    $DEPS_DIR/opus/silk/ana_filt_bank_1.c
    $DEPS_DIR/opus/silk/biquad_alt.c
    $DEPS_DIR/opus/silk/bwexpander_32.c
    $DEPS_DIR/opus/silk/bwexpander.c
    $DEPS_DIR/opus/silk/debug.c
    $DEPS_DIR/opus/silk/decode_pitch.c
    $DEPS_DIR/opus/silk/inner_prod_aligned.c
    $DEPS_DIR/opus/silk/lin2log.c
    $DEPS_DIR/opus/silk/log2lin.c
    $DEPS_DIR/opus/silk/LPC_analysis_filter.c
    $DEPS_DIR/opus/silk/LPC_inv_pred_gain.c
    $DEPS_DIR/opus/silk/table_LSF_cos.c
    $DEPS_DIR/opus/silk/NLSF2A.c
    $DEPS_DIR/opus/silk/NLSF_stabilize.c
    $DEPS_DIR/opus/silk/NLSF_VQ_weights_laroia.c
    $DEPS_DIR/opus/silk/pitch_est_tables.c
    $DEPS_DIR/opus/silk/resampler.c
    $DEPS_DIR/opus/silk/resampler_down2_3.c
    $DEPS_DIR/opus/silk/resampler_down2.c
    $DEPS_DIR/opus/silk/resampler_private_AR2.c
    $DEPS_DIR/opus/silk/resampler_private_down_FIR.c
    $DEPS_DIR/opus/silk/resampler_private_IIR_FIR.c
    $DEPS_DIR/opus/silk/resampler_private_up2_HQ.c
    $DEPS_DIR/opus/silk/resampler_rom.c
    $DEPS_DIR/opus/silk/sigm_Q15.c
    $DEPS_DIR/opus/silk/sort.c
    $DEPS_DIR/opus/silk/sum_sqr_shift.c
    $DEPS_DIR/opus/silk/stereo_decode_pred.c
    $DEPS_DIR/opus/silk/stereo_encode_pred.c
    $DEPS_DIR/opus/silk/stereo_find_predictor.c
    $DEPS_DIR/opus/silk/stereo_quant_pred.c
    $DEPS_DIR/opus/silk/LPC_fit.c
    $DEPS_DIR/opus/silk/fixed/LTP_analysis_filter_FIX.c
    $DEPS_DIR/opus/silk/fixed/LTP_scale_ctrl_FIX.c
    $DEPS_DIR/opus/silk/fixed/corrMatrix_FIX.c
    $DEPS_DIR/opus/silk/fixed/encode_frame_FIX.c
    $DEPS_DIR/opus/silk/fixed/find_LPC_FIX.c
    $DEPS_DIR/opus/silk/fixed/find_LTP_FIX.c
    $DEPS_DIR/opus/silk/fixed/find_pitch_lags_FIX.c
    $DEPS_DIR/opus/silk/fixed/find_pred_coefs_FIX.c
    $DEPS_DIR/opus/silk/fixed/noise_shape_analysis_FIX.c
    $DEPS_DIR/opus/silk/fixed/process_gains_FIX.c
    $DEPS_DIR/opus/silk/fixed/regularize_correlations_FIX.c
    $DEPS_DIR/opus/silk/fixed/residual_energy16_FIX.c
    $DEPS_DIR/opus/silk/fixed/residual_energy_FIX.c
    $DEPS_DIR/opus/silk/fixed/warped_autocorrelation_FIX.c
    $DEPS_DIR/opus/silk/fixed/apply_sine_window_FIX.c
    $DEPS_DIR/opus/silk/fixed/autocorr_FIX.c
    $DEPS_DIR/opus/silk/fixed/burg_modified_FIX.c
    $DEPS_DIR/opus/silk/fixed/k2a_FIX.c
    $DEPS_DIR/opus/silk/fixed/k2a_Q16_FIX.c
    $DEPS_DIR/opus/silk/fixed/pitch_analysis_core_FIX.c
    $DEPS_DIR/opus/silk/fixed/vector_ops_FIX.c
    $DEPS_DIR/opus/silk/fixed/schur64_FIX.c
    $DEPS_DIR/opus/silk/fixed/schur_FIX.c
    
    # celt
    $DEPS_DIR/opus/celt/bands.c
    $DEPS_DIR/opus/celt/celt.c
    $DEPS_DIR/opus/celt/celt_encoder.c
    $DEPS_DIR/opus/celt/celt_decoder.c
    $DEPS_DIR/opus/celt/cwrs.c
    $DEPS_DIR/opus/celt/entcode.c
    $DEPS_DIR/opus/celt/entdec.c
    $DEPS_DIR/opus/celt/entenc.c
    $DEPS_DIR/opus/celt/kiss_fft.c
    $DEPS_DIR/opus/celt/laplace.c
    $DEPS_DIR/opus/celt/mathops.c
    $DEPS_DIR/opus/celt/mdct.c
    $DEPS_DIR/opus/celt/modes.c
    $DEPS_DIR/opus/celt/pitch.c
    $DEPS_DIR/opus/celt/celt_lpc.c
    $DEPS_DIR/opus/celt/quant_bands.c
    $DEPS_DIR/opus/celt/rate.c
    $DEPS_DIR/opus/celt/vq.c
    
    # opusfile
    $DEPS_DIR/opusfile/src/http.c
    $DEPS_DIR/opusfile/src/info.c
    $DEPS_DIR/opusfile/src/internal.c
    $DEPS_DIR/opusfile/src/opusfile.c
    $DEPS_DIR/opusfile/src/stream.c
)

OPUS_FLAGS=(
    -DOPUS_BUILD
    -DUSE_ALLOCA
    -DFIXED_POINT=1
    -DDISABLE_FLOAT_API
    -DOP_FIXED_POINT=1
    
    -Wno-parentheses
    -Wno-long-long
)

VP9_SOURCES=(
    $DEPS_DIR/libvpx/vp9/common/vp9_alloccommon.c
    $DEPS_DIR/libvpx/vp9/common/vp9_blockd.c
    $DEPS_DIR/libvpx/vp9/common/vp9_common_data.c
    $DEPS_DIR/libvpx/vp9/common/vp9_entropy.c
    $DEPS_DIR/libvpx/vp9/common/vp9_entropymode.c
    $DEPS_DIR/libvpx/vp9/common/vp9_entropymv.c
    $DEPS_DIR/libvpx/vp9/common/vp9_filter.c
    $DEPS_DIR/libvpx/vp9/common/vp9_frame_buffers.c
    $DEPS_DIR/libvpx/vp9/common/vp9_idct.c
    $DEPS_DIR/libvpx/vp9/common/vp9_loopfilter.c
    $DEPS_DIR/libvpx/vp9/common/vp9_mvref_common.c
    $DEPS_DIR/libvpx/vp9/common/vp9_pred_common.c
    $DEPS_DIR/libvpx/vp9/common/vp9_quant_common.c
    $DEPS_DIR/libvpx/vp9/common/vp9_reconinter.c
    $DEPS_DIR/libvpx/vp9/common/vp9_reconintra.c
    $DEPS_DIR/libvpx/vp9/common/vp9_rtcd.c
    $DEPS_DIR/libvpx/vp9/common/vp9_scale.c
    $DEPS_DIR/libvpx/vp9/common/vp9_scan.c
    $DEPS_DIR/libvpx/vp9/common/vp9_seg_common.c
    $DEPS_DIR/libvpx/vp9/common/vp9_thread_common.c
    $DEPS_DIR/libvpx/vp9/common/vp9_tile_common.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_decodeframe.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_decodemv.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_decoder.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_detokenize.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_dsubexp.c
    $DEPS_DIR/libvpx/vp9/decoder/vp9_job_queue.c
    $DEPS_DIR/libvpx/vp9/vp9_dx_iface.c
    $DEPS_DIR/libvpx/vp9/vp9_iface_common.c
    $DEPS_DIR/libvpx/vpx/src/vpx_codec.c
    $DEPS_DIR/libvpx/vpx/src/vpx_decoder.c
    $DEPS_DIR/libvpx/vpx/src/vpx_encoder.c
    $DEPS_DIR/libvpx/vpx/src/vpx_image.c
    $DEPS_DIR/libvpx/vpx_dsp/bitreader.c
    $DEPS_DIR/libvpx/vpx_dsp/bitreader_buffer.c
    $DEPS_DIR/libvpx/vpx_dsp/intrapred.c
    $DEPS_DIR/libvpx/vpx_dsp/inv_txfm.c
    $DEPS_DIR/libvpx/vpx_dsp/loopfilter.c
    $DEPS_DIR/libvpx/vpx_dsp/prob.c
    $DEPS_DIR/libvpx/vpx_dsp/skin_detection.c
    $DEPS_DIR/libvpx/vpx_dsp/vpx_convolve.c
    $DEPS_DIR/libvpx/vpx_dsp/vpx_dsp_rtcd.c
    $DEPS_DIR/libvpx/vpx_mem/vpx_mem.c
    $DEPS_DIR/libvpx/vpx_scale/generic/gen_scalers.c
    $DEPS_DIR/libvpx/vpx_scale/generic/vpx_scale.c
    $DEPS_DIR/libvpx/vpx_scale/generic/yv12config.c
    $DEPS_DIR/libvpx/vpx_scale/generic/yv12extend.c
    $DEPS_DIR/libvpx/vpx_scale/vpx_scale_rtcd.c
    $DEPS_DIR/libvpx/vpx_util/vpx_thread.c
    $DEPS_DIR/libvpx/vpx_util/vpx_write_yuv_frame.c
    
    $DEPS_DIR/libvpx/vpx_config.c
    $DEPS_DIR/libvpx/tools_common.c
)

VP9_SOURCES_CXX=(
    $DEPS_DIR/libvpx/video_reader.cpp
    $DEPS_DIR/libvpx/ivfdec.cpp
)

VP9_FLAGS=(
    
)

RSDK_SOURCES=(
    $RSDK_DIR/RSDKv5/main.cpp
    $RSDK_DIR/RSDKv5/RSDK/Audio/Audio.cpp
    $RSDK_DIR/RSDKv5/RSDK/Core/Link.cpp
    $RSDK_DIR/RSDKv5/RSDK/Core/Math.cpp
    #$RSDK_DIR/RSDKv5/RSDK/Core/ModAPI.cpp
    $RSDK_DIR/RSDKv5/RSDK/Core/Reader.cpp
    $RSDK_DIR/RSDKv5/RSDK/Core/RetroEngine.cpp
    $RSDK_DIR/RSDKv5/RSDK/Dev/Debug.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Animation.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Drawing.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Palette.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Scene3D.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Sprite.cpp
    $RSDK_DIR/RSDKv5/RSDK/Graphics/Video.cpp
    $RSDK_DIR/RSDKv5/RSDK/Input/Input.cpp
    $RSDK_DIR/RSDKv5/RSDK/Scene/Collision.cpp
    $RSDK_DIR/RSDKv5/RSDK/Scene/Object.cpp
    $RSDK_DIR/RSDKv5/RSDK/Scene/Objects/DefaultObject.cpp
    $RSDK_DIR/RSDKv5/RSDK/Scene/Objects/DevOutput.cpp
    $RSDK_DIR/RSDKv5/RSDK/Scene/Scene.cpp
    $RSDK_DIR/RSDKv5/RSDK/Storage/Storage.cpp
    $RSDK_DIR/RSDKv5/RSDK/Storage/Text.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserAchievements.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserCore.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserLeaderboards.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserPresence.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserStats.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Core/UserStorage.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyAchievements.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyCore.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyLeaderboards.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyPresence.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyStats.cpp
    $RSDK_DIR/RSDKv5/RSDK/User/Dummy/DummyStorage.cpp
)

GAME_SOURCES=(
    $MANIA_DIR/SonicMania/Game.c
    $MANIA_DIR/SonicMania/Objects/All.c
)

RSDK_SOURCES_DEPS=(
    $RSDK_DIR/dependencies/all/miniz/miniz.c
    $RSDK_DIR/dependencies/all/iniparser/dictionary.cpp
    $RSDK_DIR/dependencies/all/iniparser/iniparser.cpp
    #$RSDK_DIR/dependencies/all/tinyxml2/tinyxml2.cpp
    #$RSDK_DIR/dependencies/all/stb_vorbis/std_vorbis.c
)

GAME_INCLUDES=(
    -I$MANIA_DIR/SonicMania
    -I$MANIA_DIR/SonicMania/Objects
)

RSDK_INCLUDES=(
    -I$RSDK_DIR/RSDKv5
    -I$RSDK_DIR/dependencies/all
    -I$DEPS_DIR/libogg/include
    -I$DEPS_DIR/libtheora/include
    -I$DEPS_DIR/opus
    -I$DEPS_DIR/opus/include
    -I$DEPS_DIR/opus/celt
    -I$DEPS_DIR/opus/silk
    -I$DEPS_DIR/opus/silk/fixed
    -I$DEPS_DIR/opusfile/include
    -I$DEPS_DIR/libvpx
    -I$DEPS_DIR/libvpx/vp9
    -I$DEPS_DIR/libvpx/vpx
    -I$DEPS_DIR/libvpx/vpx_dsp
    -I$DEPS_DIR/libvpx/vpx_mem
    -I$DEPS_DIR/libvpx/vpx_ports
    -I$DEPS_DIR/libvpx/vpx_scale
    -I$DEPS_DIR/libvpx/vpx_util
)

GAME_DEFINES=(
    -DRETRO_REVISION=2
    -DRETRO_USE_MOD_LOADER=0
    -DRETRO_MOD_LOADER_VER=2
    -DGAME_INCLUDE_EDITOR=0
    -DMANIA_PREPLUS=0
    -DMANIA_FIRST_RELEASE=0
    -DGAME_VERSION=6
)

RSDK_DEFINES=(
    -DRETRO_DISABLE_LOG=0
    -DRETRO_MOD_LOADER_VER=2
    -DRETRO_REVISION=2
    -DRETRO_STANDALONE=0
    -DRETRO_USE_MOD_LOADER=0
    -DRSDK_AUTOBUILD=1 # Disables Plus
    -DRSDK_USE_DX9=1
    
    # To make miniz less chonky
    -DMINIZ_NO_ARCHIVE_APIS=0
    -DMINIZ_NO_STDIO=0
    -DMINIZ_NO_ARCHIVE_WRITING_APIS=0
    -DMINIZ_NO_TIME=0
	
	# Misc
	#-D
)

RSDK_FLAGS=(
    -mwindows
    -DRETRO_EMBEDDED_DATA
)

MANIA_INCLUDES=(
    ${RSDK_INCLUDES[@]}
    ${GAME_INCLUDES[@]}
)

MANIA_C_FLAGS=(
    -fpermissive
    -Wbuiltin-macro-redefined
    #--no-warnings
    
    #Optimization
    -Oz
    -fdata-sections
    -ffunction-sections
    -flto
    -fno-unroll-loops
    -fno-math-errno
    -fwrapv
    -fomit-frame-pointer
    -fno-asynchronous-unwind-tables
    -fwhole-file
    #-msse4
    -fmerge-constants
    -g3
    -ggdb
)

MANIA_LINK_FLAGS=(
    -Wl,--gc-sections
    -flto
    #-s
    -fmerge-constants
)

MANIA_C_DEFINES=(
    -DNDEBUG
    -D_WIN32_WINNT=0x0501
    -DWINVER=0x0501
)

MANIA_CXX_FLAGS=(
    ${MANIA_C_FLAGS[@]}
    -fno-rtti
    -fno-exceptions
    -std=c++17
    -g3
    -ggdb
)

MANIA_CXX_DEFINES=(
    ${MANIA_C_DEFINES[@]}
)

MANIA_LIBS=(
    -ld3d9
    -ld3dcompiler
    -lXInput
    -lksguid
    -lksuser
    -lxaudio2_9
    -lole32
    -lgdi32
    -lcomctl32
    -lwinmm
)

# Create bin and obj dirs and clean obj
BIN_DIR="./bin"
OBJ_DIR="./obj"
mkdir $BIN_DIR
mkdir $OBJ_DIR
mkdir $OBJ_DIR/ogg
mkdir $OBJ_DIR/opus
mkdir $OBJ_DIR/libvpx
mkdir $OBJ_DIR/game
#rm $OBJ_DIR/*.o
#rm $OBJ_DIR/ogg/*.o
#rm $OBJ_DIR/opus/*.o
#rm $OBJ_DIR/libvpx/*.o
#rm $OBJ_DIR/game/*.o

# Build vp9
for src in ${VP9_SOURCES[@]}; do
    name=$(basename $src)
    echo "[CC]  "$name
    $CC -c ${src} -o $OBJ_DIR/libvpx/$name".o" ${MANIA_INCLUDES[@]} ${MANIA_C_FLAGS[@]} ${MANIA_C_DEFINES[@]} ${VP9_FLAGS[@]}
done;

for src in ${VP9_SOURCES_CXX[@]}; do
    name=$(basename $src)
    echo "[CXX] "$name
    $CXX -c ${src} -o $OBJ_DIR/libvpx/$name".o" ${MANIA_INCLUDES[@]} ${MANIA_CXX_FLAGS[@]} ${MANIA_CXX_DEFINES[@]} ${VP9_FLAGS[@]} ${RSDK_DEFINES[@]}
done;

# Build ogg
for src in ${OGG_SOURCES[@]}; do
    name=$(basename $src)
    echo "[CC]  "$name
    $CC -c ${src} -o $OBJ_DIR/ogg/$name".o" ${MANIA_INCLUDES[@]} ${MANIA_C_FLAGS[@]} ${MANIA_C_DEFINES[@]}
done;

# Build opus
for src in ${OPUS_SOURCES[@]}; do
    name=$(basename $src)
    echo "[CC]  "$name
    $CC -c ${src} -o $OBJ_DIR/opus/$name".o" ${MANIA_INCLUDES[@]} ${MANIA_C_FLAGS[@]} ${MANIA_C_DEFINES[@]} ${OPUS_FLAGS[@]}
done;

# Build Sonic Mania
for src in ${GAME_SOURCES[@]}; do
    name=$(basename $src)
    echo "[CC]  "$name
    $CC -c ${src} -o $OBJ_DIR/game/$name".o" ${MANIA_INCLUDES[@]} ${GAME_DEFINES[@]} ${MANIA_C_FLAGS[@]} ${MANIA_C_DEFINES[@]}
done;

# Compile resources
$WINDRES ./mania10res/SonicMania10.rc -O coff $OBJ_DIR/res.rc.o

$LD -r -b binary ${DATA_DIR}/Data_Small.rsdk.zlib -o ${DATA_DIR}/data_rsdk.o

data_rsdk_symbols=()
IFS=$'\n' read -r -d '' -a data_rsdk_symbols < <( nm -j ${DATA_DIR}/data_rsdk.o && printf '\0' )

printf "#pragma once\n" > $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "#include <stdint.h>\n" >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "extern const int8_t %s[];\n" ${data_rsdk_symbols[0]} >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "extern const int8_t %s[];\n" ${data_rsdk_symbols[2]} >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h

printf "#define DATA_START %s\n" ${data_rsdk_symbols[2]} >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "#define DATA_END %s\n" ${data_rsdk_symbols[0]} >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "#define DATA_REAL_SIZE %u\n" $(stat -c %s ${DATA_DIR}/Data_Small.rsdk) >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h
printf "#define DATA_ZLIB_SIZE %u\n" $(stat -c %s ${DATA_DIR}/Data_Small.rsdk.zlib) >> $RSDK_DIR/RSDKv5/RSDK/Tenmb/rsdk_zlib.h

# Build RSDKv5
for src in ${RSDK_SOURCES[@]} ${RSDK_SOURCES_DEPS[@]}; do
    name=$(basename $src)
    echo "[CXX] "$name
    $CXX -c ${src} -o $OBJ_DIR/game/$name".o" ${RSDK_DEFINES[@]} ${RSDK_FLAGS[@]} ${MANIA_CXX_FLAGS[@]} ${MANIA_INCLUDES[@]}
done;

# Get current time
CUR_TIME=$(date '+%F_%H_%M_%S')

# Link everything
TARGET_FILE=${BIN_DIR}/RSDKv5_$CUR_TIME.exe

$CXX $OBJ_DIR/*.o $OBJ_DIR/ogg/*.o $OBJ_DIR/libvpx/*.o $OBJ_DIR/opus/*.o $OBJ_DIR/game/*.o  \
     ${DATA_DIR}/data_rsdk.o -o ${TARGET_FILE}                                              \
     ${RSDK_FLAGS[@]} ${MANIA_LIBS[@]} ${MANIA_LINK_FLAGS[@]} -static -static-libstdc++ -lpthread
     
$STRIP ${TARGET_FILE}