#!/bin/sh
gcc -std=gnu11 -Wall -Werror src/**/*.c src/*.c -ojong-un -DNDEBUG -DUNIX -Iinclude -Iinclude/asar -Iinclude/common -Iinclude/file -Iinclude/jong-un -L. -lasar -ldl
