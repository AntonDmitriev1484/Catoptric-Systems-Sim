// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AntonUtils.generated.h"

/**
 * 
 */
UCLASS()
class MIRRORSRAYTRACINGV3_API UAntonUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void print(FString s);
	static void print(FString begin, FVector v);
	static void print(FString begin, FRotator r);
	static void print(FString begin, FVector v, FString end);

	static void found_config_message(int threshold, int iterations);
	static void found_config_message(int iterations);

	static void print_on_screen(FString s);

	static bool write_string_to_file(FString path, FString string);
	
};
