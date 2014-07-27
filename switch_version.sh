#!/bin/bash
if [[ $# -ne 2 ]]; then
	echo "Usage: switch_version.sh [10.0 10.1 10.2 10.3] [full lite]"
	exit -1
fi

if [[ ! -f ./nsrreader_bb10.pro ]]; then
	echo "You should run this script in NSR Reader root directory"
	exit -1
fi

if [[ "$1" != "10.0" ]] && [[ "$1" != "10.1" ]] && [[ "$1" != "10.2" ]] && [[ "$1" != "10.3" ]]; then
	echo "Only 10.0, 10.1, 10.2 and 10.3 SDK versions supported!"
	exit -1
fi

if [[ "$2" != "full" ]] && [[ "$2" != "lite" ]]; then
	echo "Only full and lite app versions supported!"
	exit -1
fi

rm -rf ./bar-descriptor.xml
rm -rf ./Makefile
rm -rf ./translations/Makefile
rm -rf ./translations/*.ts

if [[ "$1" == "10.3" ]]; then
	PROJECT_SDK=10.3
else
	PROJECT_SDK=10.0
fi

cp ./projects/$PROJECT_SDK/bar-descriptor.xml.$2 ./bar-descriptor.xml
cp ./projects/Makefile.$2 ./Makefile
cp ./projects/translations/Makefile.$2 ./translations/Makefile

for ts in `(ls ./projects/translations/ | grep .*.ts)`; do
	if [[ "$2" == "lite" ]]; then
		cp ./projects/translations/$ts ./translations/`(echo $ts | sed s/nsrreader_bb10/nsrreader_bb10_lite/)`
	else
		cp ./projects/translations/$ts ./translations/$ts
	fi
done
