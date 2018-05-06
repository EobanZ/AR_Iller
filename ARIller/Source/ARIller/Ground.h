// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Classes/Engine/World.h"
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Ground.generated.h"

/**
 * 
 */
UCLASS()
class ARILLER_API AGround : public AStaticMeshActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	
};
