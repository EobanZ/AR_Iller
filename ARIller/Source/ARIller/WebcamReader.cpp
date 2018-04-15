// Fill out your copyright notice in the Description page of Project Settings.
#include "WebcamReader.h"
#include "ARIller.h"

#include <iostream>

using namespace cv;
using namespace std;


// Sets default values
AWebcamReader::AWebcamReader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize OpenCV and webcam properties
	CameraID = 0;
	OperationMode = 0;
	RefreshRate = 25;
	isStreamOpen = false;
	VideoSize = FVector2D(0, 0);
	ShouldResize = false;
	ResizeDeminsions = FVector2D(320, 240);
	RefreshTimer = 0.0f;
	stream = cv::VideoCapture();
	frame = cv::Mat();	

}

// Called when the game starts or when spawned
void AWebcamReader::BeginPlay()
{
	Super::BeginPlay();

	//Get Video Path
	FString RelativeContentPath = FPaths::GameContentDir();
	std::string RelativeContentPathString = std::string(TCHAR_TO_UTF8(*RelativeContentPath));

	// Open the stream
	stream.open(RelativeContentPathString + "Iller.m4v"); //mit webcam hier einfach "CameraID" in die klammern
	if (stream.isOpened())
	{
		isStreamOpen = true;
		UpdateFrame();
		
		VideoSize = FVector2D(frame.cols, frame.rows);
		size = cv::Size(ResizeDeminsions.X, ResizeDeminsions.Y);
		VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);
		VideoTexture->UpdateResource();
		VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VideoSize.X, VideoSize.Y);
		

		// Initialize data array
		Data.Init(FColor(0, 0, 0, 255), VideoSize.X * VideoSize.Y);

		// Do first frame
		DoProcessing();
		UpdateTexture();
		OnNextVideoFrame();

		LoadConfigFile();
		CalculateAndSetFOV();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Didnt open stream"));
	}

	billboard->SetRelativeLocation(FVector(billboardDistance, 0, 0));
	
}

// Called every frame
void AWebcamReader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshTimer += DeltaTime;
	if (isStreamOpen && RefreshTimer >= 1.0f / RefreshRate)
	{
		RefreshTimer -= 1.0f / RefreshRate;
		UpdateFrame();
		DoProcessing();
		UpdateTexture();
		OnNextVideoFrame();
	}
	

}

void AWebcamReader::ChangeOperation()
{
	OperationMode++;
	OperationMode %= 3;
}

void AWebcamReader::SetCameraReference(UCameraComponent* cameraComponent)
{
	this->cam = cameraComponent;
}

void AWebcamReader::SetBillboardReference(UStaticMeshComponent* billboardComponent)
{
	this->billboard = billboardComponent;
}

void AWebcamReader::UpdateFrame()
{
	if (stream.isOpened())
	{
		stream.read(frame);
		if (ShouldResize)
		{
			cv::resize(frame, frame, size);
		}
	}
	else
	{
		isStreamOpen = false;
	}
}

void AWebcamReader::DoProcessing()
{
	// TODO: Do any processing with frame here!

	if (OperationMode == 0)
	{
		// Apply nothing
	}
	else if (OperationMode == 1)
	{
		// Apply median blur
		cv::Mat src = frame.clone();
		cv::medianBlur(src, frame, 7);
	}
	else if (OperationMode == 2)
	{
		cv::Mat src, dst;
		cv::cvtColor(frame, src, cv::COLOR_RGB2GRAY);

		int thresh = 50;

		cv::Canny(src, dst, thresh, thresh * 2, 3);
		cv::cvtColor(dst, frame, cv::COLOR_GRAY2BGR);
	}
}

void AWebcamReader::UpdateTexture()
{
	if (isStreamOpen && frame.data)
	{
		// Copy Mat data to Data array
		for (int y = 0; y < VideoSize.Y; y++)
		{
			for (int x = 0; x < VideoSize.X; x++)
			{
				int i = x + (y*VideoSize.X);
				Data[i].B = frame.data[i * 3 + 0];
				Data[i].G = frame.data[i * 3 + 1];
				Data[i].R = frame.data[i * 3 + 2];
			}
		}
		
		// Update texture 2D
		UpdateTextureRegions(VideoTexture, (int32)0, (uint32)1, VideoUpdateTextureRegion, (uint32)(4 * VideoSize.X), (uint32)4, (uint8*)Data.GetData(), false);
	}

}

