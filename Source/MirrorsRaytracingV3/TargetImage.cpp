// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetImage.h"


// Sets default values
ATargetImage::ATargetImage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	target_point_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("target_point_mesh"));
	//target_point_mesh->SetupAttachment(RootComponent);
	//this->SetActorEnableCollision(true);
	RootComponent = target_point_mesh;

	//Setting up a collision box so that we can spawn this directly on surfaces
	//USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh_asset(TEXT("StaticMesh'/Game/AddedContent/TargetPointMeshActor.TargetPointMeshActor'"));

	if (mesh_asset.Succeeded())
	{
		target_point_mesh->SetStaticMesh(mesh_asset.Object);
	}
}

// Called when the game starts or when spawned
void ATargetImage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetImage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// A way for PairSystem to pass in extra target points to this TargetImage instance
// Note: This method will crash and give a memory access exception if bind doesn't find a TargetImage instance!
void ATargetImage::addTargetPointActors(TArray<ATargetP*> _target_point_actors) {
	UE_LOG(LogTemp, Warning, TEXT("Added new target point"));

	//Maintain an array to all target point actors (just in case)
	(this->target_point_actors).Append(_target_point_actors);

	/*
	* With the possibility of moving target points
	* this no longer works, as we could change the target point locations
	* and the target_points_arr wouldn't change
	* 
	* updating getTargetPointArray() to re-read all TargetP actors each time its called
	* 
	* NOTE: No longer going to maintain this->target_points_arr
	* 
	//Enqueue the positions of new actors added
		//Append to the end our target points array
	for (int i = 0; i < _target_point_actors.Num(); i++) {
		//target_points.Enqueue(_target_point_actors[i]->GetActorLocation());
		target_points_arr.Add(_target_point_actors[i]->GetActorLocation()); //Memory access violation, isn't reading the pointer properly
	}
	*/
}


TQueue<FVector>* ATargetImage::getTargetPointQueue() {
	//I can return a pointer to it
	//But can't return a copy of the queue itself
	return &target_points;
}

TArray<FVector> ATargetImage::getTargetPointArray() {
	TArray<FVector> arr;
	for (int i = 0; i < target_point_actors.Num(); i++) {
		arr.Add(target_point_actors[i]->GetActorLocation());
	}
	return arr;
}
