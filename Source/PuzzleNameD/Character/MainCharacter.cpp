// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "PuzzleNameD/Interfaces/InteractableInterface.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PuzzleNameD/Objects/ObjectBase.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->bUsePawnControlRotation = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainContext, 0);
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Camera == nullptr || PhysicsHandle == nullptr) return;

	if (PhysicsHandle->GrabbedComponent)
	{
		FVector Start = Camera->GetComponentLocation();
		FVector End = Start + Camera->GetComponentRotation().Vector() * Length;
		PhysicsHandle->SetTargetLocation(End);
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryGrab);
		EnhancedInputComponent->BindAction(DuplicateAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryDuplicate);
		EnhancedInputComponent->BindAction(DeleteAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryDelete);
		EnhancedInputComponent->BindAction(ClearAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryClear);
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AMainCharacter::TryGrab()
{
	if (PhysicsHandle->GrabbedComponent)
	{
		Release();
	}
	else
	{
		Grab();
	}
}

void AMainCharacter::Grab()
{
	if (Camera == nullptr || PhysicsHandle == nullptr) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AActor* Interactable = HitResult.GetActor();
	UPrimitiveComponent* Target = HitResult.GetComponent();
	if (Interactable && Interactable->Implements<UInteractableInterface>() && Target)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(Target, NAME_None, Target->GetComponentLocation(), FRotator::ZeroRotator);
		Length = (Target->GetComponentLocation() - Camera->GetComponentLocation()).Size();
	}
}

void AMainCharacter::Grab(UPrimitiveComponent* Target)
{
	if (PhysicsHandle == nullptr || Target == nullptr) return;

	PhysicsHandle->GrabComponentAtLocationWithRotation(Target, NAME_None, Target->GetComponentLocation(), FRotator::ZeroRotator);
}

void AMainCharacter::Release()
{
	if (PhysicsHandle == nullptr) return;

	UPrimitiveComponent* Component = PhysicsHandle->GrabbedComponent;
	if (Component && Component->GetOwner())
	{
		Component->GetOwner()->SetActorEnableCollision(true);
		IInteractableInterface::Execute_OnPlace(Component->GetOwner());
	}
	PhysicsHandle->ReleaseComponent();
}

void AMainCharacter::TryDuplicate()
{
	AObjectBase* FirstObject = nullptr;
	AObjectBase* SecondObject = nullptr;
	Duplicate(FirstObject, SecondObject);
}

float AMainCharacter::Duplicate(AObjectBase*& OriginalObject, AObjectBase*& DuplicatedObject)
{
	if (PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent) return 0.0f;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	float Mass = 0.0f;
	AActor* Interactable = HitResult.GetActor();
	if (Interactable && Interactable->Implements<UInteractableInterface>())
	{
		Length = DuplicateLength;
		FActorSpawnParameters SpawnParams;
		if (Interactable->GetOwner())
		{
			SpawnParams.Owner = Interactable->GetOwner();
		}
		else
		{
			SpawnParams.Owner = Interactable;
		}
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Interactable->GetClass(), Start + Camera->GetComponentRotation().Vector() * DuplicateLength, Interactable->GetActorRotation(), SpawnParams);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorEnableCollision(false);
			IInteractableInterface::Execute_OnPreview(SpawnedActor);
			UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(SpawnedActor->GetRootComponent());
			Grab(TargetComponent);
			AObjectBase* InteractableObject = Cast<AObjectBase>(Interactable);
			AObjectBase* SpawnedObject = Cast<AObjectBase>(SpawnedActor);
			if (InteractableObject && SpawnedObject)
			{
				OriginalObject = InteractableObject;
				DuplicatedObject = SpawnedObject;
				Mass = InteractableObject->GetMass();
			}
		}
	}

	return Mass;
}

void AMainCharacter::TryDelete()
{
	Delete();
}

float AMainCharacter::Delete()
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	float Mass = 0.0f;
	AActor* Interactable = HitResult.GetActor();
	if (Interactable && Interactable->Implements<UInteractableInterface>() && Interactable->GetOwner())
	{
		AObjectBase* Object = Cast<AObjectBase>(Interactable);
		if (Object)
		{
			Mass = Object->GetMass();
			IInteractableInterface::Execute_OnDisappear(Object);
		}
	}

	return Mass;
}

void AMainCharacter::TryClear()
{
	Clear();
}

float AMainCharacter::Clear()
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	float TotalMass = 0.0f;
	AActor* Interactable = HitResult.GetActor();
	if (Interactable && Interactable->Implements<UInteractableInterface>())
	{
		TArray<AActor*> TargetActors;
		UGameplayStatics::GetAllActorsOfClass(this, Interactable->GetClass(), TargetActors);
		for (AActor* TargetActor : TargetActors)
		{
			if (TargetActor && TargetActor->GetOwner())
			{
				AObjectBase* TargetObject = Cast<AObjectBase>(TargetActor);
				if (TargetObject)
				{
					TotalMass += TargetObject->GetMass();
					IInteractableInterface::Execute_OnDisappear(TargetObject);
				}
			}
		}
	}

	return TotalMass;
}
