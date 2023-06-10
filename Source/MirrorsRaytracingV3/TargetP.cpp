// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetP.h"

// Sets default values
ATargetP::ATargetP()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
void ATargetP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATargetP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

