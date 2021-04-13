// Fill out your copyright notice in the Description page of Project Settings.

#include "Draw3DAgentComponent.h"
#include "Engine/World.h"



UDraw3DAgentComponent::UDraw3DAgentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bHiddenInGame = false;
	bCastDynamicShadow = false;

	bDrawOnlyIfSelected = false;
}

FPrimitiveSceneProxy* UDraw3DAgentComponent::CreateSceneProxy()
{
	/** Represents 'How to draw primitives' to the scene manager. */
	class FDraw3DAgentSceneProxy : public FPrimitiveSceneProxy
	{
	public:
		FDraw3DAgentSceneProxy(UDraw3DAgentComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, InComponent(InComponent)
		{
			bWillEverBeLit = false;
		}

		// FPrimitiveSceneProxy interface.

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_Draw3DAgentSceneProxy_GetDynamicMeshElements);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FMatrix& LocalToWorld = GetLocalToWorld();

					// Taking into account the min and maximum drawing distance
					const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalToWorld.GetOrigin()).SizeSquared();
					if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
					{
						continue;
					}

					InComponent->CreateDynMeshElemDelegate.ExecuteIfBound(View, PDI, LocalToWorld);

					InComponent->CreateDynamicElem(PDI);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleForSelection = !InComponent->bDrawOnlyIfSelected || IsSelected();
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bVisibleForSelection) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

		virtual SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		UDraw3DAgentComponent* InComponent;
	};

	return new FDraw3DAgentSceneProxy(this);
}

void UDraw3DAgentComponent::Refresh()
{
	MarkRenderStateDirty();
}

void UDraw3DAgentComponent::CreateDynamicElem_Implementation(FPrimitiveDrawProxy PDP)
{
	// Do nothing.
}

void UDraw3DAgentComponent::GetCameraAxis(const FSceneView* View, FVector& CameraX, FVector& CameraY)
{
	const FMatrix& Transform = View->ViewMatrices.GetViewProjectionMatrix();
	const FMatrix InvTransform = Transform.Inverse();
	CameraX = InvTransform.TransformVector(FVector::ForwardVector).GetSafeNormal();
	CameraY = InvTransform.TransformVector(FVector::RightVector).GetSafeNormal();
}

// ===================================
void UDraw3DAgentComponent::DrawLine(
	FPrimitiveDrawProxy PDP,
	const FVector& Start, const FVector& End, const FLinearColor& Color,
	ESceneDepthPriorityGroupProxy DepthPriorityGroup, float Thickness, float DepthBias, bool bScreenSpace)
{
	PDP.PDI->DrawLine(Start, End, Color, (uint8)DepthPriorityGroup, Thickness, DepthBias, bScreenSpace);
}

void UDraw3DAgentComponent::DrawPoint(
	FPrimitiveDrawProxy PDP,
	const FVector& Position, const FLinearColor& Color, float PointSize,
	ESceneDepthPriorityGroupProxy DepthPriorityGroup)
{
	PDP.PDI->DrawPoint(Position, Color, PointSize, (uint8)DepthPriorityGroup);
}

void UDraw3DAgentComponent::DrawCircle(
	FPrimitiveDrawProxy PDP,
	const FVector& Base, const FVector& X, const FVector& Y,
	const FLinearColor& Color, float Radius, int32 NumSides,
	ESceneDepthPriorityGroupProxy DepthPriority,
	float Thickness, float DepthBias, bool bScreenSpace)
{
	::DrawCircle(PDP.PDI, Base, X, Y, Color, Radius, NumSides, (uint8)DepthPriority, Thickness, DepthBias, bScreenSpace);
}

// Other Draw- function wrappers..
