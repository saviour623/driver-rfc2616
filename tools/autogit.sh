#!/bin/bash

CMDOPT=(--branch=0, --time=0)

# AUTOMATICALLY GIT PUSH/FETCH
ARGS=${BASH_ARGV}
HOST=${HOSTNAME}
TYPE=${HOSTTYPE}
HOSTID=${UID}

declare -a CACHE # -A for associative
#index array ${name[subscript]} ${name[*]} -all ${!name[subscript]} - indices/keys \
#unset name[subscript] to remove
# ${A:-R} put R is a is null/unset
# ${A:=R} Assign A to R is A is null/unset
# ${A:?R} display error R if A is null/unset

# ${A:offset} offset of string