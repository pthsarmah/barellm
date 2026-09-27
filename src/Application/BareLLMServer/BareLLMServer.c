#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <Library/UefiBootServicesTableLib.h>

typedef struct {
	UINT32 Rows;
	UINT32 Cols;
	INT32  *Data;
} Matrix;

UINTN ReadNumber(void) {
	EFI_INPUT_KEY Key;
	UINTN EventIndex;
	UINTN Number = 0;

	gBS->WaitForEvent(1, gST->ConIn->WaitForKey, &EventIndex);
}

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
	gST->ConOut->ClearScreen(gST->ConOut);
	Print(L"BareLLM UEFI Server v0.1\r\n");
	Print(L"Booted successfully.\r\n");
	UINT32 Index = 0;
	for (Index=0; Index<FixedPcdGet64(PcdBareLLMMaxConnections); Index++) {
		Print(L"Hello World\r\n");
	}

	//simple NN
	
	// weight matrix
	Matrix weights1;

	weights1.Rows = 0;

	return EFI_SUCCESS;
}
