#!/bin/bash

PLUGIN_NAME=""

while getopts "n:" opt; do
  case "$opt" in 
    n) PLUGIN_NAME="$OPTARG"
      ;;
  esac
done

if [[ -z "$PLUGIN_NAME" ]]; then
  echo "Please supply the -n [plugin name] argument"
  exit 1
fi

mkdir "$PLUGIN_NAME"
mkdir "$PLUGIN_NAME/src"
touch "$PLUGIN_NAME/src/$PLUGIN_NAME.c"

sed "s/@PLUGIN_NAME@/${PLUGIN_NAME}/g" template/Makefile.template > "$PLUGIN_NAME/Makefile"
sed "s/@PLUGIN_NAME@/${PLUGIN_NAME}/g" template/manifest.ttl.template > "$PLUGIN_NAME/manifest.ttl"
sed "s/@PLUGIN_NAME@/${PLUGIN_NAME}/g" template/plugin.ttl.template > "$PLUGIN_NAME/$PLUGIN_NAME.ttl"
