// Fill out your copyright notice in the Description page of Project Settings.

#include "ScreenshotWidget.h"
#include "Runtime/Engine/Public/HighResScreenshot.h"
#include "Runtime/Core/Public/Misc/DateTime.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"

void UScreenshotWidget::SubmitBugData(UImage* i_Image, FString i_BugReport)
{
	FString fileSaveDirectory = "//155.97.29.109/Bug Reports/BugReport" + FDateTime::Now().ToString() + "/";
	FString imageFileName = fileSaveDirectory + "BugImage";
	FString reportFileName = fileSaveDirectory + "BugReport.txt";
	FString resultPath;
	FHighResScreenshotConfig& hConfig = GetHighResScreenshotConfig();
	hConfig.SaveImage(imageFileName, bitMapData, FIntPoint(X, Y), &resultPath);
	//For Bug report text file
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*fileSaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = reportFileName;

		// Allow overwriting or file doesn't already exist
		if (false || !PlatformFile.FileExists(*AbsoluteFilePath))
		{
			FFileHelper::SaveStringToFile(i_BugReport, *AbsoluteFilePath);
		}
	}

	//DBApplications
}
