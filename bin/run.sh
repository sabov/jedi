#!/bin/bash
cd ..
make
RETVAL=$?
[ $RETVAL -eq 0 ] && cd bin && ./Jedi
[ $RETVAL -ne 0 ] && cd bin
