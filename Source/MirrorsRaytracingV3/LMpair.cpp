#include "LMpair.h"

// Sets default values
ALMpair::ALMpair()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mirror_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mirror_mesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh_asset(TEXT("StaticMesh'/Game/AddedContent/RealMirror.RealMirror'"));
	//For the Mirror part of this actor, I'm using the static mesh I made previously in the editor with the modeling tools interface.
	// (Its just an upside down cone that has the same measurements as the real life mirrors.

	if (mesh_asset.Succeeded())
	{
		mirror_mesh->SetStaticMesh(mesh_asset.Object);

		// y green, z blue, x red

		// This attaches rotatable arrow components to the mirror in game,
		// These represent a frame of reference for the mirror coordinates
		// they create a transformation matrix: from UE5 absolute coordinates to mirror frame of reference

		basis_z_arrow_component = CreateDefaultSubobject<UArrowComponent>(TEXT("basis_z_arrow"));
		basis_x_arrow_component = CreateDefaultSubobject<UArrowComponent>(TEXT("basis_x_arrow"));
		basis_y_arrow_component = CreateDefaultSubobject<UArrowComponent>(TEXT("basis_y_arrow"));

		bool weld_bodies = false;
		FAttachmentTransformRules r(EAttachmentRule::KeepWorld, weld_bodies);
		FVector scale(0.25, 0.15, 0.15);

		basis_x_arrow_component->AttachToComponent(mirror_mesh, r);
		basis_x_arrow_component->SetRelativeScale3D(scale);
		basis_x_arrow_component->SetRelativeRotation(FVector::LeftVector.Rotation());
		basis_x_arrow_component->SetArrowColor(FColor::Red);


		basis_z_arrow_component->AttachToComponent(mirror_mesh, r);
		basis_z_arrow_component->SetRelativeScale3D(scale);
		basis_z_arrow_component->SetRelativeRotation(FVector::UpVector.Rotation());
		basis_z_arrow_component->SetArrowColor(FColor::Blue);


		basis_y_arrow_component->AttachToComponent(mirror_mesh, r);
		basis_y_arrow_component->SetRelativeScale3D(scale);
		basis_y_arrow_component->SetRelativeRotation((FVector::BackwardVector.Rotation()));
		basis_y_arrow_component->SetArrowColor(FColor::Green);
	}

}

// Called when the game starts or when spawned
void ALMpair::BeginPlay()
{
	Super::BeginPlay();

	calculateTarget();

	// Immediately on play 
	// Create a basis matrix out of the arrow components this mirror is initialized with.
	this->irl_mirror_basis = FBasisVectorMatrix(
		basis_x_arrow_component->GetForwardVector(),
		basis_y_arrow_component->GetForwardVector(),
		basis_z_arrow_component->GetForwardVector(),
		FVector::ZeroVector
		);
}

// Called every frame
void ALMpair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (is_LT_initialized) {
		//Need to make sure this runs after rotate in PairSystem, not before!
		//Since we're drawing them with each tick, we can leave persistent off
		//this way we don't have to manually clear and re-draw debug line traces every time.
		//They will individually dissappear at the end of the tick, then get re-drawn on the next tick.

		//This creates our line trace visualization
		//but its important to note that we call performLineTrace a second time
		//within APairSystem::compare.

		//This is because this tick happens before we rotate the mirror
		//so to get an updated trace, we need to call this method once
		//externally after the tick also.

		performLineTrace(true, false);
	}

}

// [IGNORE] Called to bind functionality to input
void ALMpair::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Calculates the point which the line trace should always target, the center of the flat side of our upside down cone.
void ALMpair::calculateTarget() {
	int cone_height = 3.75; //IRL a mirror is situated 3.75cm from its pivot, this matches the cone model I'm using.
	//To create a mirror cone thats faithful to the real world dimensions, I input h=3.75 x r=5 in unreal units
	//Everything is scaled based off of the RealMirror mesh.

	this->target = GetActorLocation() + (cone_height * GetActorUpVector());
}

// [IGNORE] (fix later?) not using this right now, don't know if I need to.
// We want our line trace to adjust as the mirror rotates
// For each rotation of the mirror, the line trace needs to move such that:
// the new line is parallel to the original, but, still hits the mirror's target
// this means that the LT_source_start needs to be re-adjusted
void ALMpair::recalculateSourceStart() { 
	this->LT_source_start = this->target + (this->LT_length * (this->LT_direction));
	//this->LT_source_start = (this->LT_length * (this->LT_direction));
}

// Initializes a line trace for this LM Pair.
// Without a line trace present, the mirror will still work, it just won't reflect a line trace.
void ALMpair::initializeLineTrace(FVector source_start) {
	this->is_LT_initialized = true;
	this->LT_source_start = source_start;

	calculateTarget();

	FVector line = this->target - source_start;
	
	this->LT_length = line.Size();
	this->LT_direction = line / line.Size();
	//Gives us a unit vector direction,
	//Whenever the start point is re-adjusted, the new line trace must be parallel to this direction
}

