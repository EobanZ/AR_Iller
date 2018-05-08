// Fill out your copyright notice in the Description page of Project Settings.

#include "Ground.h"
#include "WebcamReader.h"




void AGround::BeginPlay()
{
	

	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AWebcamReader* webcamReader = Cast<AWebcamReader>(pawn);
	webcamReader->SetGroundActorReference(this);

}
