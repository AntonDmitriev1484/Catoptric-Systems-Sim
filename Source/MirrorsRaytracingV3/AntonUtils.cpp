// Fill out your copyright notice in the Description page of Project Settings.


#include "AntonUtils.h"

void UAntonUtils::print(FString s) {
	//No clue on this sweet earth how FString works, honestly I do not care at this point
	//that is why I am making these helpers
	UE_LOG(LogTemp, Warning, TEXT("%s"), *s);
}

void UAntonUtils::print(FString before, FVector v) {
	UE_LOG(LogTemp, Warning, TEXT("%s : %f %f %f"), *before, v.X, v.Y, v.Z);
}

void UAntonUtils::print(FString before, FRotator r) {
	UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *before, *r.ToString());
}

void UAntonUtils::print(FString before, FVector v, FString after) {
	UE_LOG(LogTemp, Warning, TEXT("%s : %f %f %f . %s"), *before, v.X, v.Y, v.Z, *after);
}

void UAntonUtils::found_config_message(int threshold, int iterations) {
	UE_LOG(LogTemp, Warning, TEXT("Configuration found! Threshold of %d reached in %d iterations"), threshold, iterations);
}

void UAntonUtils::found_config_message(int iterations) {
	UE_LOG(LogTemp, Warning, TEXT("Configuration found! In %d iterations"), iterations);
}

bool UAntonUtils::write_string_to_file(FString path, FString string) {
	// Unclear what flags to set to append to the C++ file
	return FFileHelper::SaveStringToFile(string, *path);
}