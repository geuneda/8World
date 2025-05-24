#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PWGameMode.generated.h"

class AResourceManager;
class UResourceDataManager;

/**
 * 8World 게임 모드 클래스
 */
UCLASS()
class EIGHTWORLDPROJECT_API APWGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	APWGameMode();

	virtual void BeginPlay() override;
	
	// 리소스 매니저 생성
	UFUNCTION(BlueprintCallable, Category = "Game")
	void CreateResourceManager();
	
	// 리소스 데이터 매니저 생성
	UFUNCTION(BlueprintCallable, Category = "Game")
	void CreateResourceDataManager();
	
	// 리소스 매니저 가져오기
	UFUNCTION(BlueprintCallable, Category = "Game")
	AResourceManager* GetResourceManager() const { return ResourceManager; }
	
	// 리소스 데이터 매니저 가져오기
	UFUNCTION(BlueprintCallable, Category = "Game")
	UResourceDataManager* GetResourceDataManager() const { return ResourceDataManager; }
	
private:
	// 리소스 매니저 인스턴스
	UPROPERTY()
	AResourceManager* ResourceManager;
	
	// 리소스 데이터 매니저 인스턴스
	UPROPERTY(EditDefaultsOnly, Category = "Resource Data")
	TSubclassOf<UResourceDataManager> ResourceDataManagerClass;
	
	// 리소스 데이터 매니저 객체
	UPROPERTY()
	UResourceDataManager* ResourceDataManager;
};
