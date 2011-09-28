#!/bin/bash

gcc $(pkg-config --cflags --libs gthread-2.0) -g libfutures.c -shared -c libfutures.c && gcc $(pkg-config --cflags --libs gthread-2.0) -g libfutures.o test.c 

