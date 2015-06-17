#!/bin/sh
# clean.sh - Remove temporary files

find . -name '*.tmp' -print | xargs rm
