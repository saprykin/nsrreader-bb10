#!/bin/bash
if [[ -f Makefile.full ]]; then
	NEW_SUFFIX=lite
	CUR_SUFFIX=full
else
	NEW_SUFFIX=full
	CUR_SUFFIX=lite
fi

#Move current files to new prefix
mv ./bar-descriptor.xml ./bar-descriptor.xml.$NEW_SUFFIX
mv ./Makefile ./Makefile.$NEW_SUFFIX
mv ./translations/Makefile ./translations/Makefile.$NEW_SUFFIX
mv ./.cproject ./.cproject.$NEW_SUFFIX
mv ./.project ./.project.$NEW_SUFFIX

#Recover saved files
mv ./bar-descriptor.xml.$CUR_SUFFIX ./bar-descriptor.xml
mv ./Makefile.$CUR_SUFFIX ./Makefile
mv ./translations/Makefile.$CUR_SUFFIX ./translations/Makefile
mv ./.cproject.$CUR_SUFFIX ./.cproject
mv ./.project.$CUR_SUFFIX ./.project

if [[ "$CUR_SUFFIX" != "lite" ]]; then
	mv ./translations/nsrreader_bb10_lite.ts ./translations/nsrreader_bb10.ts
	mv ./translations/nsrreader_bb10_lite_ru.ts ./translations/nsrreader_bb10_ru.ts
else
	mv ./translations/nsrreader_bb10.ts ./translations/nsrreader_bb10_lite.ts
	mv ./translations/nsrreader_bb10_ru.ts ./translations/nsrreader_bb10_lite_ru.ts
fi