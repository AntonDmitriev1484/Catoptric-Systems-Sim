// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"


AMyPlayerController::AMyPlayerController() {

}

void AMyPlayerController::BeginPlay() {
	Super::BeginPlay();
	this->mouse_selection = false;
}

void AMyPlayerController :: SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::G, IE_Pressed, this, &AMyPlayerController::toggleMouse);
	//At the end we pass in a function pointer, this is kind of similar to first class functions / lambdas in other languages
}

void AMyPlayerController::Tick(float DeltaTime) {

}

void AMyPlayerController::toggleMouse() {
	this->mouse_selection = !this->mouse_selection;
	UE_LOG(LogTemp, Warning, TEXT("In toggleMouse()"));

	if (mouse_selection) {
		this->bShowMouseCursor = true;
	}
}

void AMyPlayerController::mouseClickMirrorRotation() {

}

//Need to make my own Gamemode that uses this controller? Yucky?
// https://www.reddit.com/r/unrealengine/comments/5tx2fa/playercontroller_not_working/
//Maybe just track down the controller that GameModeBase uses and add one binding to it?