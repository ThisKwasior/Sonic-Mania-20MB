data_og_path=$1
data_out_path=$2

input_dir="${data_og_path}/Meshes"
output_dir="${data_out_path}/Meshes"

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
    
    # Create an output directory for these meshes
    cur_dir_out="${output_dir}/${base_cur_dir}"
    echo $cur_dir_out
    mkdir ./$cur_dir_out
    
    #Iterate through the files
    for ((j=1; j!=$files_arr_len; j++));
    do
        cur_file=${files_arr[j]}
        base_cur_file=$(basename $cur_file)
        cur_file_out="${cur_dir_out}/${base_cur_file}"
        echo "    "$cur_file_out

        # Convert
        ./tools/mdl2emdl $cur_file $cur_file_out &

        # Parallel processing 
        parallel_proc=$((parallel_proc + 1))
        if [ $parallel_proc -eq 16 ]; then
            wait
            parallel_proc=0
        fi

    done
done

wait