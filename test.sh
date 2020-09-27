#!/bin/bash

testdir="test"
in_files="file1.txt file2.txt"
expect_file="${testdir}/expect.txt"
testee_file="${testdir}/testee.txt"

mkdir -p "$testdir"

./main $in_files > "$testee_file"
cat $in_files > "$expect_file"

diff "$expect_file" "$testee_file"