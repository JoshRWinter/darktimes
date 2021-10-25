#!/bin/bash

if [ $# -ne 1 ]; then
	echo need roll output param!
	exit 1
fi

cd asset
roll $1
