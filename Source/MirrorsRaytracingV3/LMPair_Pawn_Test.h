// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LMPair_Pawn_Test.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API ALMPair_Pawn_Test : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALMPair_Pawn_Test();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void calculateTarget();
	void recalculateSourceStart();

	FVector target;

	FVector LT_source_start;
	FVector LT_direction;
	float LT_length;
	bool is_LT_initialized;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaticMeshComponent* mirror_mesh;
	void initializeLineTrace(FVector source_start);
	FRotator rotateMirror(FRotator new_rotation);
	void toggleMouse();

};
