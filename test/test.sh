#!/bin/bash

ls | grep -v test.sh | xargs rm&>/dev/null  # inverts ls to delete all files except this one.
cp ../*/* ./&>/dev/null # atualization of edited files
cp ../terminator.layout ~/.config/terminator/config
# compiles what needs to be compiled...
gcc -o physical-client physical-client.c
gcc -o physical-server physical-server.c

#run everything
#terminator
