#include "Protocol/SimpleTextIn.h"
#include "Uefi/UefiBaseType.h"
#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/UefiBootServicesTableLib.h>

typedef struct {
	UINT32 Rows;
	UINT32 Cols;
	double  *Data;
} Matrix;

UINT32 ReadNumber(CHAR16 *Prompt) {
	EFI_INPUT_KEY Key;
	UINTN EventIndex;
	UINT32 Number = 0;

	Print(L"%s", Prompt);

	while (TRUE) {
		gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
		if (EFI_ERROR(gST->ConIn->ReadKeyStroke(gST->ConIn, &Key)))
			continue;

		if (Key.UnicodeChar == L'\r') {
			Print(L"\r\n");
			break;
		}

		if (Key.UnicodeChar >= L'0' && Key.UnicodeChar <= L'9') {
			Number = Number * 10 + (Key.UnicodeChar - L'0');
		}

		gST->ConOut->OutputString(
			gST->ConOut,
			&Key.UnicodeChar
		);
	}

	return Number;
}

static const float IrisData[150][4] = {
    {5.1f, 3.5f, 1.4f, 0.2f},
    {4.9f, 3.0f, 1.4f, 0.2f},
    {4.7f, 3.2f, 1.3f, 0.2f},
    {4.6f, 3.1f, 1.5f, 0.2f},
    {5.0f, 3.6f, 1.4f, 0.2f},
    {5.4f, 3.9f, 1.7f, 0.4f},
    {4.6f, 3.4f, 1.4f, 0.3f},
    {5.0f, 3.4f, 1.5f, 0.2f},
    {4.4f, 2.9f, 1.4f, 0.2f},
    {4.9f, 3.1f, 1.5f, 0.1f},
    {5.4f, 3.7f, 1.5f, 0.2f},
    {4.8f, 3.4f, 1.6f, 0.2f},
    {4.8f, 3.0f, 1.4f, 0.1f},
    {4.3f, 3.0f, 1.1f, 0.1f},
    {5.8f, 4.0f, 1.2f, 0.2f},
    {5.7f, 4.4f, 1.5f, 0.4f},
    {5.4f, 3.9f, 1.3f, 0.4f},
    {5.1f, 3.5f, 1.4f, 0.3f},
    {5.7f, 3.8f, 1.7f, 0.3f},
    {5.1f, 3.8f, 1.5f, 0.3f},
    {5.4f, 3.4f, 1.7f, 0.2f},
    {5.1f, 3.7f, 1.5f, 0.4f},
    {4.6f, 3.6f, 1.0f, 0.2f},
    {5.1f, 3.3f, 1.7f, 0.5f},
    {4.8f, 3.4f, 1.9f, 0.2f},
    {5.0f, 3.0f, 1.6f, 0.2f},
    {5.0f, 3.4f, 1.6f, 0.4f},
    {5.2f, 3.5f, 1.5f, 0.2f},
    {5.2f, 3.4f, 1.4f, 0.2f},
    {4.7f, 3.2f, 1.6f, 0.2f},
    {4.8f, 3.1f, 1.6f, 0.2f},
    {5.4f, 3.4f, 1.5f, 0.4f},
    {5.2f, 4.1f, 1.5f, 0.1f},
    {5.5f, 4.2f, 1.4f, 0.2f},
    {4.9f, 3.1f, 1.5f, 0.1f},
    {5.0f, 3.2f, 1.2f, 0.2f},
    {5.5f, 3.5f, 1.3f, 0.2f},
    {4.9f, 3.6f, 1.4f, 0.1f},
    {4.4f, 3.0f, 1.3f, 0.2f},
    {5.1f, 3.4f, 1.5f, 0.2f},
    {5.0f, 3.5f, 1.3f, 0.3f},
    {4.5f, 2.3f, 1.3f, 0.3f},
    {4.4f, 3.2f, 1.3f, 0.2f},
    {5.0f, 3.5f, 1.6f, 0.6f},
    {5.1f, 3.8f, 1.9f, 0.4f},
    {4.8f, 3.0f, 1.4f, 0.3f},
    {5.1f, 3.8f, 1.6f, 0.2f},
    {4.6f, 3.2f, 1.4f, 0.2f},
    {5.3f, 3.7f, 1.5f, 0.2f},
    {5.0f, 3.3f, 1.4f, 0.2f},

    {7.0f, 3.2f, 4.7f, 1.4f},
    {6.4f, 3.2f, 4.5f, 1.5f},
    {6.9f, 3.1f, 4.9f, 1.5f},
    {5.5f, 2.3f, 4.0f, 1.3f},
    {6.5f, 2.8f, 4.6f, 1.5f},
    {5.7f, 2.8f, 4.5f, 1.3f},
    {6.3f, 3.3f, 4.7f, 1.6f},
    {4.9f, 2.4f, 3.3f, 1.0f},
    {6.6f, 2.9f, 4.6f, 1.3f},
    {5.2f, 2.7f, 3.9f, 1.4f},
    {5.0f, 2.0f, 3.5f, 1.0f},
    {5.9f, 3.0f, 4.2f, 1.5f},
    {6.0f, 2.2f, 4.0f, 1.0f},
    {6.1f, 2.9f, 4.7f, 1.4f},
    {5.6f, 2.9f, 3.6f, 1.3f},
    {6.7f, 3.1f, 4.4f, 1.4f},
    {5.6f, 3.0f, 4.5f, 1.5f},
    {5.8f, 2.7f, 4.1f, 1.0f},
    {6.2f, 2.2f, 4.5f, 1.5f},
    {5.6f, 2.5f, 3.9f, 1.1f},
    {5.9f, 3.2f, 4.8f, 1.8f},
    {6.1f, 2.8f, 4.0f, 1.3f},
    {6.3f, 2.5f, 4.9f, 1.5f},
    {6.1f, 2.8f, 4.7f, 1.2f},
    {6.4f, 2.9f, 4.3f, 1.3f},
    {6.6f, 3.0f, 4.4f, 1.4f},
    {6.8f, 2.8f, 4.8f, 1.4f},
    {6.7f, 3.0f, 5.0f, 1.7f},
    {6.0f, 2.9f, 4.5f, 1.5f},
    {5.7f, 2.6f, 3.5f, 1.0f},
    {5.5f, 2.4f, 3.8f, 1.1f},
    {5.5f, 2.4f, 3.7f, 1.0f},
    {5.8f, 2.7f, 3.9f, 1.2f},
    {6.0f, 2.7f, 5.1f, 1.6f},
    {5.4f, 3.0f, 4.5f, 1.5f},
    {6.0f, 3.4f, 4.5f, 1.6f},
    {6.7f, 3.1f, 4.7f, 1.5f},
    {6.3f, 2.3f, 4.4f, 1.3f},
    {5.6f, 3.0f, 4.1f, 1.3f},
    {5.5f, 2.5f, 4.0f, 1.3f},
    {5.5f, 2.6f, 4.4f, 1.2f},
    {6.1f, 3.0f, 4.6f, 1.4f},
    {5.8f, 2.6f, 4.0f, 1.2f},
    {5.0f, 2.3f, 3.3f, 1.0f},
    {5.6f, 2.7f, 4.2f, 1.3f},
    {5.7f, 3.0f, 4.2f, 1.2f},
    {5.7f, 2.9f, 4.2f, 1.3f},
    {6.2f, 2.9f, 4.3f, 1.3f},
    {5.1f, 2.5f, 3.0f, 1.1f},
    {5.7f, 2.8f, 4.1f, 1.3f},

    {6.3f, 3.3f, 6.0f, 2.5f},
    {5.8f, 2.7f, 5.1f, 1.9f},
    {7.1f, 3.0f, 5.9f, 2.1f},
    {6.3f, 2.9f, 5.6f, 1.8f},
    {6.5f, 3.0f, 5.8f, 2.2f},
    {7.6f, 3.0f, 6.6f, 2.1f},
    {4.9f, 2.5f, 4.5f, 1.7f},
    {7.3f, 2.9f, 6.3f, 1.8f},
    {6.7f, 2.5f, 5.8f, 1.8f},
    {7.2f, 3.6f, 6.1f, 2.5f},
    {6.5f, 3.2f, 5.1f, 2.0f},
    {6.4f, 2.7f, 5.3f, 1.9f},
    {6.8f, 3.0f, 5.5f, 2.1f},
    {5.7f, 2.5f, 5.0f, 2.0f},
    {5.8f, 2.8f, 5.1f, 2.4f},
    {6.4f, 3.2f, 5.3f, 2.3f},
    {6.5f, 3.0f, 5.5f, 1.8f},
    {7.7f, 3.8f, 6.7f, 2.2f},
    {7.7f, 2.6f, 6.9f, 2.3f},
    {6.0f, 2.2f, 5.0f, 1.5f},
    {6.9f, 3.2f, 5.7f, 2.3f},
    {5.6f, 2.8f, 4.9f, 2.0f},
    {7.7f, 2.8f, 6.7f, 2.0f},
    {6.3f, 2.7f, 4.9f, 1.8f},
    {6.7f, 3.3f, 5.7f, 2.1f},
    {7.2f, 3.2f, 6.0f, 1.8f},
    {6.2f, 2.8f, 4.8f, 1.8f},
    {6.1f, 3.0f, 4.9f, 1.8f},
    {6.4f, 2.8f, 5.6f, 2.1f},
    {7.2f, 3.0f, 5.8f, 1.6f},
    {7.4f, 2.8f, 6.1f, 1.9f},
    {7.9f, 3.8f, 6.4f, 2.0f},
    {6.4f, 2.8f, 5.6f, 2.2f},
    {6.3f, 2.8f, 5.1f, 1.5f},
    {6.1f, 2.6f, 5.6f, 1.4f},
    {7.7f, 3.0f, 6.1f, 2.3f},
    {6.3f, 3.4f, 5.6f, 2.4f},
    {6.4f, 3.1f, 5.5f, 1.8f},
    {6.0f, 3.0f, 4.8f, 1.8f},
    {6.9f, 3.1f, 5.4f, 2.1f},
    {6.7f, 3.1f, 5.6f, 2.4f},
    {6.9f, 3.1f, 5.1f, 2.3f},
    {5.8f, 2.7f, 5.1f, 1.9f},
    {6.8f, 3.2f, 5.9f, 2.3f},
    {6.7f, 3.3f, 5.7f, 2.5f},
    {6.7f, 3.0f, 5.2f, 2.3f},
    {6.3f, 2.5f, 5.0f, 1.9f},
    {6.5f, 3.0f, 5.2f, 2.0f},
    {6.2f, 3.4f, 5.4f, 2.3f},
    {5.9f, 3.0f, 5.1f, 1.8f}
};

