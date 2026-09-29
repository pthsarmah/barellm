#include "ProcessorBind.h"
#include "Protocol/SimpleTextIn.h"
#include "Uefi/UefiBaseType.h"
#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/UefiBootServicesTableLib.h>

#define EXP_PRECISION 30

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

double ExpApprox(double x)
{
	int negative = 1;
	if (x < 0) { negative = 1; x = -x; }

	/*Reductions help to converge exponentials faster
	 * e^10 = e ^ (10*16/16) = e^((10/16)*16) = (e^(0.625))^16
	 * e^0.625 converges faster
	 * Then all we need is faster squaring e^0.625 --s--> e^1.25 --s--> e^2.5 --...-> e^10
	*/
	int reductions = 0;
	while (x > 1) {
		x *= 0.5;
		reductions++;
	}

	double term = 1.0;
	double sum = 1.0;

	for (int i=0; i<=EXP_PRECISION; i++) {
		term *= x / (double)i;
		sum += term;
	}

	for (int i=0; i<reductions; i++) {
		sum *= sum;
	}

	if (negative) return 1.0 / sum;

	return sum;
}

double LogApprox(double x) {
	if (x <= 0) return -100.0;

	int k = 0;

	while (x > 1.5) {
		x *= 0.5;
		k++;
	}

	while (x < 0.75) {
		x *= 2.0;
		k--;
	}

	double z = (x - 1.0) / (x + 1.0);
	double z2 = z * z;

	double term = z;
	double sum = 0.0;

	for (UINT32 n = 1; n <= 30; n++) {
		sum += term / (double)(2 * n - 1);
		term *= z2;
	}

	const double LN2 = 0.6931471805599453;

	return 2.0 * sum + k * LN2;
}

Matrix Multiply(Matrix *X, Matrix *Y) {
	UINT32 n = X->Rows;
	UINT32 m = Y->Cols;

	Matrix res;
	res.Rows = n;
	res.Cols = m;

	for (UINTN i=0; i<n; i++) {
		for (UINTN k=0; k<n; k++) {
			double x = X->Data[i * X->Cols + k];
			for (UINTN j=0; j<n; ++j) {
				res.Data[i * res.Cols + j] = x * Y->Data[k * Y->Cols + j];
			}
		}
	}

	return res;
}

void Softmax(Matrix *Z, Matrix *P)
{
	for (UINT32 i = 0; i < Z->Rows; i++) {

		double maxValue = Z->Data[i * Z->Cols];

		for (UINT32 j = 1; j < Z->Cols; j++) {
			double value = Z->Data[i * Z->Cols + j];
			if (value > maxValue)
				maxValue = value;
		}

		double sum = 0.0;

		for (UINT32 j = 0; j < Z->Cols; j++) {
			UINT32 index = i * Z->Cols + j;
			P->Data[index] =
				ExpApprox(Z->Data[index] - maxValue);

			sum += P->Data[index];
		}

		for (UINT32 j = 0; j < Z->Cols; j++) {
			UINT32 index = i * Z->Cols + j;
			P->Data[index] /= sum;
		}
	}
}

double CategoricalCrossEntropy(Matrix *Y, Matrix *P)
{
	double loss = 0.0;

	const double EPSILON = 1e-12;

	for (UINT32 i = 0; i < Y->Rows; i++) {
		for (UINT32 j = 0; j < Y->Cols; j++) {

			UINT32 index = i * Y->Cols + j;

			double y = Y->Data[index];
			double p = P->Data[index];

			/*
			 * Prevent log(0)
			*/
			if (p < EPSILON)
				p = EPSILON;

			loss -= y * LogApprox(p);
		}
	}

	return loss / (double)Y->Rows;
}

void UpdateWeights(
    Matrix *X,
    Matrix *Y,
    Matrix *P,
    Matrix *weights,
    double alpha
)
{
	/*
	 * dW = X^T (P - Y) / N
	 */

	UINT32 N = X->Rows;
	UINT32 inputFeatures = X->Cols;
	UINT32 classes = weights->Cols;

	for (UINT32 i = 0; i < inputFeatures; i++) {

		for (UINT32 j = 0; j < classes; j++) {

			double gradient = 0.0;

			for (UINT32 sample = 0; sample < N; sample++) {

				double x =
					X->Data[sample * X->Cols + i];

				double prediction =
					P->Data[sample * P->Cols + j];

				double target =
					Y->Data[sample * Y->Cols + j];

				gradient += x * (prediction - target);
			}

			gradient /= (double)N;

			UINT32 weightIndex =
				i * weights->Cols + j;

			weights->Data[weightIndex] -=
				alpha * gradient;
		}
	}
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
	weights.Rows = X.Cols;
	weights.Cols = 3;

	weights.Data = AllocatePool(weights.Rows * weights.Cols * sizeof(double));

	for (UINT32 i=0; i<weights.Rows; i++) {
		for (UINT32 j=0; j<weights.Cols; j++) {
			weights.Data[i * weights.Cols + j] = 0;
		}
	}

	Matrix layer1 = Multiply(&X, &weights);

	for (UINT32 i=0; i<layer1.Rows; i++) {
		for (UINT32 j=0; j<layer1.Cols; j++) {
			UINT32 Index = i * layer1.Cols + j;
		}
	}

	//backprop

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
