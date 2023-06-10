// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "AntonUtils.h"
#include "LMpair.generated.h"

UCLASS()
class MIRRORSRAYTRACINGV3_API ALMpair : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALMpair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	// Line Trace fields
	FVector LT_source_start;
	FVector LT_direction;
	float LT_length;
	bool is_LT_initialized;
	TArray<FVector> LT_hits;

	// Line Trace functions:
	void recalculateSourceStart();
	void calculateTarget();

	// Mirror fields
	bool locked;
	FVector target;

	// Direction
	FVector direction;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UStaticMeshComponent* mirror_mesh;

	//Need a matrix to translate coordinates from UE5's absolute XYZ
	//to every mirror's relative coordinate system (bound by its rotation).
	// -> Translating into the mirror's frame of reference. The mirror's ability
	//		to go up / down, left / right is defined by its own axis from the direction its pointing IRL
	//		the mirror's coordinate system doesn't necessarily align with the XYZ default of UE5.

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* basis_z_arrow_component;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* basis_y_arrow_component;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* basis_x_arrow_component;

	FMatrix irl_mirror_basis;// = FBasisVectorMatrix(EForceInit::ForceInitToZero);


	// 1D mirror numbering has to be manual! No real better way to do it.
	UPROPERTY(EditAnywhere)
	int user_numbering_1D;

	// 2D mirror numbering has to be manual! No real better way to do it.
	UPROPERTY(EditAnywhere)
	FVector2D user_numbering_2D;

	// Line Trace functions called by PairSystem
	void initializeLineTrace(FVector source_start);
	void performLineTrace(bool debug_lines, bool debug_spheres); 
	TArray<FVector> getHits();

	// Mirror functions:
	void toggleLock();
	bool isLocked();
	FRotator rotateMirror(FRotator new_rotation);
	void rotateMirrorToVector(FVector new_up_vector);
	FRotator randomRotateMirror();

	//Help me in rotateMirrorTowardsTarget (direct optimization) in PairSystem
	FVector getLTDirection();
	FVector getTarget();

	FRotator getRotationRelativeToZeroVector();

};
