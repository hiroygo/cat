#!/bin/bash

in_files="file1.txt file2.txt"
diff <(cat $in_files) <(./main $in_files)