#!/bin/bash

file=$1

./comp "test_src/$file"

printf '\n'

cat "$file.asm"

printf '\n'

./test_src/VirtMach "$file.asm"