//$ Copyright 2015-24, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Utils/PrefabricatorFunctionLibrary.h"

#include "Asset/PrefabricatorAsset.h"
#include "Prefab/PrefabActor.h"
#include "Prefab/PrefabComponent.h"
#include "Prefab/PrefabTools.h"

#include "Engine/Engine.h"

APrefabActor* UPrefabricatorBlueprintLibrary::SpawnPrefab(const UObject* WorldContextObject, UPrefabricatorAssetInterface* Prefab, const FTransform& Transform, int32 Seed)
{
	APrefabActor* PrefabActor = nullptr;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World) {
		if (Prefab->bReplicates) {
			FActorSpawnParameters SpawnParams;
			PrefabActor = World->SpawnActor<AReplicablePrefabActor>(AReplicablePrefabActor::StaticClass(), Transform);
		}
		else {
			PrefabActor = World->SpawnActor<APrefabActor>(APrefabActor::StaticClass(), Transform);
		}
		

		if (PrefabActor) {
			PrefabActor->PrefabComponent->PrefabAssetInterface = Prefab;

			FRandomStream Random(Seed);
			RandomizePrefab(PrefabActor, Random);
		}
	}
	return PrefabActor;
}

void UPrefabricatorBlueprintLibrary::RandomizePrefab(APrefabActor* PrefabActor, const FRandomStream& InRandom)
{
	PrefabActor->RandomizeSeed(InRandom);

	FPrefabLoadSettings LoadSettings;
	LoadSettings.bRandomizeNestedSeed = true;
	LoadSettings.Random = &InRandom;
	FPrefabTools::LoadStateFromPrefabAsset(PrefabActor, LoadSettings);
}

void UPrefabricatorBlueprintLibrary::UnlinkPrefab(APrefabActor* PrefabActor) {
	FPrefabTools::UnlinkAndDestroyPrefabActor(PrefabActor);
}

void UPrefabricatorBlueprintLibrary::GetAllAttachedActors(AActor* Prefab, TArray<AActor*>& AttachedActors)
{
	if (!Prefab) return;

	TArray<AActor*> ChildActors;
	Prefab->GetAttachedActors(ChildActors);
	for (AActor* ChildActor : ChildActors) {
		AttachedActors.Add(ChildActor);
		GetAllAttachedActors(ChildActor, AttachedActors);
	}
}

void UPrefabricatorBlueprintLibrary::SetPrefabAsset(APrefabActor* PrefabActor, UPrefabricatorAssetInterface* Prefab, bool bReloadPrefab)
{
	if (PrefabActor && PrefabActor->PrefabComponent) {
		PrefabActor->PrefabComponent->PrefabAssetInterface = Prefab;

		if (bReloadPrefab) {
			PrefabActor->LoadPrefab();
		}
	}
}

APrefabActor* UPrefabricatorBlueprintLibrary::FindTopMostPrefabActor(AActor* InActor) {
	AActor* CurrentActor = InActor;
	APrefabActor* TopmostPrefab = nullptr;
	while (APrefabActor* PrefabActor = Cast<APrefabActor>(CurrentActor->GetAttachParentActor())) {
		TopmostPrefab = PrefabActor;
		CurrentActor = PrefabActor;
	}
	return TopmostPrefab;
}

