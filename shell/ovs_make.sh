#!/bin/bash

#ovs2.5.x make

./boot.sh
./configure --with-linux=/lib/modules/`uname -r`/build --enable-Werror
make
