#! /bin/bash

echo "[ compiling all the applications]"
for j in soclib_dspin_16p_gm soclib_dspin_16_lm 
do
	for i in mini_ocean_c 
	do
		echo "[. install.sh $j $i]"
		. install.sh $j $i 
		make clean -s
		make app_clean -s
		make -s
	done	
done

