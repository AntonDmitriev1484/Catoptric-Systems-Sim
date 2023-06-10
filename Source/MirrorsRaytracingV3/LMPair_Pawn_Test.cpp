// Fill out your copyright notice in the Description page of Project Settings.


#include "LMPair_Pawn_Test.h"

// Sets default values
ALMPair_Pawn_Test::ALMPair_Pawn_Test()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mirror_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mirror_mesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh_asset(TEXT("StaticMesh'/Game/AddedContent/Mirror.Mirror'"));
	//For the Mirror part of this actor, I'm using the static mesh I made previously in the editor with the modeling tools interface.

	if (mesh_asset.Succeeded())
	{
		mirror_mesh->SetStaticMesh(mesh_asset.Object);
	}
}

// Called when the game starts or when spawned
void ALMPair_Pawn_Test::BeginPlay()
{
	Super::BeginPlay();


	calculateTarget();
	initializeLineTrace(FVector(500, 500, 500));


	//DrawDebugSphere(GetWorld(), this->target, 2, 4, FColor::Red, true, 10000, 3, 1);

	//Testing recalculateSourceStart:
	DrawDebugLine(GetWorld(), this->LT_source_start, this->target, FColor::Blue, true, 10000);
	rotateMirror(FRotator(90, 45, 45));
	DrawDebugLine(GetWorld(), this->LT_source_start, this->target, FColor::Red, true, 10000);
	
}

// Called every frame
void ALMPair_Pawn_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (is_LT_initialized) {
		//Since we're drawing them with each tick, we can leave persistent off
		//this way we don't have to manually clear and re-draw debug line traces every time.
		//They will individually dissappear at the end of the tick, then get re-drawn on the next tick.

		DrawDebugLine(GetWorld(), this->LT_source_start, this->target, FColor::Red);
	}

	//APlayerController.GetHitResultUnderCursor
	//APlayerController::DeprojectMousePositionToWorld(mouse_world_location, mouse_world_direction);

	/*FVector mouse_trace_hit;
	bool is_mouse_over = GetHitResultUnderCursor(ECC_Visibility, false, mouse_trace_hit);*/

}

// Called to bind functionality to input
void ALMPair_Pawn_Test::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//UE_LOG(LogTemp, Warning, TEXT("In SetupInput"));
	//GEngine->AddOnScreenDebugMessage(-1, 1000000, FColor::Red, TEXT("In SetupInput"));


	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindKey(EKeys::G, IE_Pressed, this, &ALMPair_Pawn_Test::toggleMouse);




	//Ok, so this only gets called when the pawn gets posessed by the controller
	//So yes, I do need to make my own controller and my own gamemode

}

void ALMPair_Pawn_Test::toggleMouse() {
	/*UE_LOG(LogTemp, Warning, TEXT("In toggleMouse()"));
	GEngine->AddOnScreenDebugMessage(-1, 1000000, FColor::Red,TEXT("In toggleMouse()"));*/
}




// Calculates the point which the line trace should always target, the center of the flat side of our upside down cone.
void ALMPair_Pawn_Test::calculateTarget() {
	int cone_height = 5;
	this->target = GetActorLocation() + (cone_height * GetActorUpVector());
}

// We want our line trace to adjust as the mirror rotates
// For each rotation of the mirror, the line trace needs to move such that:
// the new line is parallel to the original, but, still hits the mirror's target
// this means that the LT_source_start needs to be re-adjusted
void ALMPair_Pawn_Test::recalculateSourceStart() {
	this->LT_source_start = this->target + (this->LT_length * (this->LT_direction));
}

// Initializes a line trace for this LM Pair.
// Without a line trace present, the mirror will still work, it just won't reflect any line trace.
void ALMPair_Pawn_Test::initializeLineTrace(FVector source_start) {
	this->is_LT_initialized = true;
	this->LT_source_start = source_start;

	FVector line = source_start - this->target;
	this->LT_length = line.Size();
	this->LT_direction = line / line.Size();
	//Gives us a unit vector direction,
	//Whenever the start point is re-adjusted, the new line trace must be parallel to this direction
}

FRotator ALMPair_Pawn_Test::rotateMirror(FRotator new_rotation) {
	FRotator prev_rotation = this->GetActorRotation();
	this->SetActorRelativeRotation(new_rotation);

	//Every time we need to rotate the mirror, we need to re-calculate the target and LT_source_start
	calculateTarget();
	recalculateSourceStart();

	// Returns this mirror's previous rotation
	// useful for stochastic algorithms, allows us to easily undo our random change.
	return prev_rotation;
}

