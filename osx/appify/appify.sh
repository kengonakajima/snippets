#!/bin/bash

APPNAME=${2:-$(basename "$1" ".sh")}
APPDIR="$APPNAME.app/"

if [ "$APPNAME" == "" ]; then
  echo "empty name? Usage: appify.sh YOURSCRIPT.sh"
  exit 1
fi

if [ -a "$APPNAME.app" ]; then
  echo "$PWD/$APPNAME.app already exists :("
  exit 1
fi

mkdir -p "$APPDIR/Contents/MacOS"
cp "$1" "$APPDIR/Contents/MacOS/$APPNAME"
chmod +x "$APPDIR/Contents/MacOS/$APPNAME"

cp Info.plist "$APPDIR/Contents/"
mkdir "$APPDIR/Contents/Resources"
cp catimage.icns "$APPDIR/Contents/Resources/"

echo "$PWD/$APPNAME.app"