void AWebcamReader::CalculateAndSetFOV()
{
	
	double tempFOVx = -1;
	double tempFOVy = -1;
	double tempFocalLenght = -1;
	double tempAspectRatio = -1;

	UE_LOG(LogTemp, Warning, TEXT("tempFOVx vor funktion: %d"),tempFOVx);
	
	cv::Mat camMatrix = cv::Mat();
	camMatrix =	cv::Mat(3, 3, CV_64F, cameraMatrix);
	for (int i = 0; i<3;i++)
	{
		for (int j = 0; j<3;j++)
		{
			UE_LOG(LogTemp, Warning, TEXT("camMatrix: %i %i: %f"), i,j,camMatrix.at<float>(i,j));
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("image Size: w: %i h:%i"), *imageWith, *imageHeight);
	UE_LOG(LogTemp, Warning, TEXT("apertureWidth: %f"), *apertureWidth);
	UE_LOG(LogTemp, Warning, TEXT("apertureHeight: %f"), *apertureHeight);
	

	cv::Size imageSize(*imageWith, *imageHeight);

	cv::calibrationMatrixValues(camMatrix, imageSize, *apertureWidth, *apertureHeight, tempFOVx, tempFOVy, tempFocalLenght, *principalPoint, tempAspectRatio);

	fovx = (float)tempFOVx;
	fovy = (float)tempFOVy;
	focalLenght = (float)tempFocalLenght;
	aspectRatio = (float)tempAspectRatio;

	UE_LOG(LogTemp, Warning, TEXT("tempFOVx: %f"), tempFOVx);
	UE_LOG(LogTemp, Warning, TEXT("fovx var: %f"), fovx);
	UE_LOG(LogTemp, Warning, TEXT("fovx var: %f"), fovx);

	if (cam) 
	{
		//cam->SetFieldOfView(fovx/2); // ist noch nicht richtig. bei 180 schwarzer bildschirm
		//cam->SetAspectRatio(1+1/aspectRatio);

		ResizeBillboard();

		UE_LOG(LogTemp, Warning, TEXT("Field of fiew was set to: %f"), cam->FieldOfView);
		UE_LOG(LogTemp, Warning, TEXT("Aspect Ratio was set to: %f"), cam->AspectRatio);
	}
	

	
}

void AWebcamReader::LoadConfigFile()
{
	//read cameramatrix, apertureWidth/Height
	*imageWith = 1920;
	*imageHeight = 1080;

	*apertureWidth = 1.f;
	*apertureHeight = 1.f;
	
	//temp cameramatrix
	cameraMatrix[0][0] = 3375.38;
	cameraMatrix[0][1] = 0;
	cameraMatrix[0][2] = 996.988;
	cameraMatrix[1][0] = 0;
	cameraMatrix[1][1] = 3411.35;
	cameraMatrix[1][2] = 649.654;
	cameraMatrix[2][0] = 0;
	cameraMatrix[2][1] = 0;
	cameraMatrix[2][2] = 1;

	//temp camera distortion
	cameraDistortion[0] = 0.0111965;
	cameraDistortion[1] = 0.175178;
	cameraDistortion[2] = 3.13673;
	cameraDistortion[3] = 0.0091511;
	cameraDistortion[4] = 0.00573227;
		

}

void AWebcamReader::ResizeBillboard()
{
	float calculatedSize;

	float fbeta = cam->FieldOfView/2;
	float falpha = 180 - 90 - fbeta;
	calculatedSize = billboardDistance*FMath::Sin(fbeta) / FMath::Sin(falpha);

	UE_LOG(LogTemp, Warning, TEXT("beta: %f"), fbeta);
	UE_LOG(LogTemp, Warning, TEXT("alpha: %f"), falpha);
	UE_LOG(LogTemp, Warning, TEXT("Aspect Ratio was set to: %f"), cam->AspectRatio);
	UE_LOG(LogTemp, Warning, TEXT("sin beta: %f"), FMath::Sin(fbeta));
	UE_LOG(LogTemp, Warning, TEXT("sin alpha: %f"), FMath::Sin(falpha));
	//billboard->SetWorldScale3D(FVector(16 * 100000, 16*100000, 16 * 100000));
	billboard->SetRelativeScale3D(FVector(.1f, billboardDistance, billboardDistance * cam->AspectRatio));
}

void AWebcamReader::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
				{
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip)
					{
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
					}
				}
		if (bFreeData)
		{
			FMemory::Free(RegionData->Regions);
			FMemory::Free(RegionData->SrcData);
		}
		delete RegionData;
			});
	}
}

