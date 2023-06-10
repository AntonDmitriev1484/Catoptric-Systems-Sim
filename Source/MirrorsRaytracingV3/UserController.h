// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CoreMinimal.h"
#include "UserController.generated.h"


/**
 * 
 */
UCLASS()
class MIRRORSRAYTRACINGV3_API AUserController : public APlayerController
{
	GENERATED_BODY()

//public:
//
//	AUserController();
//
//	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//	//	class USpringArmComponent* CameraBoom;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//	class UCameraComponent* UserCamera;
//
//	virtual void BeginPlay() override;
//	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
//	virtual void Tick(float DeltaTime) override;
//
//
//	void MoveFB(float Axis);
//	void MoveRL(float Axis);
	
};
