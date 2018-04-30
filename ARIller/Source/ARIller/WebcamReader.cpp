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
	RefreshRate = 60;
	isStreamOpen = false;
	VideoSize = FVector2D(0, 0);
	ShouldResize = false;
	ResizeDeminsions = FVector2D(1920, 1080);
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
	stream.open(RelativeContentPathString + "MarkerPic.mp4"); //mit webcam hier einfach "CameraID" in die klammern
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
		UE_LOG(LogTemp, Error, TEXT("Didnt open stream"));
	}



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
	billboard->SetRelativeLocation(FVector(billboardDistance, 0, 0));

	cv::Mat camMatrix = cv::Mat();
	camMatrix = cv::Mat(3, 3, CV_64F, Scalar(0));
	cv::Mat mat = cv::Mat();
	mat.create(3, 3, CV_64F);
	mat.setTo(0.0);
	mat.at<float>(0, 0) = cameraMatrix[0][0];
	mat.at<float>(0, 2) = cameraMatrix[0][2];
	mat.at<float>(1, 1) = cameraMatrix[1][1];
	mat.at<float>(1, 2) = cameraMatrix[1][2];
	mat.at<float>(2, 2) = cameraMatrix[2][2];

	UE_LOG(LogTemp, Warning, TEXT("fovx: %f"), fovx);
	UE_LOG(LogTemp, Warning, TEXT("fovy: %f"), fovy);

	//cv::calibrationMatrixValues(mat, cv::Size(*imageWith, *imageHeight), 0, 0, fovx, fovy, focalLenght, *principalPoint, aspectRatio);
	fovx = (2 * FMath::Atan(1920 / (2 * 3375.35)))*180/PI;
	fovy = (2 * FMath::Atan(1080 / (2 * 3411.35))) * 180 / PI;
	cam->SetFieldOfView(fovx);


	/*fovx = (double) FMath::Atan2(2 * cameraMatrix[0][0], *imageWith)* 180.0 / PI;
	fovy = (double)FMath::Atan2(2 * cameraMatrix[1][1], *imageHeight)*180.0/PI;*/
	UE_LOG(LogTemp, Warning, TEXT("fovx_new: %f"), fovx);
	UE_LOG(LogTemp, Warning, TEXT("fovy_new: %f"), fovy);

	if (cam)
	{

		ResizeBillboard();

	}



}

void AWebcamReader::LoadConfigFile()
{
	////Iller Video
	//*imageWith = 1920;
	//*imageHeight = 1080;

	////temp cameramatrix
	//cameraMatrix[0][0] = 3375.38;
	//cameraMatrix[0][1] = 0;
	//cameraMatrix[0][2] = 966.988;
	//cameraMatrix[1][0] = 0;
	//cameraMatrix[1][1] = 3411.35;
	//cameraMatrix[1][2] = 649.654;
	//cameraMatrix[2][0] = 0;
	//cameraMatrix[2][1] = 0;
	//cameraMatrix[2][2] = 1.0f;

	////temp camera distortion
	//cameraDistortion[0] = 0.0111965;
	//cameraDistortion[1] = 0.175178;
	//cameraDistortion[2] = 3.13673;
	//cameraDistortion[3] = 0.0091511;
	//cameraDistortion[4] = 0.00573227;


	//Marker Bild
	*imageWith = 1280;
	*imageHeight = 720;

	cameraMatrix[0][0] = 969.422;
	cameraMatrix[0][1] = 0;
	cameraMatrix[0][2] = 621.848;
	cameraMatrix[1][0] = 0;
	cameraMatrix[1][1] = 976.901;
	cameraMatrix[1][2] = 369.637;
	cameraMatrix[2][0] = 0;
	cameraMatrix[2][1] = 0;
	cameraMatrix[2][2] = 1.0f;



}

void AWebcamReader::ResizeBillboard()
{
	/*float calculatedSize;

	float fbeta = cam->FieldOfView / 2;
	float falpha = 180 - 90 - fbeta;
	calculatedSize = billboardDistance * FMath::Sin(fbeta) / FMath::Sin(falpha);
	UE_LOG(LogTemp, Warning, TEXT("MEINE_LOG_VON_fov: %f"), cam->FieldOfView);
	UE_LOG(LogTemp, Warning, TEXT("fov: %f"), cam->FieldOfView);
	UE_LOG(LogTemp, Warning, TEXT("Calculate BillboardSize With: %f"), calculatedSize);
	UE_LOG(LogTemp, Warning, TEXT("Aspect Ratio: %f"), cam->AspectRatio);

	float billboardScalefactorX = calculatedSize;
	float billboardScalefactorY = (calculatedSize* (1 / cam->AspectRatio));

	billboard->SetRelativeScale3D(FVector(1, billboardScalefactorX, billboardScalefactorY));*/

	float distance_to_origin = billboard->GetRelativeTransform().GetLocation().Size();

	float width = distance_to_origin * 2.0 * FMath::Tan(FMath::DegreesToRadians(0.5 * fovx));
	float height = width * (float)*imageHeight / (float)*imageWith;

	billboard->SetRelativeScale3D(FVector(1, width / 100.0, height / 100.0));

}

void AWebcamReader::EstimatePosition()
{
	cv::Mat Rvec;
	cv::Mat_<float> Tvec;
	cv::Mat raux, taux;

	//Sp�ter mit �bergabeparametern berechnen
	//if (!cv::solvePnP(points3d, points2d, camMatrix, disCoeff, raux, taux)) return;

	/*raux.convertTo(Rvec, CV_32F);
	taux.convertTo(Tvec, CV_32F);

	cv::Mat_<float> rotMat(3, 3);
	cv::Rodrigues(Rvec, rotMat);*/

	//den Cube/das Object mit �bergeben um die transform �ndern zu k�nnen?

	float rArray[9] = { 0.98007, -0.08268, -0.18065
						- 0.19867, -0.40785, -0.89117,
						0.00000, 0.90930, -0.41615 };
	float tArray[3] = { 140, 50, 350 };

	cv::Mat rotMat = Mat(3, 3, CV_32FC1, rArray);
	cv::Mat tVec = Mat(1, 3, CV_32FC1, tArray);

	planeTransform = FTransform();

	//for (int col = 0; col < 3; col++)
	//{
	//	for (int row = 0; row < 3; row++)
	//	{
	//		m.transformation.r().mat[row][col] = rotMat(row, col); // Copy rotation component
	//	}
	//	m.transformation.t().data[col] = Tvec(col); // Copy translation component
	//}

	planeTransform = planeTransform * CameraAdditionalRotation;
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

