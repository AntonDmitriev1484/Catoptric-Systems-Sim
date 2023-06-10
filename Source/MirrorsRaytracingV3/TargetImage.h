// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetP.h"
#include "TargetImage.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API ATargetImage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetImage();
	UStaticMeshComponent* target_point_mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<ATargetP*> target_point_actors;
	TQueue<FVector> target_points;
	TArray<FVector> target_points_arr; //NOTE: No longer invariant, or kept updated in code

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void addTargetPointActors(TArray<ATargetP*> _target_point_actors);
	TQueue<FVector>* getTargetPointQueue();
	TArray<FVector> getTargetPointArray(); // <- Because getTargetPointQueue has been misbehavin

	//Note: If a point that was added in the editor, gets moved at runtime
	//this class WILL NOT update the associated target_point in the queue.
};
