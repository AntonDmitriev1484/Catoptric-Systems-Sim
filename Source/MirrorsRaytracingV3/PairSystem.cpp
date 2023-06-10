// Fill out your copyright notice in the Description page of Project Settings.



#include "PairSystem.h"
#include "UserCharacter.h"

// Sets default values
APairSystem::APairSystem()
{

	PrimaryActorTick.bCanEverTick = true;

	this->OnActorBeginOverlap.AddDynamic(this, &APairSystem::OnOverlapBegin);
	this->OnActorEndOverlap.AddDynamic(this, &APairSystem::OnOverlapEnd);

}

// Called when the game starts or when spawned
void APairSystem::BeginPlay()
{
	Super::BeginPlay();

	this->bind_tag = FString(this->Tags[0].ToString()); // Tag should always be first element present in tags array
	//bindByTag();
	bindByLoc();
	initializeLMpairs();

	this->algo = (GetWorld()->SpawnActor<AFindConfigTracker>(AFindConfigTracker::StaticClass()));
	algo->init();

}

// Called every frame
void APairSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green);

	if (algo->active) {
		if (algo->id == STOCHASTIC) {

			//Terminates upon score reaching threshold
			if (fC1_best_score > fC1_threshold) {
				this->fC1_changed_previous_rotator = rotateRandomMirror();
				int score = compare();

				if (score < this->fC1_best_score) {
					this->fC1_best_score = score;
				}
				else {
					//Extracting what index was selected at random from our tuple
					//Extracting the old (pre-change) rotator from our tuple
					//Using these things to undo the rotation which worsened the score

					this->pairs[this->fC1_changed_previous_rotator.Get<0>()]
						->rotateMirror(this->fC1_changed_previous_rotator.Get<1>());
				}

				this->fC1_iteration++;
			}
			else {
				UAntonUtils::found_config_message(this->fC1_threshold, this->fC1_iteration);
				this->find_configuration_1 = false;
			}

		}
		else if (algo->id == DIRECT) {

			//Terminates upon all mirrors being locked
			if (locked_mirrors < pairs.Num()) {

				TTuple<int, FVector> hit_and_source = closestHitPoint();
				int num = hit_and_source.Get<0>();
				rotateMirrorTowardsTarget(num, hit_and_source.Get<1>());

				//Go to the next target point
				algo->target_point_index++;
				algo->target_point_index %= (target_image->getTargetPointArray().Num());

				//Track the iteration
				algo->iteration++;
			}
			else {

				UAntonUtils::found_config_message(algo->iteration);
				//this->find_configuration_2 = false;
				algo->active = false;

				// Unlock all mirrors, so that all mirrors will 
				// be able to rotate in the next call to find_configuration_2
				for (int i = 0; i < this->locked_mirrors; i++) {
					this->pairs[i]->toggleLock();
				}
				this->locked_mirrors = 0;

			}

		}
	}

}

void APairSystem::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor) {
	if (OtherActor->GetClass()->GetName().Equals(FString("UserCharacter"))) {
		AUserCharacter* user = Cast<AUserCharacter>(OtherActor);
		user->controlled_pair_system = this;
	}
	//Note: Overlap only triggers when you enter and exit the volume
	//if the actor spawns directly within the volume overlap wont activate
}
void APairSystem::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor) {
	if (OverlappedActor->GetClass()->GetName().Equals(FString("UserCharacter"))) {
		AUserCharacter* user = Cast<AUserCharacter>(OverlappedActor);
		user->controlled_pair_system = NULL;
	}
}

