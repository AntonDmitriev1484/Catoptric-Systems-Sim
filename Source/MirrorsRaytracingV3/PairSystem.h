// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AUserCharacter;
//Forward declaration
//To update UserCharacter's PairSystem pointer
//we need to define the that type here
//but in UserCharacter, we need to have a PairSystem
//type defined so that it can call findConfig


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NameTypes.h"
#include "LMpair.h"
#include "TargetImage.h"
#include "TargetP.h"
#include "Engine/DirectionalLight.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Templates/Tuple.h"
#include "GameFramework/PlayerController.h"
#include "FindConfigTracker.h"

#include "AntonUtils.h"
#include "EngineUtils.h"

#include "Engine/TriggerVolume.h"
#include "PairSystem.generated.h"

UENUM()
enum SystemAlgoTypes {
	NONE = 0,
	STOCHASTIC = 1,
	DIRECT = 2,
};


UCLASS()
class MIRRORSRAYTRACINGV3_API APairSystem : public ATriggerVolume
{
	//public AActor
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APairSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
	//Set the PairSystem pointer to of any player character that walk through it to 'this'

	TArray<ALMpair*> pairs;
	ATargetImage* target_image;
	AActor* light_direction_actor;

	AFindConfigTracker* algo;

	//Initialization
	//Connecting to LMpair instances that already exist in the game
	FString bind_tag;
	void bindByTag(); //Uses tags set in editor
	void bindByLoc(); //Uses the fact that PairSystem is a volume
	void initializeLMpairs();
	void testBind();

	//General use
	TArray<FVector> readHitPoints(); //Will return all hit points NOT projected onto plane
	TArray<FVector> readHitPointsUnlocked();

	//File I/O
	void exportConfiguration();

	//Default stochastic optimization
	void findConfiguration_1(const int threshold);

	void findConfiguration_1_Tick(const int threshold);
	bool find_configuration_1;
	int fC1_threshold;
	int fC1_iteration;
	int fC1_best_score;
	TTuple<int, FRotator> fC1_changed_previous_rotator;

	TTuple<int, FRotator> rotateRandomMirror();
	int compare();

	//Stochastic optimization with mirror locking
	// ...

	//Default direct optimization
	void findConfiguration_2_Tick();
	TTuple<int, FVector> closestHitPoint(); //Calculates closest hit point to our current target point
	void rotateMirrorTowardsTarget(int mirror_num, FVector hit_point);

	//Keeping track of locks
	int locked_mirrors;

	//Utilities
	void drawPoints(TArray<FVector> points, FColor color); // <- Viz utils

};