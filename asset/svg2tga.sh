#!/bin/bash

if [ $# -lt 4 ] ; then
	echo need width height input.svg output.tga
	exit 1
fi

rsvg-convert --width $1 --height $2 $3 > /tmp/tmp.png
convert /tmp/tmp.png $4
