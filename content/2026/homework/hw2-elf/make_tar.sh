#!/bin/bash -e

# Create a tar of just these files, with the date they were last modified in
# Git and no extraneous metadata.

FILES=(main.c elf.c elf1.c elf_explain.c elf_extra_credit.c elf_extra_credit2.c elf_extra_credit2lib.c Makefile .gitignore)
DATE="$(git log -1 --format=%cd --date=iso8601 -- "${FILES[@]}")"
touch -d "$DATE" "${FILES[@]}"
cd ..
tar cavf hw2-elf/hw2-elf.tar.gz -C hw2-elf --owner=0 --group=0 --no-same-owner "${FILES[@]}"
