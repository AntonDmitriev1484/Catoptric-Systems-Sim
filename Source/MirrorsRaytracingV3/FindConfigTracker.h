// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FindConfigTracker.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API AFindConfigTracker : public AActor
{
	GENERATED_BODY()

	//Note: This should be a struct
		// USTRUCT just doesn't want to work
		// so we're using UACTOR instead
		// Implementing this as a class doesnt really change that much
		// in terms of code concision
	
public:	
	// Sets default values for this actor's properties
	AFindConfigTracker();
	bool active;
	int id; //use like an enum, algo.id == STOCHASTIC
	int iteration;

	//Only used for stochastic optimization
	int threshold;
	int best_score;
	TTuple<int, FRotator> changed_previous_rotator;

	//Only used for direct optimization
	int target_point_index;

	void init(int _id, int _threshold);
	void init(int _id);
	void init();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
