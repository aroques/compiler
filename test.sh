#!/bin/bash

file=$1

./comp $file

printf '\n'

cat "$file.asm"

printf '\n'

./VirtMach "$file.asm"