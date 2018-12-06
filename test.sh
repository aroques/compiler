#!/bin/bash

./comp testfile

printf '\n'

cat testfile.asm

printf '\n'

./VirtMach testfile.asm
