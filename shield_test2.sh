#! /bin/bash

ok=0
tot=0
function test_query() {
    cat $1| ./shield -u system -p changeme |tr -d '\0' >tmp.sql
    echo ";" >> tmp.sql

    sqlplus system/changeme <tmp.sql >tmp_file 

    if grep ORA- tmp_file >/dev/null; then 
	echo "Error in query:"
	cat $1
	echo 
	echo "->"
	echo
	cat tmp.sql
	echo
	echo "->"
	cat tmp_file
    else
	ok=$(($ok+1))
    fi

    tot=$(($tot+1))
}

for i in sql/axel_evil*; do
    test_query $i
done

echo $ok of $tot ok
