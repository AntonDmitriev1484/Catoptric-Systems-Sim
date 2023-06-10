// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MIRRORSRAYTRACINGV3_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	//Standard functions that a controller should implement
	AMyPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void toggleMouse();

private:
	bool mouse_selection;
	void mouseClickMirrorRotation();
	
};
