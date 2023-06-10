// Fill out your copyright notice in the Description page of Project Settings.


#include "UserCharacter.h"

// Credit for getting me started: https://awesometuts.com/blog/character-movement-unreal-engine/

// Sets default values
AUserCharacter::AUserCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UserCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	int height_boost = 65; //Boost required to get our eye level to around 5'8 in IRL units
	UserCamera->SetRelativeLocation(FVector(0, 0, height_boost)); 
	//Moves the camera up 65 units relative to its original position attached to root component
	UserCamera->SetupAttachment(RootComponent);
	this->BaseEyeHeight += height_boost; //Not important
}

// Called when the game starts or when spawned
void AUserCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUserCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	traceForwards();
	highlightSelected();
	highlightTransformOnSelected();

	if (moving) {
		
		transform = target_hit.Location - starting_position;
		//DONE WITH THIS FOR NOW
		//need to de-jankify this code
		UAntonUtils::print(FString("Transform: "), this->transform);
	}


}

// Called to bind functionality to input
void AUserCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Movement:
	PlayerInputComponent->BindAxis("MoveFB", this, &AUserCharacter::MoveFB);
	PlayerInputComponent->BindAxis("MoveRL", this, &AUserCharacter::MoveRL);
	//Use controller rotation yaw is already true
	PlayerInputComponent->BindAxis("TurnHoriz", this, &APawn::AddControllerYawInput);
	this->bUseControllerRotationPitch = true;
	PlayerInputComponent->BindAxis("TurnVert", this, &APawn::AddControllerPitchInput);

	//PairSystem functionality:
	//These are actions, not axes. There is a big difference in axis inputs (continuously refreshed) vs action inputs, fire every key press
	PlayerInputComponent->BindAction("PlaceTargetPoint", IE_Pressed, this, &AUserCharacter::toggleTargetPointEditing); //Maybe make it a toggle
	PlayerInputComponent->BindAction("FindConfiguration", IE_Pressed, this, &AUserCharacter::callFindConfiguration);
	PlayerInputComponent->BindAction("ExportConfiguration", IE_Pressed, this, &AUserCharacter::callExportConfiguration);

	PlayerInputComponent->BindAction("ClickLMB", IE_Pressed, this, &AUserCharacter::CallbackLMB);
	PlayerInputComponent->BindAction("ClickLMB", IE_Released, this, &AUserCharacter::ReleaseLMB);

	PlayerInputComponent->BindAction("ClickRMB", IE_Pressed, this, &AUserCharacter::CallbackRMB);

	PlayerInputComponent->BindAction("LassoSelect", IE_Pressed, this, &AUserCharacter::beginLassoSelect);
	PlayerInputComponent->BindAction("LassoSelect", IE_Released, this, &AUserCharacter::endLassoSelect);

	PlayerInputComponent->BindAction("Clear", IE_Pressed, this, &AUserCharacter::clearSelected);
}

void AUserCharacter::MoveFB(float Axis) {
	//Make it so that we only rotate along one axis at a time
	if (!lasso_selecting) {
		if (rotating_mirrors) {
			Axis -= 1;
			//Temporary while the way to do it with a mouse is unclear to me
			//Because we're transforming the pre-existing up vector relative to our forward vector
			//not replacing it with some function of our forward vector
			FVector axis = this->GetActorForwardVector();
			//So Axis is 0 if no key is being pressed, and some value, if it is

			//WHY IS THIS SO CRINGE OH MY LORD
			
			//axis is our vector. So now I multiply these two together for motion to only occur when a key is pressed.
			this->transform = this->transform.RotateAngleAxis(1, Axis*FVector(1,0,0));
		}
		else {
			FRotator Rotation = Controller->GetControlRotation();
			FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
			FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(ForwardDirection, Axis);
		}

	}
}
void AUserCharacter::MoveRL(float Axis) {
	//Note: This gets passed the same Axis variable at the same time!
	//so we need to add an extra boolean to only be able to tilt one way at a time

	if (!lasso_selecting) {
		if (rotating_mirrors) {
			//rotating_fb = false;
			//Axis -= 1;
			// 
			//FVector axis = this->GetActorForwardVector().RotateAngleAxis(90, FVector(0,0,1));
			//Perpendicular to forwards backwards motion, we want to rotate the forward vector 90 degrees about the Z
			//this->transform = this->transform.RotateAngleAxis(1, Axis*FVector(0,1,0));
		}
		else {
			FRotator Rotation = Controller->GetControlRotation();
			FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
			FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(RightDirection, Axis);
		}

	}
}

