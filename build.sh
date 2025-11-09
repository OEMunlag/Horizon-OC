#!/bin/sh

SRC="Source/KIP/stratosphere/loader/source/oc"
DEST="build/stratosphere/loader/source/oc"
mkdir -p "dist/atmosphere/kips/"
mkdir -p "$DEST"

cp -r "$SRC"/. "$DEST"/

cd build/stratosphere/loader || exit 1
make -j"$(nproc)"
hactool -t kip1 out/nintendo_nx_arm64_armv8a/release/loader.kip --uncompress=hoc.kip
cd ../../../ # exit
cp build/stratosphere/loader/hoc.kip dist/atmosphere/kips/hoc.kip

cd Source/sys-clk/
./build.sh
cp -r dist/ ../../

cd ../../

cd Source/hoc-monitor/
make -j"$(nproc)"
cp hoc-monitor.ovl ../../dist/switch/.overlays/hoc-monitor.ovl

# cd ../../Source/configurator

# pip install -U pyinstaller
# pip install dpg
# pip install numpy
# pip install psutil
# pip install pillow
# pip install pathlib

# python3 -m PyInstaller --onefile --add-data "assets:assets" --icon=assets/icon.ico --noconsole src/main.py

# mv dist/main dist/hocconfig
# cp dist/hocconfig ../../