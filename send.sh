#!/bin/bash

# -d 01"hello"
send_1(){
while true
do	
	sendip -p ipv4 -is 10.0.0.1 -id 10.0.0.2 -p udp -us 1000 -ud 63000 -d 0x000148656C6C6F 10.0.0.2
	sleep 0.2
done
}
 
# -d 02"hello"
send_2(){
while true
do
        sendip -p ipv4 -is 10.0.0.1 -id 10.0.0.2 -p udp -us 2000 -ud 63000 -d 0x000248656C6C6F 10.0.0.2
        sleep 0.5
done
}
 
send_1 & send_2
