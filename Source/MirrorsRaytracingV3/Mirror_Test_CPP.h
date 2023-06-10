// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Mirror_Test_CPP.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API AMirror_Test_CPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMirror_Test_CPP();

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
	UStaticMeshComponent* mirror_mesh;
	void initializeLineTrace( FVector source_start);
	FRotator rotateMirror(FRotator new_rotation);

};
