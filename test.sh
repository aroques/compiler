#!/bin/bash

file=$1

./comp "test_src/$file"

printf '\n'

cat "test_src/$file.asm"

printf '\n'

./test_src/VirtMach "test_src/$file.asm"