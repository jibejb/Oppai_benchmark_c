# Oppai_benchmark_c

Oppai_benchmark rewrite in C.

## Bugs:
NVIDIA driver's(not nouveau) OpenGL has bugs.

## Usage:


Dependences: ISO C99 OpenGL(3.3 or later) glfw3(libglfw-dev) ncurses(libncurses-dev)/pdcurses(on Windows) libpthread

May it works on Windows if you modify the CMakeLists.txt.

```bash
git clone https://github.com/jibejb/Oppai_benchmark_c.git
cd Oppai_benchmark_c
mkdir build
cd build
cmake ..
make
./oppai_benchmark -h
```

You can watch the video "simple.mp4" to see how it works.

## Thanks:
Oppai_benchmark: https://github.com/PenguinCabinet/Oppai_benchmark

thpool: https://github.com/Pithikos/C-Thread-Pool

Linux C编程一站式学习: https://akaedu.github.io/book/

ChatGPT: https://chatgpt.com/

LearnOpenGL CN : https://learnopengl-cn.github.io/

......

