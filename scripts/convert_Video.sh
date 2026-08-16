data_og_path=$1
data_out_path=$2

input_dir="${data_og_path}/Video"
output_dir="${data_out_path}/Video"

# Create an output directory
mkdir ./$output_dir

FF_IN=" -loglevel error  "

#VP9_BITRATE=" -vf mpdecimate -minrate 10K -b:v 20K -maxrate 40K "
VP9_BITRATE=" -vf mpdecimate -minrate 20K -b:v 40K -maxrate 80K "
VP9_BITRATE_MANIA=" -vf mpdecimate -minrate 40K -b:v 80K -maxrate 160K "
VP9_FLAGS=" -tune-content default -quality best -speed 0
           -rc_lookahead 25 -row-mt 1 -auto-alt-ref 1
           -deadline best -lag-in-frames 25 -g 250
           -arnr-maxframes 15 -enable-tpl 1 -tile-columns 0 -frame-parallel 0 -aq-mode 0"
           
VID_RES="192:96"

#OUT_FPS="20"
OUT_FPS="12"
       
# Read files
vid_files=()
readarray -d '' vid_files < <(find ./$input_dir -maxdepth 1 -print0)
vid_files_count=${#vid_files[@]}

#Iterate through the directories
for ((i=1; i!=$vid_files_count; i++));
do
    cur_file=${vid_files[i]}
    base_cur_file=$(basename $cur_file)
    cur_file_out="${output_dir}/${base_cur_file}"
    echo "    "$cur_file_out

    OG_FPS=$(ffprobe -i $cur_file -v error -select_streams v -show_entries stream=r_frame_rate -of csv=p=0)

    if [ $base_cur_file == "Mania.ogv" ]; then
        echo "        Pass 1" 
        ffmpeg $FF_IN -i $cur_file -vf format=rgb24,scale=$VID_RES,format=yuv420p -fps_mode passthrough -f rawvideo - |	\
        ffmpeg $FF_IN -y -f rawvideo -s $VID_RES -r $OG_FPS -pix_fmt yuv420p                                			\
               -i - -r 18 -c:v libvpx-vp9 $VP9_BITRATE_MANIA $VP9_FLAGS -pass 1 -f ivf /dev/null
        echo "        Pass 2"  
        ffmpeg $FF_IN -i $cur_file -vf format=rgb24,scale=$VID_RES,format=yuv420p -fps_mode passthrough -f rawvideo - |	\
        ffmpeg $FF_IN -stats -y -f rawvideo -s $VID_RES -r $OG_FPS -pix_fmt yuv420p                         			\
               -i - -r 18 -c:v libvpx-vp9 $VP9_BITRATE_MANIA $VP9_FLAGS -pass 2 -f ivf $cur_file_out

    else
        echo "        Pass 1" 
        ffmpeg $FF_IN -i $cur_file -vf format=rgb24,scale=$VID_RES,format=yuv420p -fps_mode passthrough -f rawvideo - |	\
        ffmpeg $FF_IN -y -f rawvideo -s $VID_RES -r $OG_FPS -pix_fmt yuv420p                                			\
               -i - -r $OUT_FPS -c:v libvpx-vp9 $VP9_BITRATE $VP9_FLAGS -pass 1 -f ivf /dev/null
        echo "        Pass 2"  
        ffmpeg $FF_IN -i $cur_file -vf format=rgb24,scale=$VID_RES,format=yuv420p -fps_mode passthrough -f rawvideo - |	\
        ffmpeg $FF_IN -stats -y -f rawvideo -s $VID_RES -r $OG_FPS -pix_fmt yuv420p                         			\
               -i - -r $OUT_FPS -c:v libvpx-vp9 $VP9_BITRATE $VP9_FLAGS -pass 2 -f ivf $cur_file_out
    fi
done

rm ffmpeg2pass-0.log