static const UINT8 IrisLabels[150] = {
    // 0-49   = setosa
    // 50-99  = versicolor
    // 100-149 = virginica

    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,

    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,

    2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2
};

float Dot(Matrix X, Matrix Y) {

}

/*
 * Train the model, return the weights
 * @param: X - training features
 * @param: Y - training classes
 * @param: alpha - learning rate
 * @param: epochs - no. of iterations
 * @returns: A matrix which is the learned weight matrix
*/
Matrix NNTrain(Matrix X, Matrix Y, float alpha, UINTN epochs) {
	Matrix weights;
	return weights;
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

	EFI_INPUT_KEY StopKey;

	UINT32 SplitIndex = 0.8 * 150;
	Matrix XTrain, YTrain, XTest, YTest;

	XTrain.Rows = SplitIndex;
	YTrain.Rows = SplitIndex;
	XTrain.Cols = 4;
	YTrain.Cols = 1;

	XTest.Rows = 150 - SplitIndex;
	YTest.Rows = 150 - SplitIndex;
	XTest.Cols = 4;
	YTest.Cols = 1;

	XTrain.Data = AllocatePool(XTrain.Rows * XTrain.Cols * sizeof(double));
	YTrain.Data = AllocatePool(YTrain.Rows * YTrain.Cols * sizeof(double));
	XTest.Data = AllocatePool(XTest.Rows * XTrain.Cols * sizeof(double));
	YTest.Data = AllocatePool(YTest.Rows * YTest.Cols * sizeof(double));

	for (UINTN i = 0; i < SplitIndex; i++) {
		for (UINTN j = 0; j < XTrain.Cols; j++) {
			XTrain.Data[i * XTrain.Cols + j] = IrisData[i][j];
			if (SplitIndex + i < 150)
				XTest.Data[SplitIndex + i * XTest.Cols + j] = IrisData[SplitIndex + i][j];
		}
	}

	for (UINTN i = 0; i < SplitIndex; i++) {
		YTrain.Data[i] = IrisLabels[i];
		if (SplitIndex + i < 150)
			YTest.Data[SplitIndex + i] = IrisLabels[SplitIndex + i];
	}

	NNTrain(XTrain, YTrain, 0.1, 100);

	gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
	gST->ConIn->ReadKeyStroke(gST->ConIn, &StopKey);

	return EFI_SUCCESS;
}
