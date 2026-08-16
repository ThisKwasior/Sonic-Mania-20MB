data_og_path=$1
data_out_path=$2

input_dir="${data_og_path}/Music"
output_dir="${data_out_path}/Music"

# Create an output directory
mkdir ./$output_dir

SAMPLERATE=$3
CHANNELS=$4
BITRATE=$5

OPUSENC_PARAMS=" --framesize 60 --music --bitrate $BITRATE --raw --raw-bits 8 --raw-rate $SAMPLERATE --raw-chan $CHANNELS "

# Read files
mus_files=()
readarray -d '' mus_files < <(find ./$input_dir -maxdepth 1 -print0)
mus_files_count=${#mus_files[@]}

parallel_proc=0

#Iterate through the directories
for ((i=1; i!=$mus_files_count; i++));
do
    cur_file=${mus_files[i]}
    base_cur_file=$(basename $cur_file)
    cur_file_out="${output_dir}/${base_cur_file}"
    echo "    "$cur_file_out

    # Convert
    ffmpeg -loglevel error -i $cur_file -ar $SAMPLERATE -f u8 -ac $CHANNELS - |                 \
    opusenc --quiet $OPUSENC_PARAMS - - |                                                       \
    ffmpeg -y -loglevel error -c:a libopus -i - -map_metadata -1 -c:a copy -f opus $cur_file_out &
    
    # Last resort
    #ffmpeg -loglevel error -i $cur_file -ar $SAMPLERATE -af acrusher=bits=1:samples=8:mode=log -f u8 -ac $CHANNELS - |  \
    #opusenc --quiet $OPUSENC_PARAMS - - |                                                                               \
    #ffmpeg -y -loglevel error -c:a libopus -i - -map_metadata -1 -c:a copy -f opus $cur_file_out
    
    # Parallel processing 
    parallel_proc=$((parallel_proc + 1))
    if [ $parallel_proc -eq 16 ]; then
        wait
        parallel_proc=0
    fi
done

wait