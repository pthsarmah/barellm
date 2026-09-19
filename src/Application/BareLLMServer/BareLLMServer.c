#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
	Print(L"BareLLM UEFI Server v0.1\r\n");
	Print(L"Booted successfully.\r\n");
	UINT32 Index = 0;
	for (Index=0; Index<FixedPcdGet64(PcdBareLLMMaxConnections); Index++) {
		Print(L"Hello World\r\n");
	}
	return EFI_SUCCESS;
}
