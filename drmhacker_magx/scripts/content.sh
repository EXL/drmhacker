#!/usr/bin/env bash

function usage {
	echo -e "Helper script for copying content directories for MotoMAGX\n"
	echo -e "Usage:"
	echo -e "\t./content.sh <input-unpacked-CG52-dir> <output-content-dir>\n"
	echo -e "Example:"
	echo -e "\t./content.sh AURA-CG52-squashfs-root AURA-standard-files\n"
	exit 1
}

function check_arguments {
	if [ -z "$1"  ] || [ -z "$2" ]; then
		usage
	fi
}

function check_directories {
	if [ ! -d "$1" ]; then
		usage
	fi
	if [ ! -f "$1/ezx_resource_version.txt" ]; then
		echo -e "Error: check your input unpacked CG52 directory!"
		exit 1
	fi
	if [ ! -d "$2" ]; then
		mkdir "$2"
	fi
}

function make_directories {
	mkdir -p "$1/picture/"
	mkdir -p "$1/java/"
	mkdir -p "$1/sound/"
	mkdir -p "$1/video/"
}

function copy_files {
	cp --no-preserve=mode,ownership -v "$1/ezx_resource_version.txt" "$2"

	cd "$1/osb/"
	cp --no-preserve=mode,ownership -v * "../../$2/picture/"
	cd -

	cd "$1/picture/"
	cp --no-preserve=mode,ownership -v * "../../$2/picture/"
	cd -

	cd "$1/poweroff/"
	cp --no-preserve=mode,ownership -v * "../../$2/picture/"
	cd -

	cd "$1/poweron/"
	cp --no-preserve=mode,ownership -v * "../../$2/picture/"
	cd -

	cd "$1/sound/ringtones/"
	cp --no-preserve=mode,ownership -v * "../../../$2/sound/"
	cd -

	cd "$1/sound/slider_tones/"
	cp --no-preserve=mode,ownership -v * "../../../$2/sound/"
	cd -

	cd "$1/sound/ui_sounds/"
	cp --no-preserve=mode,ownership -v * "../../../$2/sound/"
	cd -

	cd "$1/preloadapps/javaapps/"
	cp --no-preserve=mode,ownership -v * "../../../$2/java/"
	cd -

	cd "$1/videos/"
	cp --no-preserve=mode,ownership -v * "../../$2/video/"
	cd -
}

function delete_files {
	find $1 -type f -name "*.drm*" -delete
	find $1 -type f -name "*.dcf*" -delete
	rm -f "$1/java/JavaDB"
}

function main {
	check_arguments "$1" "$2"
	check_directories "$1" "$2"
	make_directories "$2"
	copy_files "$1" "$2"
	delete_files "$2"
}

main "$1" "$2"
