# Oppai_benchmark_c

Oppai_benchmark rewrite in C.

## You can watch the video "simple.mp4" to see how it works.

## Bugs:
on linux NVIDIA driver's(not nouveau) OpenGL has bugs.
Viewport size may be wrong on some systems.

## Usage:


Dependences: ISO C99 OpenGL(3.3 or later) glfw3(libglfw-dev) ncurses(libncurses-dev)/pdcurses(on Windows) pthread

### Windows

Install visualstudio and vcpkg https://github.com/microsoft/vcpkg

```bash
git clone https://github.com/jibejb/Oppai_benchmark_c.git
vcpkg install pthreads:x64-windows-static
vcpkg install glfw3:x64-windows-static
vcpkg install pdcurses:x64-windows-static

cd Oppai_benchmark_c
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[your path]/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
cd .\Release
.\oppai_benchmark -h
```

### Linux

```bash
git clone https://github.com/jibejb/Oppai_benchmark_c.git
cd Oppai_benchmark_c
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./oppai_benchmark -h
```

### This may works on other Unix like systems.


## Thanks:
Oppai_benchmark: https://github.com/PenguinCabinet/Oppai_benchmark

thpool: https://github.com/Pithikos/C-Thread-Pool

Linux C编程一站式学习: https://akaedu.github.io/book/

ChatGPT: https://chatgpt.com/

LearnOpenGL CN : https://learnopengl-cn.github.io/

......

