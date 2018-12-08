#!/bin/bash

./comp testfile2

printf '\n'

cat testfile2.asm

printf '\n'

./VirtMach testfile2.asm