// Returns the PairSystem that this user will have the ability to control.
APairSystem* AUserCharacter::getControlledPairSystem() {
	//For now, this will just always return "test"
	//we should also cache the pointer somewhere, to avoid repeatedly using actor iterator
	//ideally we wont even have to use actor iterator, and can just check if we're standing in
	//the pair system's volume

	/*
	APairSystem* pair_system = 0;

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//Retrieve actor pointer from iterator
		AActor* MyActor = *ActorItr;

		//Get actor UClass name
		FString name = MyActor->GetClass()->GetName();

		//If this AActor has a tag that associates it with our pair system
		//We want to connect it to this pair system instance some how
		if (MyActor->ActorHasTag(FName("Test"))) {

			if (name.Equals(FString("PairSystem"))) {
				UE_LOG(LogTemp, Warning, TEXT("User character has found pair system"));
				//If the actor is an LMpair, we know we can safely cast our type from AActor -> ALMpair
				pair_system = Cast<APairSystem>(MyActor);
			}
		}
	}
	*/

	return this->controlled_pair_system;
}

void AUserCharacter::traceForwards() {
	// Get forward vector
	// Get actor position
	// Line trace forward from our position by some distance
	FVector direction = this->GetActorForwardVector();
	FVector start = this->GetActorLocation();
	FVector end = this->GetActorLocation() + direction * 10000;

	FHitResult new_target_hit;

	FCollisionQueryParams TraceParams;
	this->target_hit_present = GetWorld()->LineTraceSingleByChannel(new_target_hit, start, end, ECC_Visibility, TraceParams);

	if (target_hit_present) {
		int adjust = 8;
		this->target_hit.Location = new_target_hit.Location + adjust * (new_target_hit.ImpactNormal); 
		//We want our target sphere to never spawn "inside" a surface, but always be right up against it.
		//This is just so we can get them to spawn normally, because by default collision at spawn point prevents spawning
		//To do this, I'm moving it 13 units away from the surface at the point of impact
		//Ideally 13 is actually the size of the target point

		//YES! This little offset fixed our spawning problem
		//FVector t = target_hit.Location + 10*(target_hit.ImpactNormal);

		AActor* target_hit_actor = new_target_hit.GetActor();
		FString target_hit_actor_class_name = target_hit_actor->GetClass()->GetName();

		if (this->editing_target_points) {
			if (target_hit_actor_class_name.Equals("TargetP")) {
				//Highlight around the target point or lm pair
				highlightTarget(target_hit_actor->GetActorLocation(), FColor::Yellow, 14);
				this->highlighted_actor = target_hit_actor;
			}
			else {
				//Otherwise, display where the target hit would go
				highlightTarget(this->target_hit.Location, FColor::Blue, 3);
				this->highlighted_actor = NULL;
			}

		}
		else {
			if (target_hit_actor_class_name.Equals("LMpair")) {
				UAntonUtils::print(FString("Highlighting LMPair"));
				//Highlight around the target point or lm pair
				highlightTarget(target_hit_actor->GetActorLocation(), FColor::Yellow, 14);
				this->highlighted_actor = target_hit_actor;
			}
			else {
				this->highlighted_actor = NULL;
			}
		}

		//this->highlighted_actor = target_hit_actor;
		
	}
}

