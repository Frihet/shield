#! /bin/sh

shield_sock_dir=/var/lib/shield
user=$1
pass=$2
db=$3

mkdir -p $shield_sock_dir
shield_multiplex -u $user -p $pass -c $db -s $shield_sock_dir/$db-$user.sock
