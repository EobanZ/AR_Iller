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

	this->SetActorLocation(FVector(0, 0, 0));
	this->SetActorRotation(FRotator(0, 0, 0));

	//Get Video Path
	FString RelativeContentPath = FPaths::GameContentDir();
	std::string RelativeContentPathString = std::string(TCHAR_TO_UTF8(*RelativeContentPath));

	// Open the stream
	stream.open(RelativeContentPathString + "StaudamVid.mp4"); //mit webcam hier einfach "CameraID" in die klammern
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

		LoadConfigFiles();
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

void AWebcamReader::SetCubeReference(UStaticMeshComponent * cubeComponent)
{
	cube = cubeComponent;
}

void AWebcamReader::SetGroundActorReference(AGround* goundActor)
{
	ground = goundActor;
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
	mat.at<float>(0, 2) = *imageWith*0.5; 
	mat.at<float>(1, 1) = cameraMatrix[1][1];
	mat.at<float>(1, 2) = *imageHeight*0.5; 
	mat.at<float>(2, 2) = 1.0; 

	double u0 = *imageWith*0.5; 
	double v0 = *imageHeight*0.5; 
	double fx = cameraMatrix[0][0];
	double fy = cameraMatrix[1][1];
	fovx = (FMath::Atan2(u0, fx) + FMath::Atan2(*imageWith - u0, fx)) * 180.0 / PI;
	fovy = (FMath::Atan2(v0, fy) + FMath::Atan2(*imageHeight - v0, fy)) * 180.0 / PI;

	float aspec = *imageWith / *imageHeight;

	cam->SetFieldOfView(fovx);

	if (cam)
	{
		ResizeBillboard();
		EstimatePosition();
	}

}

void AWebcamReader::LoadConfigFiles()
{
	
	FString path = FPaths::GetPath(FPaths::GetProjectFilePath()); //<-- beim Build vll anderen pfad?
	string Path = TCHAR_TO_UTF8(*path);

#pragma region XML erstellen
	//Camera Calibration
	/*string filename_CamCal = "/CamCalibration.xml";

	cv::FileStorage fs_camMat(Path.append(filename_CamCal), FileStorage::WRITE);

	int wWidth = 1920;
	int wHeight = 1080;

	cv::Mat wCamMatrix;
	wCamMatrix.create(3, 3, CV_64F);
	wCamMatrix.setTo(0.0f);
	wCamMatrix.at<float>(0, 0) = 3.4099954023400387e+03;
	wCamMatrix.at<float>(0, 2) = 960;
	wCamMatrix.at<float>(1, 1) = 3.4099954023400387e+03;
	wCamMatrix.at<float>(1, 2) = 540;
	wCamMatrix.at<float>(2, 2) = 1.0;

	cv::Mat wDistMatrix;
	wDistMatrix.create(1, 5, CV_64F);
	wDistMatrix.setTo(0.0f);
	wDistMatrix.at<float>(0, 0) = 0.0111965;
	wDistMatrix.at<float>(0, 1) = 0.175178;
	wDistMatrix.at<float>(0, 2) = 3.13673;
	wDistMatrix.at<float>(0, 3) = 0.0091511;
	wDistMatrix.at<float>(0, 4) = 0.00573227;

	fs_camMat << "ImageWidth" << wWidth;
	fs_camMat << "ImageHeight" << wHeight;
	fs_camMat << "CameraMatrix" << wCamMatrix;
	fs_camMat << "DistortionMatrix" << wDistMatrix;

	fs_camMat.release();
*/
	//R und T Vector
	/*string filename_RT = "/rtVectors.xml";
	cv::FileStorage fs_rtVec(Path.append(filename_RT), FileStorage::WRITE);

	cv::Mat Rvec;
	Rvec.create(1, 3, CV_64F);
	Rvec.at<float>(0, 0) = 1.697740754667221;
	Rvec.at<float>(0, 1) = 0.08271544218127355;
	Rvec.at<float>(0, 2) = -0.02180714106396032;

	cv::Mat Tvec;
	Tvec.create(1, 3, CV_64F);
	Tvec.at<float>(0, 0) = -339.6517685150949;
	Tvec.at<float>(0, 1) = -41.872098690714;
	Tvec.at<float>(0, 2) = 2091.52018739509;

	fs_rtVec << "R" << Rvec;
	fs_rtVec << "T" << Tvec;

	fs_rtVec.release();*/

#pragma endregion

	cv::FileStorage fs;

	//Load Camera Calibration
	string filename_Calib = "/CamCalibration.xml";
	string calibPath = Path; calibPath = calibPath.append(filename_Calib);

	fs.open(calibPath, FileStorage::READ);

	fs["ImageWidth"] >> *imageWith;
	fs["ImageHeight"] >> *imageHeight;

	cv::Mat rCameraMatrix;
	fs["CameraMatrix"] >> rCameraMatrix;

	cv::Mat rDistortionMatrix;
	fs["DistortionMatrix"] >> rDistortionMatrix;

	// cameramatrix für Iller
	cameraMatrix[0][0] = rCameraMatrix.at<float>(0, 0);
	cameraMatrix[0][1] = rCameraMatrix.at<float>(0, 1);
	cameraMatrix[0][2] = rCameraMatrix.at<float>(0, 2);
	cameraMatrix[1][0] = rCameraMatrix.at<float>(1, 0);
	cameraMatrix[1][1] = rCameraMatrix.at<float>(1, 1);
	cameraMatrix[1][2] = rCameraMatrix.at<float>(1, 2);
	cameraMatrix[2][0] = rCameraMatrix.at<float>(2, 0);
	cameraMatrix[2][1] = rCameraMatrix.at<float>(2, 1);
	cameraMatrix[2][2] = rCameraMatrix.at<float>(2, 2);

	// camera distortion
	cameraDistortion[0] = rDistortionMatrix.at<float>(0, 0);
	cameraDistortion[1] = rDistortionMatrix.at<float>(0, 1);
	cameraDistortion[2] = rDistortionMatrix.at<float>(0, 2);
	cameraDistortion[3] = rDistortionMatrix.at<float>(0, 3);
	cameraDistortion[4] = rDistortionMatrix.at<float>(0, 4);

	fs.release();

	//Load RT Vecotrs
	string filename_RT = "/rtVectors.xml";
	string rtPath = Path; rtPath = rtPath.append(filename_RT);

	fs.open(rtPath, FileStorage::READ);

	cv::Mat rotMatrix = cv::Mat(3,3, CV_64F);
	cv::Mat rotVec = cv::Mat(1,3,CV_64F);
	fs["R"] >> rotVec;
	cv::Rodrigues(rotVec, rotMatrix);
	UE_LOG(LogTemp, Warning, TEXT("RotVector: %f %f %f"), rotVec.at<float>(0,0), rotVec.at<float>(0, 1), rotVec.at<float>(0, 2));

	rotationMatrix[0][0] = rotMatrix.at<float>(0, 0);
	rotationMatrix[0][1] = rotMatrix.at<float>(0, 1);
	rotationMatrix[0][2] = rotMatrix.at<float>(0, 2);
	rotationMatrix[1][0] = rotMatrix.at<float>(1, 0);
	rotationMatrix[1][1] = rotMatrix.at<float>(1, 1);
	rotationMatrix[1][2] = rotMatrix.at<float>(1, 2);
	rotationMatrix[2][0] = rotMatrix.at<float>(2, 0);
	rotationMatrix[2][1] = rotMatrix.at<float>(2, 1);
	rotationMatrix[2][2] = rotMatrix.at<float>(2, 2);

	cv::Mat tVec;
	fs["T"] >> tVec;

	translationVector[0] = tVec.at<float>(0, 0);
	translationVector[1] = tVec.at<float>(0, 1);
	translationVector[2] = tVec.at<float>(0, 2);

	fs.release();



}

