:
ip_address=192.168.254.3
#socket_number=10000
#socket_number=12345
#socket_number=100000000000000
socket_number=102030506060600
consume_socket_message $ip_address $socket_number &
sleep 1
produce_socket_message $ip_address $socket_number "hello world 1"
produce_socket_message $ip_address $socket_number "hello world 2"
produce_socket_message $ip_address $socket_number "hello world 3"
produce_socket_message $ip_address $socket_number "hello world 4"
produce_socket_message $ip_address $socket_number "hello world 5"
produce_socket_message $ip_address $socket_number "hello world 6"
produce_socket_message $ip_address $socket_number "hello world 7"
produce_socket_message $ip_address $socket_number "hello world 8"
produce_socket_message $ip_address $socket_number "hello world 9"
produce_socket_message $ip_address $socket_number "hello world 10"
produce_socket_message $ip_address $socket_number "hello world 11"
killprocess.sh `whoami` consume_socket_message y >/dev/null 2>&1
exit 0
