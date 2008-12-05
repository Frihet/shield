#! /bin/bash

function test_query() {
    echo "Query: $1";
    echo $1 | ./shield | sqlplus -s system/changeme | grep "^ORA-" 
}

test_query "create table test1 (id int not null);"
test_query "select * from test1;"
test_query "insert into test1 (id) values (3);"



