#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Buildables.generated.h"

/**
 * 건축물 데이터 구조체
 */
USTRUCT(BlueprintType)
struct EIGHTWORLDPROJECT_API FBuildData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildables")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildables")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildables")
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildables")
	int32 TreeCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildables")
	int32 RockCost;
};
