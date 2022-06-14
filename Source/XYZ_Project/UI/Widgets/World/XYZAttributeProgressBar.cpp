#include "XYZAttributeProgressBar.h"
#include <Components/ProgressBar.h>

void UXYZAttributeProgressBar::SetProgressPercentage(float Percentage)
{
	AttributeProgressBar->SetPercent(Percentage);
}

void UXYZAttributeProgressBar::OnProgressChanged(float CurrentValue, float MaxValue)
{
	float Percentage = (MaxValue != 0.0f) ? (CurrentValue / MaxValue) : 0.0f;
	SetProgressPercentage(Percentage);
}

