// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stdio.h>
#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"


#include "Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include "Ground.h"

#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WebcamReader.generated.h"

using namespace cv::xfeatures2d;

UCLASS()
class ARILLER_API AWebcamReader : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWebcamReader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The device ID opened by the Video Stream
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	int32 CameraID;

	// The operation that will be applied to every frame
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	int32 OperationMode;

	// The targeted resize width and height (width, height)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	FVector2D ResizeDeminsions;

	// The rate at which the color data array and video texture is updated (in frames per second)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	float RefreshRate;

	// The refresh timer
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
	float RefreshTimer;

	// Blueprint Event called every time the video frame is updated
	UFUNCTION(BlueprintImplementableEvent, Category = Webcam)
	void OnNextVideoFrame();

	// Change OpenCV operation that will be applied to every frame
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Operations", Keywords = "Change Operation"), Category = Webcam)
	void ChangeOperation();



	// OpenCV fields
	cv::Mat frame;
	cv::VideoCapture stream;
	cv::Size size;

	// OpenCV prototypes
	void UpdateFrame();
	void DoProcessing();
	void UpdateTexture();

	// If the stream has succesfully opened yet
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	bool isStreamOpen;

	// The videos width and height (width, height)
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
	FVector2D VideoSize;

	// The current video frame's corresponding texture
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	UTexture2D* VideoTexture;

	// The current data array
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	TArray<FColor> Data;


	//Calculate FOV
	double cameraMatrix[3][3];
	double cameraDistortion[5];
	int* imageWith = new int; //ohne pointer hats hier immer bugs gegeben
	int* imageHeight = new int;

	double rotationMatrix[3][3];
	double translationVector[3];

	cv::Point2d* principalPoint = new cv::Point2d;
	double fovx;
	double fovy;
	double aspectRatio;
	double focalLenght;
	double billboardDistance = 10000; //100m

	UCameraComponent* cam;
	UStaticMeshComponent* billboard;
	UStaticMeshComponent* cube;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	AGround* ground;

	FTransform planeTransform;

	FTransform CameraAdditionalRotation;

	void CalculateAndSetFOV();

	void LoadConfigFiles();

	void ResizeBillboard();

	void EstimatePosition();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Camera Reference", Keywords = "Set Camera Reference"), Category = Webcam)
	void SetCameraReference(UCameraComponent* cameraComponent);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Billboard Reference", Keywords = "Set Billboard Reference"), Category = Webcam)
	void SetBillboardReference(UStaticMeshComponent* billboardComponent);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Cube Reference", Keywords = "Set Cube Reference"), Category = Webcam)
	void SetCubeReference(UStaticMeshComponent* cubeComponent);

	void SetGroundActorReference(AGround* goundActor);

	//Tracking

	std::vector<cv::Point2f> initPoints;
	std::vector<cv::Point3f> init3dPoints;

	cv::Rect2d* bbox; //Box wird durch den Tracker geupdated
	cv::Ptr<cv::Tracker> tracker = cv::TrackerMedianFlow::create();

	void FindImageWithSURF();

	void Track();


protected:

	// Use this function to update the texture rects you want to change:
	// NOTE: There is a method called UpdateTextureRegions in UTexture2D but it is compiled WITH_EDITOR and is not marked as ENGINE_API so it cannot be linked
	// from plugins.
	// FROM: https://wiki.unrealengine.com/Dynamic_Textures
	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	// Pointer to update texture region 2D struct
	FUpdateTextureRegion2D* VideoUpdateTextureRegion;

};
