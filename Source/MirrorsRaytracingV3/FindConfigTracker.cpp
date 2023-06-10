// Fill out your copyright notice in the Description page of Project Settings.


#include "FindConfigTracker.h"

// Sets default values
AFindConfigTracker::AFindConfigTracker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void AFindConfigTracker::init(int _id, int _threshold) {
	//MY BRAIN IS CHEESE COME BACK TO THIS LATER
}

void AFindConfigTracker::init(int _id) {
	id = _id;
	active = true;
	iteration = 0;
	target_point_index = 0;
}

void AFindConfigTracker::init() {
	id = 0;
	active = false;
	iteration = 0;
	target_point_index = 0;
}

// Called when the game starts or when spawned
void AFindConfigTracker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFindConfigTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

