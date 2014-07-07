#!/bin/bash
if [[ $# -ne 1 ]]; then
	echo "Usage: switch_version.sh [10.0 10.3]"
	exit 0
fi

if [[ "$1" != "10.0" ]] && [[ "$1" != "10.3" ]]; then
	echo "Only 10.0 and 10.3 SDK versions supported!"
	exit 0
fi

if [[ -f Makefile.full ]]; then
	NEW_SUFFIX=lite
	CUR_SUFFIX=full
else
	NEW_SUFFIX=full
	CUR_SUFFIX=lite
fi

if [[ -f ./bar-descriptor.xml ]]; then
	rm -rf ./bar-descriptor.xml
fi

cp ./projects/$1/bar-descriptor.xml.$CUR_SUFFIX ./bar-descriptor.xml

#Move current files to new prefix
mv ./Makefile ./Makefile.$NEW_SUFFIX
mv ./translations/Makefile ./translations/Makefile.$NEW_SUFFIX
mv ./.cproject ./.cproject.$NEW_SUFFIX
mv ./.project ./.project.$NEW_SUFFIX

#Recover saved files
mv ./Makefile.$CUR_SUFFIX ./Makefile
mv ./translations/Makefile.$CUR_SUFFIX ./translations/Makefile
mv ./.cproject.$CUR_SUFFIX ./.cproject
mv ./.project.$CUR_SUFFIX ./.project

if [[ "$CUR_SUFFIX" != "lite" ]]; then
	mv ./translations/nsrreader_bb10_lite.ts ./translations/nsrreader_bb10.ts
	mv ./translations/nsrreader_bb10_lite_ru.ts ./translations/nsrreader_bb10_ru.ts
	mv ./translations/nsrreader_bb10_lite_id.ts ./translations/nsrreader_bb10_id.ts
else
	mv ./translations/nsrreader_bb10.ts ./translations/nsrreader_bb10_lite.ts
	mv ./translations/nsrreader_bb10_ru.ts ./translations/nsrreader_bb10_lite_ru.ts
	mv ./translations/nsrreader_bb10_id.ts ./translations/nsrreader_bb10_lite_id.ts
fi