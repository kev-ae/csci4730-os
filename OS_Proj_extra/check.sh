#!/bin/bash

cp corrupted1.disk c1.disk
cp corrupted2.disk c2.disk
cp corrupted3.disk c3.disk

./fsck c1.disk
./fsck c2.disk
./fsck c3.disk

./fs_reader c1.disk > c1_result
./fs_reader c2.disk > c2_result
./fs_reader c3.disk > c3_result

./fs_reader ori.disk > result

diff c1_result result
diff c2_result result
diff c3_result result
