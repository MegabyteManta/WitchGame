// Copyright Epic Games, Inc. All Rights Reserved.

#include "WitchCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "CustomGameInstance.h"




//////////////////////////////////////////////////////////////////////////
// AWitchCharacter

AWitchCharacter::AWitchCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void AWitchCharacter::BeginPlay() {
	Super::BeginPlay();
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AWitchCharacter::OnHit);
	//OverlapCapsule->OnComponentBeginOverlap.AddDynamic(this, &APlayerSack::OnBeginOverlap);
	StartingMass = GetCharacterMovement()->Mass;
	StartingFriction = GetCharacterMovement()->FallingLateralFriction;
	StartingControl = GetCharacterMovement()->AirControl;
}


void AWitchCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	count++;
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
		UE_LOG(LogTemp, Warning, TEXT("NOT FALLING %d"), count);
	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("This is bullshit"));
	if (is_flying) {
		GetCharacterMovement()->AddForce(FVector(0.f, 0.f, FlyForce*100000.f));
	}

	if (Box)
		RemoveCollectedIfDropped();
	/*
	if (Box)
		Box->SetActorLocation(FVector(GetActorLocation().X-60.f, GetActorLocation().Y, GetActorLocation().Z-120.f));
	
	if (BoxCenter) {
		BoxCenter->GetComponentLocation();

	}
	*/
	//GetCharacterMovement()->SafeMoveUpdatedComponent

}

//Checks if collected objects are dropped, and if so, removes them from sey
void AWitchCharacter::RemoveCollectedIfDropped() {
	TArray<ACollectibleObject*> ToBeRemoved;
	for (ACollectibleObject* CollectibleObject : CollectedObjects) {
		if (!CollectibleObject->Deposited && CollectibleObject->Collected) {
			if (FVector::DistSquared(CollectibleObject->GetActorLocation(), Box->GetActorLocation()) > pow(DropDistance,2)) {
				//Collected.Remove(CollectibleObject);
				ToBeRemoved.Add(CollectibleObject);
				CollectibleObject->Collected = false;
				UCustomGameInstance* CustomGameInstance = Cast<UCustomGameInstance>(GetGameInstance());
				CustomGameInstance->UpdateScore(-1);
				UpdateWeight(-WeightDelta);
			}
		}
	}
	for (ACollectibleObject* CollectibleObject : ToBeRemoved) {
		CollectedObjects.Remove(CollectibleObject);
	}
}

void AWitchCharacter::UpdateWeight(float Delta) {
	Weight += Delta;
	if (Weight == 0) {
		UE_LOG(LogTemp, Warning, TEXT("WEIGHT IS 0"));
	}
	else {
		GetCharacterMovement()->Mass = StartingMass*Weight;
		GetCharacterMovement()->FallingLateralFriction = StartingFriction * 1/Weight;
		GetCharacterMovement()->AirControl = StartingControl * 1 / Weight;
	}
}



//////////////////////////////////////////////////////////////////////////
// Input

void AWitchCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &AWitchCharacter::Fly);
	PlayerInputComponent->BindAction("Fly", IE_Released, this, &AWitchCharacter::StopFly);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWitchCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWitchCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWitchCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWitchCharacter::LookUpAtRate);
}

void AWitchCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWitchCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWitchCharacter::MoveForward(float Value)
{
	Value *= VelocityScale;
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AWitchCharacter::MoveRight(float Value)
{
	Value *= VelocityScale;
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AWitchCharacter::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	//if (OtherActor->ActorHasTag("WaxBody")) {
	count++;
	UE_LOG(LogTemp, Warning, TEXT("Collided with %s %d"), *OtherActor->GetName(), count);
	//PickUp();
	//}
}

void AWitchCharacter::OnBeginOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	count++;
	UE_LOG(LogTemp, Warning, TEXT("Overlapped with %s %d"), *OtherActor->GetName(), count);
	//OtherActor->Destroy();
}

void AWitchCharacter::Fly() {
	//GetCharacterMovement()->AddForce(FVector(0.f,0.f,FlyForce*100000.f));
	is_flying = true;
}

void AWitchCharacter::StopFly() {
	//GetCharacterMovement()->AddForce(FVector(0.f,0.f,FlyForce*100000.f));
	is_flying = false;
}