void AUserCharacter::highlightTarget(FVector target_location, FColor color, int size) {
	DrawDebugSphere(GetWorld(), target_location, size, 8, color);
}
void AUserCharacter::highlightSelected() {
	for (int i = 0; i < selected_actors.Num(); i++) {
		highlightTarget(selected_actors[i]->GetActorLocation(), FColor::Green, 14);
	}
}
void AUserCharacter::clearSelected() {
	//Empty might call destructor on elements
	//we just want to remove all of them from the array, not destroy them
	this->selected_actors.RemoveAllSwap( //RemoveAllSwap doesn't preserve order of elements
		[](AActor* val) { //Apparently this is how predicates work in C++
			return true;
		});
}

void AUserCharacter::ReleaseLMB() {
	if (this->moving) {

		this->moving = false;
		moveSelected();
	}

	if (this->rotating_mirrors) {
		this->rotating_mirrors = false;
		rotateSelectedMirrors();
	}
}

void AUserCharacter::CallbackLMB() {

	if (editing_target_points) {
		if (!this->moving) {
			if (highlighted_actor != NULL) { //If we click LMB on a highlighted actor
				this->moving = true; //Set moving to true, it'll turn off when we release LMB
				this->starting_position = highlighted_actor->GetActorLocation();
			}
			else { //If we click LMB not on a target point, it just places a target point
				placeTargetPoint();
			}
		}

	}
	else {
		if (highlighted_actor != NULL) {
			UE_LOG(LogTemp, Warning, TEXT("Actor highlighted on LMB click"));

			this->rotating_mirrors = true;
			this->transform = highlighted_actor->GetActorUpVector() * 40;

			starting_position = highlighted_actor->GetActorLocation();
		}


	}
}
void AUserCharacter::CallbackRMB() {
	//Behavior changes depending on whether or not
	//we're in target point editing mode

	if (lasso_selecting) {



	}
	else { //Single selecting
		if (highlighted_actor != NULL) {
			if (editing_target_points) {
				if (highlighted_actor->GetClass()->GetName().Equals("TargetP")) {
					selected_actors.Add(highlighted_actor);
				}
			}
			else {
				if (highlighted_actor->GetClass()->GetName().Equals("LMpair")) {
					selected_actors.Add(highlighted_actor);
				}
			}
		}
	}
}

void AUserCharacter::beginLassoSelect() {
	this->lasso_selecting = true;
}
void AUserCharacter::endLassoSelect() {
	this->lasso_selecting = false;
}

void AUserCharacter::toggleTargetPointEditing() {
	this->editing_target_points = !this->editing_target_points;
	//Need to clear the selection.
	//Never want to have target points and mirrors selected at the same time
	clearSelected();
}
void AUserCharacter::placeTargetPoint() {
	// Spawn an actor at the hit we have traced from our actor

	if (this->target_hit_present) {

		const FVector spawn_loc = this->target_hit.Location;

		//We want to make sure noCollisionFail is true
		//https://docs.unrealengine.com/5.0/en-US/spawning-actors-in-unreal-engine/
		ATargetP* target_point = GetWorld()->SpawnActor<ATargetP>(ATargetP::StaticClass(), spawn_loc, FRotator(0, 0, 0));
	
		//Now add this point into the target image
		//Assume, we only know what PairSystem we're in

		//The pair_system we're currently working in / changing through our UserCharacter
		APairSystem* pair_system = getControlledPairSystem();

		target_point->Tags.Add(FName(pair_system->bind_tag)); //Add the pair system bind_tag onto this target point for consistency

		TArray<ATargetP*> target_points;
		target_points.Add(target_point);
		pair_system->target_image->addTargetPointActors(target_points); //Add this target point into the pair system
	}
}
void AUserCharacter::callFindConfiguration() {
	getControlledPairSystem()->findConfiguration_2_Tick();
}
void AUserCharacter::callExportConfiguration() {
	UAntonUtils::print("Exporting Configuration");
	getControlledPairSystem()->exportConfiguration();
}


