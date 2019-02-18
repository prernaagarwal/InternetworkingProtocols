#!/bin/bash
testfile="./test.jpg"
port=1080
ip="127.0.0.1"
server="./cs494rcp_server"
client="./cs494rcp_client"

serverout="/dev/null"
clientout="/dev/null"

#start server in background. Redirect output to serverout
echo "Starting $server..."
$server $port &>$serverout &

#wait one second before starting client. Redirect output to clientout
sleep 1
echo "Starting $client..."
$client $ip $port $testfile &>$clientout

md5orig=$(md5sum $testfile | awk '{ print $1 }')
echo $testfile MD5: $md5orig
md5new=$(md5sum $testfile".out" | awk '{ print $1 }')
echo $testfile".out" MD5: $md5new

if [ ! -z "$md5orig" ] && [ "$md5orig" == "$md5new" ]; then
   echo -e "Result: \e[92m OK\e[0m"
   exit 0
else
   echo -e "Result: \e[91m FAILED\e[0m"
   exit 1
fi
   