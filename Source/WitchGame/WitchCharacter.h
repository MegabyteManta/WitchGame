// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CollectibleObject.h"
#include "WitchCharacter.generated.h"

UCLASS(config = Game)
class AWitchCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying")
		float FlyForce = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible Objects")
		TSet<ACollectibleObject*> CollectedObjects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible Objects")
		TSet<ACollectibleObject*> DepositedObjects;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stacking")
		AActor* Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stacking")
		float DropDistance = 1000;

	UPROPERTY(BlueprintReadWrite, Category = "Weight")
		float Weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
		float WeightDelta = .2f;

	UFUNCTION(BlueprintCallable, Category = "Weight")
		void UpdateWeight(float Delta);

	float StartingMass;
	float StartingFriction;
	float StartingControl;
	void Fly();
	void StopFly();
	void RemoveCollectedIfDropped();

	int count = 0;

	bool is_flying = false;



public:
	AWitchCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	//Overlap event
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//Collision event
	UFUNCTION()
		void OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp,
			FVector NormalImpulse, const FHitResult & Hit);

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
};