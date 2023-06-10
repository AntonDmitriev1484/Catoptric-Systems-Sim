// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LMPair.h"
#include "AbstractPairSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbstractPairSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MIRRORSRAYTRACINGV3_API IAbstractPairSystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

private:
	// Even though its labeled as an interface, you can essentially treat this as though its an abstract class
	// You can write a virtual function here and add a default implementation into AbstractPairSystem.cpp

	//TArray<ALMpair*> pairs;
	//ATargetImage* target_image;

	//Should just call findConfiguration_n() in the implementing class
	//virtual void findConfiguration(int n);


};
