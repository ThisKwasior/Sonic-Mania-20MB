# Sonic-Mania-20MB
RSDKv5 and Sonic Mania decompilation modified to make the entire game fit in 20MB (data embedded in executable).

# How to build
You will need:
  - [MSYS2](https://www.msys2.org/)
  - `Data.rsdk` from your copy of Sonic Mania (Plus update)
  
After installing, you will need to run the UCRT64 terminal.

Update all packages with:
```
pacman -Syu
```
It should prompt you to restart the terminal. Do so and run UCRT64 terminal again.

Now we will install all of the required packages:
```
pacman -Syu gcc git mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-directx-headers mingw-w64-ucrt-x86_64-headers mingw-w64-ucrt-x86_64-zopfli mingw-w64-ucrt-x86_64-oxipng mingw-w64-ucrt-x86_64-ffmpeg mingw-w64-ucrt-x86_64-opus-tools
```

Now, we gotta clone the repository.
Navigate to a folder that's easily accessible. For this we'll build in `C:\mania20`
```
mkdir /c/mania20
cd /c/mania20
git clone https://github.com/ThisKwasior/Sonic-Mania-20MB.git
cd Sonic-Mania-20MB
```

Copy `Data.rsdk` into the `Data-rsdk` folder.

Now we can build.
Run this command to compress the Data file:
```
./convert_everything.sh
```
After around 20 minutes you'll see `Data_Small.rsdk.zlib` in the folder at around 17,4MB.

Next we will compile the game:
```
./build_msys2.sh
```
After some time as well, you should see an executable in the `./bin` directory - that's your game build!

That's all, have fun!
