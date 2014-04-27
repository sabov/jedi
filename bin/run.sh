#!/bin/bash
cd ..
make
RETVAL=$?
[ $RETVAL -eq 0 ] && cd bin && ./SimpleVRGame
[ $RETVAL -ne 0 ] && cd bin
