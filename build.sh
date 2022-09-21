#!/bin/bash

set -xeu

gcc -g -Wall -Wextra -pedantic -o tests.out tests.c
./tests.out