void AUserCharacter::highlightTransformOnSelected() {
	if (moving || rotating_mirrors) {
		for (int i = 0; i < selected_actors.Num(); i++) {
			FVector l = selected_actors[i]->GetActorLocation();
			/*
			UE_LOG(LogTemp, Warning, TEXT("Transform: %f %f %f"), transform.X, transform.Y, transform.Z);*/
			DrawDebugDirectionalArrow(GetWorld(), l, l + transform, 10, FColor::Green);
		}
	}
}
void AUserCharacter::moveSelected() {
	auto it = selected_actors.CreateIterator(); //Ape discovers how to use iterator, proceeds to not use iterator
	
	for (int i = 0; i < selected_actors.Num(); i++) {
		UAntonUtils::print(FString("Transform: "), this->transform);
		FVector l = selected_actors[i]->GetActorLocation();
		UAntonUtils::print(FString("Change Vec: "), l + this->transform);
		selected_actors[i]->SetActorLocation(l + this->transform, true);
		//Set sweep to true, this will keep points from moving into terrain in weird cases
	}

	//Moving them doesn't seem to be actually updating the actor's location
	//Spawn, move, findconfiguration goes to their original location?

	//So yeah, not sure how its changing the target positions but not changing 
	//what getTargetPointArray returns

	//oh! Its because we push back values to target point array
	//as we add to the pair system.

	// I guess I never thought I would have to mutate the target points after setting them
	// so this oversight makes sense

	//I just need to make getTargetPointArray() do a O(n) read of all of its actors
	//and not add to it as targetpoint actors are added to the system
}
void AUserCharacter::rotateSelectedMirrors() {
	for (int i = 0; i < selected_actors.Num(); i++) {
		ALMpair * pair = Cast<ALMpair>(selected_actors[i]);
		pair->rotateMirrorToVector(this->transform);
	}
}


void AUserCharacter::mouseMirrorRotationCodeDump() {

	//CreateDefaultSubobject can only be used from constructor

	//We can initialize the FTransform with a scale
	//ex. I could set the added component to be 2x the size of my root component

	//Attempt #1
	/*FTransform init;
	init.SetScale3D(FVector(2, 2, 2));
	UActorComponent* bounding_sphere = highlighted_actor->AddComponentByClass(USphereComponent::StaticClass(), true, FTransform(), false);
	bounding_sphere->RegisterComponent();
	bounding_sphere->InitializeComponent();
	bounding_sphere->SetActive(true);

	FString n = bounding_sphere->GetOwner()->GetFName().ToString();*/


	//Attempt #2
	/*
	USphereComponent* sphere_hitbox = NewObject<USphereComponent>(highlighted_actor, USphereComponent::StaticClass(), TEXT("please work"));

	if (sphere_hitbox) {
		UE_LOG(LogTemp, Warning, TEXT("Component Created"));
		sphere_hitbox->RegisterComponent();
		//TSet<UActorComponent*> comps = highlighted_actor->GetComponentsSet();
		//UE_LOG(LogTemp, Warning, TEXT("Actor has %d components"), comps.Num());
		sphere_hitbox->AttachToComponent(highlighted_actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		sphere_hitbox->CreationMethod = EComponentCreationMethod::Instance; //Actually makes the component visible in world outliner


	}

	FString n = sphere_hitbox->GetOwner()->GetFName().ToString();
	DrawDebugSphere(GetWorld(), sphere_hitbox->GetOwner()->GetActorLocation(), 15, 8, FColor::Red, true, 10000);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *n); //Regardless of the name this prints. Component is being attached to the right owner, just doesn't show up in components.
	*/



	//bounding_sphere->Set
	//bounding_sphere->RegisterComponent();
	//bounding_sphere->SetRelativeScale3D(FVector(2,2,2));
	//bounding_sphere->AttachToActor(highlighted_actor);
	//highlighted_actor->AddInstanceComponent(bounding_sphere); //DOESN'T CRASH!
	// 
	////AddComponent(USphereComponent::StaticClass());
	//	//->ConstructObject<USphereComponent>(USphereComponent::StaticClass(), FName("Timmy"));
	//bounding_sphere->SetSphereRadius(20);


}