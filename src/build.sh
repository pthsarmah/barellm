#!/usr/bin/env bash

set -e

# --------------------------------------------------
# Paths
# --------------------------------------------------

export WORKSPACE="/home/crankshaft/Projects/baremetallm/edk/edk2"
export PACKAGES_PATH="/home/crankshaft/Projects/baremetallm/src"

EDK2="$WORKSPACE"
SRC="/home/crankshaft/Projects/baremetallm/src"

OVMF_CODE="/usr/share/edk2/x64/OVMF_CODE.4m.fd"
OVMF_VARS="/usr/share/edk2/x64/OVMF_VARS.4m.fd"

BUILD_DIR="$SRC/Build/BareLLMServer/DEBUG_GCC/X64"
ESP="/tmp/esp"
VARS="/tmp/OVMF_VARS.4m.fd"

# --------------------------------------------------
# Setup EDK II
# --------------------------------------------------

cd "$EDK2"

source "$EDK2/edksetup.sh"

# --------------------------------------------------
# Build
# --------------------------------------------------

build \
    -p "$SRC/BareLLMServer.dsc" \
    -m "$SRC/Application/BareLLMServer/BareLLMServer.inf" \
    -a X64 \
    -t GCC

# --------------------------------------------------
# Prepare EFI System Partition
# --------------------------------------------------

rm -rf "$ESP"
mkdir -p "$ESP/EFI/BOOT"

cp "$BUILD_DIR/BareLLMServer.efi" \
   "$ESP/EFI/BOOT/BOOTX64.EFI"

# --------------------------------------------------
# Copy fresh UEFI variable store
# --------------------------------------------------

cp "$OVMF_VARS" "$VARS"

# --------------------------------------------------
# Run QEMU
# --------------------------------------------------

qemu-system-x86_64 \
    -machine q35 \
    -m 512M \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$VARS" \
    -drive format=raw,file=fat:rw:"$ESP"
