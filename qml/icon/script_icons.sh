#!/bin/bash

name=harbour-sailtuner.png
dir=../../icons
origin=$dir/256x256/$name

function ic_resize
{
	convert $origin -geometry $1 $dir/$1/$name
}

ic_resize 86x86
ic_resize 108x108
ic_resize 128x128
