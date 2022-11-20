#!/bin/bash

set -xeu

gcc -g -Wall -Wextra -pedantic -o tests.out utils.c srdparser.c rdparser.c tests.c
./tests.out
