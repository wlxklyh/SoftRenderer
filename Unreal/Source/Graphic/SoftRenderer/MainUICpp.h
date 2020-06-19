// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Image.h"
#include "ScreenDevice.h"
#include "MainUICpp.generated.h"

/**
 * 
 */



UCLASS()
class  UMainUICpp : public UUserWidget
{
	GENERATED_BODY()
protected:
	//初始化
	void NativeConstruct() override;

	//Update函数
	UFUNCTION(BlueprintCallable)
	void Update(float InDeltaTime);

private:
	// Image控件 相当于屏幕
	UImage* MainImage;
	// Image每帧绘制的纹理 会从软渲染器ScreenDevice中取出 FrameBuffer 填充到这个MainTexture
	UTexture2D* MainTexture;
	//把HScreenDevice里面的FrameBuffer转成MainColors先再转Texture
	FColor* MainColors ;

};