void AWebcamReader::ResizeBillboard()
{

	float distance_to_origin = billboard->GetRelativeTransform().GetLocation().Size();

	float width = distance_to_origin * 2.0 * FMath::Tan(FMath::DegreesToRadians(0.5 * fovx));
	float height = width * (float)*imageHeight / (float)*imageWith;

	billboard->SetRelativeScale3D(FVector(0, width / 100.0, height / 100.0));

	UE_LOG(LogTemp, Warning, TEXT("distance to origin: %f, width: %f, height: %f"), distance_to_origin, width, height);

}

void AWebcamReader::EstimatePosition()
{


	planeTransform = FTransform();


	UE_LOG(LogTemp, Warning, TEXT("Vector 1: %f %f %f"), rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]);
	UE_LOG(LogTemp, Warning, TEXT("Vector 2: %f %f %f"), rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]);
	UE_LOG(LogTemp, Warning, TEXT("Vector 3: %f %f %f"), rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]);
	
	FMatrix matrix = FMatrix(FVector(rotationMatrix[0][0], rotationMatrix[1][0], rotationMatrix[2][0]), FVector(rotationMatrix[0][1], rotationMatrix[1][1], rotationMatrix[2][1]), FVector(rotationMatrix[0][2], rotationMatrix[1][2], rotationMatrix[2][2]), FVector(translationVector[0], translationVector[1], translationVector[2]));
	//FMatrix matrix = FMatrix(FVector(0,0 ,0 ), FVector(0,0 ,0 ), FVector(0,0 ,0 ), FVector(-339.6517685150949, -41.872098690714, 2091.52018739509));
	planeTransform.SetFromMatrix(matrix);


	planeTransform.SetScale3D(FVector(1, 1, 1));
	

	//open cv Koordinatensystem in unreal
	CameraAdditionalRotation.SetFromMatrix(FMatrix(FVector(0, 1, 0), FVector(0, 0, -1), FVector(1, 0, 0), FVector(0, 0, 0)));

	planeTransform = planeTransform * CameraAdditionalRotation;


	cube->SetRelativeTransform(planeTransform);

	ground->SetActorTransform(planeTransform);



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

