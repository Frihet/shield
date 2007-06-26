#! /bin/bash

ok=0
tot=0
function test_query() {
    (cat $1; echo ';' )| ./shield >/dev/null 2>/dev/null
    foo=$?
    if test $foo != 0; then 
	echo "status $foo on in file $1 query:"
	cat $1
	echo
	(cat $1; echo ';' )| ./shield 2>&1 >/dev/null 
	echo
	echo
    else
	ok=$(($ok+1))
    fi
    tot=$(($tot+1))
}

for i in sql/axel_evil* sql2/axel_evil*; do
    test_query $i
done

echo $ok of $tot ok
