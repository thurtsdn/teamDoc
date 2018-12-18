#!/bin/bash

# ryu install & config
# please run this in ryu root dir

# cd ryu
sudo apt-get install python-pip
sudo pip install .
sudo pip install --upgrade pip
sudo apt-get install curl
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
sudo python2.7 get-pip.py --force-reinstall

# set soft link
cd /usr/local/lib/python2.7/dist-packages
sudo mv ryu ryu.orig
sudo ln -s $(cd -)/ryu ./
