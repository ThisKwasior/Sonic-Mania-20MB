data_og_path=$1
data_out_path=$2

# Create an output directory
mkdir ./$output_dir

# Convert
ffmpeg -loglevel error -y -i $data_og_path/Images/CESA.png           \
       -filter_complex "[0]palettegen=4[p],[0][p]paletteuse=none"   \
       $data_out_path/Images/CESA.png

ffmpeg -loglevel error -y -i $data_og_path/Images/TrueEnd.png        \
       -filter_complex "[0]palettegen=100[p],[0][p]paletteuse=none" \
       $data_out_path/Images/TrueEnd.png


ffmpeg -loglevel error -y -i $data_og_path/Images/CESA.tga           \
       -filter_complex "[0]palettegen=4[p],[0][p]paletteuse=none"   \
       $data_out_path/Images/CESA.png

ffmpeg -loglevel error -y -i $data_og_path/Images/TrueEnd.tga        \
       -filter_complex "[0]palettegen=100[p],[0][p]paletteuse=none" \
       $data_out_path/Images/TrueEnd.png

oxipng -o max -z --zi 50 --strip all $data_out_path/Images/CESA.png
oxipng -o max -z --zi 50 --strip all $data_out_path/Images/TrueEnd.png