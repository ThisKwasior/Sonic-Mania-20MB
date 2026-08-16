data_og_path=$1
data_out_path=$2

input_dir="${data_og_path}/SoundFX"
output_dir="${data_out_path}/SoundFX"

SAMPLERATE=$3
CHANNELS=$4
BITRATE=$5

OPUSENC_PARAMS=" --discard-comments --discard-pictures --framesize 60 --downmix-mono --music --bitrate $BITRATE --padding 0 --raw --raw-bits 8 --raw-rate $SAMPLERATE --raw-chan 1 "

# Read directories
directories=()
readarray -d '' directories < <(find ./$input_dir -maxdepth 1 -print0)
directories_count=${#directories[@]}

# Create an output directory
mkdir ./$output_dir

parallel_proc=0

#Iterate through the directories
for ((i=1; i!=$directories_count; i++));
do
    # Read SFX directory
    cur_dir=${directories[$i]}

    # Read all files in the dir
    files_arr=()
    readarray -d '' files_arr < <(find $cur_dir -maxdepth 1 -print0)
    files_arr_len=${#files_arr[@]}
 
    # Get the name of current directory
    base_cur_dir=$(basename $cur_dir)
    
    # Create an output directory for these SFX
    cur_dir_out="${output_dir}/${base_cur_dir}"
    echo $cur_dir_out
    mkdir ./$cur_dir_out

    # Check if VO directory
    if [ $base_cur_dir == "VO" ]; then
        opus_params_final=$OPUSENC_PARAMS" --speech"
    else
        opus_params_final=$OPUSENC_PARAMS" --music"
    fi

    #Iterate through the files
    for ((j=1; j!=$files_arr_len; j++));
    do
        cur_file=${files_arr[j]}
        base_cur_file=$(basename $cur_file)
        cur_file_out="${cur_dir_out}/${base_cur_file}"
        echo "    "$cur_file_out

        # Get audio codec for evil format hack
        audio_codec=$(ffprobe -i $cur_file -v error -select_streams a -show_entries stream=codec_name -of csv=p=0)
        ff_codec_hack=""
        
        # Check if we should hack the audio format
        if [ $audio_codec == "unknown" ]; then
            echo "        Unknown codec. Probably IEEE Float"
            ff_codec_hack="-c:a pcm_s16le"
        else
            echo "        Codec: " $audio_codec
        fi

        # Convert
        ffmpeg -loglevel error $ff_codec_hack -i $cur_file -ar $SAMPLERATE -f u8 -ac $CHANNELS - |  \
        opusenc --quiet $opus_params_final - - |                                                    \
        ffmpeg -y -loglevel error -c:a libopus -i - -map_metadata -1 -c:a copy -f opus $cur_file_out &

        # Parallel processing 
        parallel_proc=$((parallel_proc + 1))
        if [ $parallel_proc -eq 16 ]; then
            wait
            parallel_proc=0
        fi

    done
done

wait