// All mirror rotations should be performed through this wrapper function
FRotator ALMpair::rotateMirror(FRotator new_rotation) {
	FRotator prev_rotation = this->GetActorRotation();
	this->SetActorRelativeRotation(new_rotation);

	//Every time we need to rotate the mirror, we need to re-calculate the target and LT_source_start
	calculateTarget();
	//recalculateSourceStart(); [IGNORE]

	// Returns this mirror's previous rotation
	// useful for stochastic algorithms, allows us to easily undo our random change.
	return prev_rotation;
}

void ALMpair::rotateMirrorToVector(FVector new_up_vector) {
	//Only intended to be used in direct optimization
	//GetActorUpVector is what we actually use to calculate center
	rotateMirror(new_up_vector.Rotation() - FRotator(90,0,0)); //Unsure why this FRotator adjustment by 90 fixes it but it works so I'm not going to complain
}

// Randomizes mirror rotation, Stochastic
// Currently not bounded in any way, should bound it.
FRotator ALMpair::randomRotateMirror() {

	double pitch = FMath::FRandRange(0.0, 180.0);
	double yaw = FMath::FRandRange(0.0, 180.0);
	double roll = FMath::FRandRange(0.0, 180.0);

	return rotateMirror(FRotator(pitch, yaw, roll));
}

// Returns all points that the line trace has hit.
// LT_hits is going to have a size of 2.
TArray<FVector> ALMpair::getHits() {
	return this->LT_hits;
}

// Performs a new line trace, with debug parameters to help visualize.
// Clears the LT_hits array and updates it with new results.
// Notes:
// - Currently this method seems to not work 100% of the time, but adjusting the position of the LMpair actor fixes it?
// - With stochastic, reflects off of the back of the mirror, this could probably be solved by adding some kind of disc on top of the cone
//		this disc could have a different collision layer from the cone body. This way, you could set up a multi-line trace, that
//		only records the hits if the trace hits the disc before the cone body.
void ALMpair::performLineTrace(bool debug_lines, bool debug_sphere) {

	this->LT_hits.Empty();

	FCollisionQueryParams TraceParams;
	FHitResult hit1;

	//The vector which represents the "path" our first trace takes
	FVector trace1_vector = this->target - this->LT_source_start;

	bool trace_hit1 = GetWorld()->LineTraceSingleByChannel(hit1, this->LT_source_start, this->target + (trace1_vector*0.1), ECC_WorldStatic, TraceParams);


	if (trace_hit1) {
		FVector hit1_vector = hit1.ImpactPoint;

		//Ternary for debug lines, again, dont need to make these persistent because Ill be calling this function within tick
		debug_lines ? DrawDebugDirectionalArrow(GetWorld(), this->LT_source_start, hit1_vector, 10, FColor::Red) : false;

		//Store the first hit into our array
		this->LT_hits.Add(hit1_vector);

		FVector& hit1_normal = hit1.ImpactNormal;

		//The vector which represents the "path" our next trace should take
		//It bounces off of the surface it made contact with
		float d = 10000; // Some arbitrary distance for the second trace
		FVector trace2_vector = d * trace1_vector.MirrorByVector(hit1_normal);

		FHitResult hit2;
		bool trace_hit2 = GetWorld()->LineTraceSingleByChannel(hit2, hit1_vector, hit1_vector + trace2_vector, ECC_Visibility, TraceParams);

		if (trace_hit2) {
			//Store the second hit into our array
			FVector hit2_vector = hit2.ImpactPoint;
			debug_lines ? DrawDebugDirectionalArrow(GetWorld(), hit1_vector, hit2_vector, 10, FColor::Red) : false;
			this->LT_hits.Add(hit2_vector);
		}
	}
}


void ALMpair::toggleLock() {
	this->locked = !this->locked;
}
bool ALMpair::isLocked() {
	return this->locked;
}
FVector ALMpair::getLTDirection() {
	return this->LT_direction;
}
FVector ALMpair::getTarget() {
	return this->target;
}

// Returns rotation of the mirror from its own frame of reference (basis)
// Used in PairSystem exportConfig().
FRotator ALMpair::getRotationRelativeToZeroVector() {

	FVector pointing_in_R3 = this->GetActorUpVector(); //Where mirror is pointing in UE5 R3 coordinate system.

	FVector pointing_in_irl_mirror_basis = this->irl_mirror_basis.TransformVector(pointing_in_R3); //Where mirror is pointing in mirror R3 coordinate system.

	FRotator r = pointing_in_irl_mirror_basis.Rotation(); //Sets Pitch (about Y axis) and Yaw (about Z axis)

	r -= FRotator(90, 0, 0);
	return r;

}
