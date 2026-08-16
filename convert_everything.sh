TOOLS_DIR=./tools
SCRIPT_DIR=./scripts

# Unpack the original data.rsdk
$TOOLS_DIR/RSDKv5Extract/RSDKv5Extract ./Data-rsdk/Data.rsdk $TOOLS_DIR/RSDKv5Extract/rsdk_files_list.txt ./Data-rsdk/Data-Original

#Create the output directory(-ies)
RSDK_OG="./Data-rsdk/Data-Original"
RSDK_SMALL="./Data-rsdk/Data-Small"

RSDK_OG_DIR="$RSDK_OG/Data"
RSDK_SMALL_DIR="$RSDK_SMALL/Data"

mkdir ./Data-rsdk/Data-Small $RSDK_SMALL_DIR
mkdir $RSDK_SMALL_DIR/Game $RSDK_SMALL_DIR/Images $RSDK_SMALL_DIR/Meshes $RSDK_SMALL_DIR/Music          \
      $RSDK_SMALL_DIR/Objects $RSDK_SMALL_DIR/Palettes $RSDK_SMALL_DIR/Shaders $RSDK_SMALL_DIR/SoundFX  \
      $RSDK_SMALL_DIR/Sprites $RSDK_SMALL_DIR/Stages $RSDK_SMALL_DIR/Strings $RSDK_SMALL_DIR/Video

# Version of RSDK
printf "5\0" > ./Data-rsdk/Data-Small/RSDKVer.txt

# Remove Plus contents
echo Removing Plus
$SCRIPT_DIR/remove_plus.sh $RSDK_OG_DIR

# Remove unused
echo Removing unused files
$SCRIPT_DIR/remove_unused.sh $RSDK_OG_DIR

# Game
echo Converting Game
cp -r $RSDK_OG_DIR/Game $RSDK_SMALL_DIR

# Images
echo Converting Images
$SCRIPT_DIR/convert_Images.sh $RSDK_OG_DIR $RSDK_SMALL_DIR

# Meshes
echo Converting Meshes
$SCRIPT_DIR/convert_Meshes.sh $RSDK_OG_DIR $RSDK_SMALL_DIR

# Music
echo Converting Music
$SCRIPT_DIR/convert_Music.sh $RSDK_OG_DIR $RSDK_SMALL_DIR 24000 1 16

# Objects
echo Converting Objects
cp -r $RSDK_OG_DIR/Objects $RSDK_SMALL_DIR

# Shaders
echo Converting Shaders
cp -r $RSDK_OG_DIR/Shaders $RSDK_SMALL_DIR

# SoundFX
echo Converting SoundFX
$SCRIPT_DIR/convert_SoundFX.sh $RSDK_OG_DIR $RSDK_SMALL_DIR 24000 1 24

# Sprites
echo Converting Sprites
$SCRIPT_DIR/convert_Sprites.sh $RSDK_OG_DIR $RSDK_SMALL_DIR

# Stages
echo Converting Stages
$SCRIPT_DIR/convert_Stages.sh $RSDK_OG_DIR $RSDK_SMALL_DIR

# Strings
echo Converting Strings
cp -r $RSDK_OG_DIR/Strings $RSDK_SMALL_DIR

# Video
echo Converting Video
$SCRIPT_DIR/convert_Video.sh $RSDK_OG_DIR $RSDK_SMALL_DIR

# Pack it all up
echo Packing the rsdk archive
$TOOLS_DIR/RSDKv5Pack/RSDKv5Pack "$RSDK_SMALL" ./Data-rsdk/Data_Small.rsdk

echo Compressing with zopfli
zopfli --i30 --zlib ./Data-rsdk/Data_Small.rsdk

# Done!
echo "Done!"