#! /bin/bash

ok=0
tot=0
function test_query() {
    cat $1| ./shield  -u system -p changeme 2>tmp.err >/dev/null
    foo=$?
    if test $foo != 0; then 
	echo "status $foo on in file $1 query:"
	cat $1
	echo
	cat tmp.err
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