// Acquires pointers to all associated actor instances, LMpairs, TargetPoints, TargetImage
// Sets up our pairs TArray
void APairSystem::bindByLoc() {
	TArray<ATargetP*> target_points;

	DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale(), FColor::Purple);
	FBox bounds = this->GetComponentsBoundingBox(true, false);
	
	//Whatever you parametrize the TActorITerator with will be the only data type it returns.
	//Had this as ALMpair earlier and, surprise, it only iterated over ALMpairs
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//Retrieve actor pointer from iterator
		AActor* MyActor = *ActorItr;

		//Get actor UClass name
		FString name = MyActor->GetClass()->GetName();

		//If this AActor has a tag that associates it with our pair system
		//We want to connect it to this pair system instance some how

		FVector loc = MyActor->GetActorLocation();
		if (bounds.IsInside(MyActor->GetActorLocation())) {
			if (name.Equals(FString("LMpair"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound LMpair"));
				//If the actor is an LMpair, we know we can safely cast our type from AActor -> ALMpair
				ALMpair* pair = Cast<ALMpair>(MyActor);
				pairs.Add(pair);
			}
			else if (name.Equals(FString("TargetP"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound target point"));
				//No real need to cast these, whats most important is their coordinate location, not their type
				//Want to keep the pointer to them around just in case
				ATargetP* target_point = Cast<ATargetP>(MyActor);
				target_points.Add(target_point);
			}
			else if (name.Equals(FString("TargetImage"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound target image"));
				//If the actor is a TargetImage, we know we can safely cast our type from AActor -> ATargetImage
				this->target_image = Cast<ATargetImage>(MyActor);
			}
			else if (name.Equals(FString("DirectionalLight"))) {
				//Note: Light source parent class is Light
				//wont be hit by actor iterator
				UE_LOG(LogTemp, Warning, TEXT("Bound light"));
				this->light_direction_actor = MyActor;
			}
		}
	}

	target_image->addTargetPointActors(target_points);
}
void APairSystem::bindByTag() {
	//Name of this PairSystem, the tag we're looking for
	FString s = this->bind_tag; 
	TArray<ATargetP*> target_points;

	//Whatever you parametrize the TActorITerator with will be the only data type it returns.
	//Had this as ALMpair earlier and, surprise, it only iterated over ALMpairs
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//Retrieve actor pointer from iterator
		AActor* MyActor = *ActorItr;
		
		//Get actor UClass name
		FString name = MyActor->GetClass()->GetName();

		//If this AActor has a tag that associates it with our pair system
		//We want to connect it to this pair system instance some how
		if (MyActor->ActorHasTag(FName(*s))) {

			if (name.Equals(FString("LMpair"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound LMpair"));
				//If the actor is an LMpair, we know we can safely cast our type from AActor -> ALMpair
				ALMpair* pair = Cast<ALMpair>(MyActor);
				pairs.Add(pair);
			}
			else if (name.Equals(FString("TargetP"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound target point"));
				//No real need to cast these, whats most important is their coordinate location, not their type
				//Want to keep the pointer to them around just in case
				ATargetP* target_point = Cast<ATargetP>(MyActor);
				target_points.Add(target_point);
			}
			else if (name.Equals(FString("TargetImage"))) {
				UE_LOG(LogTemp, Warning, TEXT("Bound target image"));
				//If the actor is a TargetImage, we know we can safely cast our type from AActor -> ATargetImage
				this->target_image = Cast<ATargetImage>(MyActor);
			}
			else if (name.Equals(FString("DirectionalLight"))){
				//Note: Light source parent class is Light
				//wont be hit by actor iterator
				UE_LOG(LogTemp, Warning, TEXT("Bound light"));
				//ADirectionalLight* dl = Cast<ADirectionalLight>(MyActor);
				this->light_direction_actor = MyActor;
			}
		}
	}

	target_image->addTargetPointActors(target_points);

} 

void APairSystem::testBind() {
	UE_LOG(LogTemp, Warning, TEXT("pairs array size: %d"), pairs.Num());
	for (int i = 0; i < this->pairs.Num(); i++) {
		pairs[i]->rotateMirror(FRotator(0, 0, 0));
	}
}

// Sets up LT_source_location for all mirrors, based on the light source we've bound
void APairSystem::initializeLMpairs() {
	const int abs_distance = 10000; //Hard coded, cringe I know
	//We want the rays to start 10000 away from each mirror, but still be parallel
	//to the vector of our light source.

		FVector light_direction = light_direction_actor->GetActorForwardVector();
		FVector source_start;

		for (int i = 0; i < pairs.Num(); i++) {
			source_start = pairs[i]->GetActorLocation() - abs_distance*light_direction;
			pairs[i]->initializeLineTrace(source_start);
		}


}

// Gets all hit points from involved LMpair instances
TArray<FVector> APairSystem::readHitPoints() {
	TArray<FVector> hit_points;
	TArray<FVector> pair_hits;

	for (int i = 0; i < pairs.Num(); i++) {

		pairs[i]->performLineTrace(true, false);
		//IMPORTANT! This forces a line trace to go through immediatley before we get hits
		// but this will always occur chronologically after we change our rotation.

		pair_hits = pairs[i]->getHits();

		if (pair_hits.Num() > 1) {
			hit_points.Add(pair_hits[1]);

		}
		else {
			//Add a super positive weight when there is no second hit "+INF"
			hit_points.Add(FVector(100000, 1000000, 100000));
		}
	}
	return hit_points;
}
TArray<FVector> APairSystem::readHitPointsUnlocked() {
	TArray<FVector> hit_points = readHitPoints();
	TArray<FVector> filtered_hit_points;

	for (int i = 0; i < hit_points.Num(); i++) {
		if (!pairs[i]->isLocked()) {
			filtered_hit_points.Add(hit_points[i]);
		}
	}

	return filtered_hit_points;
}

// Scores current configuration by summation
// of distances between the closest pairs. "Nearest Neighbor Squared Distance"
int APairSystem::compare() {
	TArray<FVector> hit_points = readHitPoints();
	TArray<FVector> target_points = target_image->getTargetPointArray();

	int sum = 0;
	float dist, min_dist = 0;
	FVector hit_point, target_point;

	
	for (int i = 0; i < target_points.Num(); i++) {
		target_point = target_points[i];
		min_dist = FVector::Dist(target_point, hit_points[0]);

		for (int j = 0; j < hit_points.Num(); j++) {

			hit_point = hit_points[j];
			dist = FVector::Dist(target_point,hit_point);

			if (dist < min_dist) {
				min_dist = dist;
			}
		}

		sum += static_cast<int32>(min_dist);
	}

	if (hit_points.Num() == 0) {
		//No hit points detected, can't compare
	}
	
	drawPoints(hit_points, FColor::Blue);

	return sum;
}

// Rotates a random mirror from our pairs TArray
TTuple<int, FRotator> APairSystem::rotateRandomMirror() {
	float mirror = FMath::FRandRange(0.0, float(pairs.Num()-1)); //Got a random out of bounds here once??

	int mirror_number = static_cast<int32>(mirror);

	FRotator previous_mirror_rotator = this->pairs[mirror_number]->randomRotateMirror();

	return MakeTuple(mirror_number, previous_mirror_rotator);
}

// Default stochastic optimization
void APairSystem::findConfiguration_1(const int threshold) {
	
	int iteration = 0;
	int best_score = compare();
	int score = best_score;
	int changed_mirror_index;
	FRotator previous_mirror_rotator;

	TTuple<int, FRotator> changed_previous_rotator;

	while (best_score > threshold) {

		changed_previous_rotator = rotateRandomMirror();

		//Rotates them all away until its 0?
		score = compare();
		if (score < best_score) {
			best_score = score;
		}
		else {
			//Extracting what index was selected at random from our tuple
			changed_mirror_index = changed_previous_rotator.Get<0>();
			//Extracting the old (pre-change) rotator from our tuple
			previous_mirror_rotator = changed_previous_rotator.Get<1>();
			//Undo the rotation which worsened our score
			this->pairs[changed_mirror_index]->rotateMirror(previous_mirror_rotator);
		}

		iteration++;
	}

	UE_LOG(LogTemp, Warning, TEXT("Configuration found! Threshold of %d reached in %d iterations"), threshold, iteration);
}
void APairSystem::findConfiguration_1_Tick(int threshold) {
	this->find_configuration_1 = true;
	this->fC1_threshold = threshold;
	this->fC1_best_score = 10000000; // initialize this to some super large positive value for now
	this->fC1_iteration = 0;
}

// Default direct optimization
void APairSystem::findConfiguration_2_Tick() {
	this->algo->init(DIRECT);
}

TTuple<int, FVector> APairSystem::closestHitPoint() {

	TArray<FVector> hit_points = readHitPoints();
	//drawPoints(hit_points, FColor::Yellow);
	TArray<FVector> target_points = this->target_image->getTargetPointArray();

	float min_dist = 4294967295;
	
	float dist = 0;
	//Starting min_dist off at some distance within the array

	FVector closest_hit_point(0, 0, 0);
	int mirror_num = 0;
	bool unlocked = true;

	for (int i = 0; i < hit_points.Num(); i++) {
		dist = FVector::Dist(target_points[algo->target_point_index], hit_points[i]); 
		

		if ((dist < min_dist) & !this->pairs[i]->isLocked()) {
			min_dist = dist;
			closest_hit_point = hit_points[i];
			mirror_num = i;
		}
	}

	return MakeTuple(mirror_num, closest_hit_point);
}

// Solves a geometry problem and determines how the source mirror of this hit_point
// should be rotated to aim its light spot at the target
void APairSystem::rotateMirrorTowardsTarget(int mirror_num, FVector hit_point) { //This should be the target point

	//The mirrors that get rotated have their light direction rotated also?
	//Really weird, this doesn't happen for stochastic
	//It does actually!
	//The line traces arent being rotated this way by my code
	//but they get initialized this way when they are given some initila rotation in the preview

	bool debug_lines = false;
	//debug_lines ? DrawDebugDirectionalArrow(GetWorld(), hit1_vector, hit2_vector, 10, FColor::Green) : false;

	//Making a triangle a,b,c
	//FVector a = hit_point;
	FVector a = this->target_image->getTargetPointArray()[algo->target_point_index];
	FVector c = this->pairs[mirror_num]->getTarget();

	//DrawDebugSphere(GetWorld(), a, 5, 4, FColor::Red);
	//UE_LOG(LogTemp, Warning, TEXT("Hit point: %f %f %f"), hit_point.X, hit_point.Y, hit_point.Z);

	FVector hit_to_mirror = c - a;
	debug_lines ? DrawDebugDirectionalArrow(GetWorld(), a, a + hit_to_mirror, 10, FColor::Blue) : false;

	//b is calculated by going dist(hit_to_mirror)
	//up the direction of the light source vector
	FVector b = c + (hit_to_mirror.Size() * (-this->pairs[mirror_num]->getLTDirection()));

	//UE_LOG(LogTemp, Warning, TEXT("b: %f %f %f"),b.X, b.Y, b.Z);
	debug_lines ? DrawDebugDirectionalArrow(GetWorld(), c, b, 10, FColor::Red) : false;

	FVector a_to_b = b - a;
	debug_lines ? DrawDebugDirectionalArrow(GetWorld(), a, a + a_to_b, 10, FColor::Purple) : false;

	FVector midpoint = b - (0.5 * a_to_b);

	FVector pivot_center = this->pairs[mirror_num]->GetActorLocation();
	FVector new_forward_vector = midpoint - pivot_center;
	//debug_lines ? DrawDebugDirectionalArrow(GetWorld(), midpoint, pivot_center, 10, FColor::Yellow) : false;

	new_forward_vector /= new_forward_vector.Size(); // Convert new_target_vector to unit vector
	debug_lines ? DrawDebugDirectionalArrow(GetWorld(), pivot_center,pivot_center+20*new_forward_vector, 10, FColor::Orange) : false;
	
	this->pairs[mirror_num]->rotateMirrorToVector(new_forward_vector);
	this->pairs[mirror_num]->toggleLock();
	this->locked_mirrors++;

	//Lock this mirror, we no longer want to consider its hit point or move it
}

// Exports current mirror config to a text file
void APairSystem::exportConfiguration() {
	FString folder_path = "C:\\Users\\soula\\Documents\\Unreal Projects\\MirrorsRaytracingV3\\Content\\ConfigExportOutput";
	FString file_name = this->GetActorLabel();
	FString extension = "Config.txt";

	FString file_path = folder_path.Append("\\").Append(file_name).Append(extension);

	FString text;
	FString pair_number;
	FRotator r;

	for (int i = 0; i < this->pairs.Num(); i++) {
		r = this->pairs[i]->getRotationRelativeToZeroVector();

		//For a system with only 1D numbering:
		//pair_number = FString::FromInt(this->pairs[i]->user_numbering_1D);

		//For a system with 2D numbering:
		pair_number = FString::FromInt(static_cast<int32>(this->pairs[i]->user_numbering_2D.X))
			.Append(",")
			.Append(FString::FromInt(static_cast<int32>(this->pairs[i]->user_numbering_2D.Y)));

		text.Append(
			FString("(").Append(pair_number).Append(")").Append(" : ").Append(r.ToString()).Append("\n")
		);
	}

	UAntonUtils::write_string_to_file(file_path, text);
}

// Utility: Draws a bunch of points in a specific color
void APairSystem::drawPoints(TArray<FVector> points, FColor color) {
	for (int i = 0; i < points.Num(); i++) {
		DrawDebugSphere(GetWorld(), points[i], 3, 4, color);
	}
}

