# Famicom-cpp

## NES Emulator written in C++

<br><br><br>

install SDL2

<pre>
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-net-dev libsdl2-ttf-dev
</pre>

<br><br><br>

### WIndows + MSYS2

MSYS2  

<pre>
    pacman -Syuu
    pacman -S mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain base-devel msys2-devel
    pacman -S mingw-w64-x86_64-clang mingw-w64-x86_64-llvm mingw-w64-x86_64-clang-tools-extra
    pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja 
    pacman -S mingw-w64-x86_64-libsndfile mingw-w64-x86_64-lldb mingw-w64-x86_64-opus mingw-w64-x86_64-boost
    pacman -S vim openssh sshpass git unzip rsync 
    pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf
</pre>

<br><br><br>

vscode  extensions
<pre>
    C/C++
    C/C++ Extension Pack
    Better C++ Syntax
    CMake
    CMake Tools
    CodeLLDB
    Makefile Tools
    IntelliCode
    clangd
</pre>

https://user-images.githubusercontent.com/10168979/167252280-cf27defe-456b-4261-877a-c19ef5543edc.mp4




<br><br><br>




(Ctrl + Shift + p)  
CMake: Configure

<br><br><br>

### F7
Build

<br>

### F5  
debug

<br><br><br>

## Keyboard
<pre>
    case SDLK_x:
        return PAD_A;
    case SDLK_z:
        return PAD_B;
    case SDLK_a:
        return PAD_SELECT;
    case SDLK_s:
        return PAD_START;
    case SDLK_RIGHT:
        return PAD_R;
    case SDLK_LEFT:
        return PAD_L;
    case SDLK_UP:
        return PAD_U;
    case SDLK_DOWN:
        return PAD_D;

</pre>


<br><br><br><br><br><br><br><br><br>

