#!/bin/sh
awk -f "${INPUT_DIR}case.awk" < "${INPUT_DIR}case.txt" > "${OUTPUT_DIR}case.c"
exit $?
