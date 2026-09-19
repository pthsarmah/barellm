#!/bin/bash

build \
      -p /home/crankshaft/Projects/baremetallm/src/BareLLMServer.dsc \
      -m /home/crankshaft/Projects/baremetallm/src/Application/BareLLMServer/BareLLMServer.inf \
      -a X64 \
      -t GCC

cp /usr/share/edk2/x64/OVMF_VARS.4m.fd /tmp/OVMF_VARS.4m.fd

mkdir -p /tmp/esp/EFI/BOOT/
cp Build/BareLLMServer/DEBUG_GCC/X64/BareLLMServer.efi /tmp/esp/EFI/BOOT/BOOTX64.EFI

qemu-system-x86_64 \
          -machine q35 \
          -m 512M \
          -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
          -drive if=pflash,format=raw,file=/tmp/OVMF_VARS.4m.fd \
          -drive format=raw,file=fat:rw:/tmp/esp
