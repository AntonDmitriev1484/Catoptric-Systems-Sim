// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/NameTypes.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TargetP.h"
#include "PairSystem.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "LMpair.h"
#include "AntonUtils.h"

#include "GameFramework/Character.h"
#include "UserCharacter.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API AUserCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUserCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* UserCamera;

	void MoveFB(float Axis);
	void MoveRL(float Axis);

	void CallbackLMB();
	void CallbackRMB();
	void ReleaseLMB(); //Means we're done rotating or moving

	APairSystem* controlled_pair_system;
	APairSystem* getControlledPairSystem();

	void placeTargetPoint();
	void callFindConfiguration();
	void callExportConfiguration();

	//We always keep track of the user's target point
	//(where a target point would go if it was placed)
	FHitResult target_hit;
	bool target_hit_present;
	void traceForwards();
	void highlightTarget(FVector target_location, FColor color, int size);

	//Target point editing mode
	bool editing_target_points;
	void toggleTargetPointEditing();

	//Lasso selection mode
	bool lasso_selecting;
	TArray<FVector> selection_points; //Will be converted into a 2D polygon
	void beginLassoSelect();
	void highlightLassoSelection(); //Draws the selection points and connects them by vectors
	void endLassoSelect();

	//We always keep track of the object that might be selected (currently highlighted)
	//and the object(s) that are currently selected
	AActor* highlighted_actor;
	TArray<AActor*> selected_actors;
	void highlightSelected();
	void clearSelected();

	//Manipulating selected groups
	bool moving;
	FVector starting_position;
	FVector transform;
	void highlightTransformOnSelected();
	void moveSelected();


	bool rotating_mirrors; //Maybe this could be the same variable as moving?
	bool rotating_fb;
	void rotateSelectedMirrors();
	


	void mouseMirrorRotationCodeDump();
};
