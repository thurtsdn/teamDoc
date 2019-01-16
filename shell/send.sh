#!/bin/bash

send_1(){
while true
do
        # send flow id 0 and 1 
        sendip -p ipv4 -is 10.0.0.1 -id 10.0.0.2 -p udp -us 8000 -ud 63000 -d 0x00 10.0.0.2
        sendip -p ipv4 -is 10.0.0.1 -id 10.0.0.2 -p udp -us 8000 -ud 63000 -d 0x01 10.0.0.2
        sleep 1
done
}

send_1
