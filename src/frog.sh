#!/bin/bash

path="/data-2/mstruong/Frog_dataset/"
addr0=${path}"youtube.bin"
addr1=${path}"roadNetCA.bin"
addr2=${path}"wikiTalk.bin"
addr3=${path}"amazon.bin"
addr4=${path}"dblp.bin"
addr5=${path}"twitter.bin"
ADDR_ARRAY=($addr0 $addr1 $addr2 $addr3 $addr4 $addr5)
echo "[0] Run youtube dataset"
echo "[1] Run roadNetCA dataset"
echo "[2] Run wikiTalk dataset"
echo "[3] Run amazon dataset"
echo "[4] Run dblp dataset"
echo "[5] Run twitter dataset"
read -p "Option: " set

./exp ${ADDR_ARRAY[$set]} 
