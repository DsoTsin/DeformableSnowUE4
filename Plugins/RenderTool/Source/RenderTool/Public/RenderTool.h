#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "RHICommandList.h"

class FRenderToolModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

struct FQuadVertex
{
    FVector4 Position;
    FVector2D UV;
};

class FQuadVertexDeclaration : public FRenderResource
{
public:
    FVertexDeclarationRHIRef VertexDeclarationRHI;
    virtual ~FQuadVertexDeclaration() {}
    virtual void InitRHI()
    {
        FVertexDeclarationElementList Elements;
        Elements.Add(FVertexElement(0, STRUCT_OFFSET(FQuadVertex, Position), VET_Float4, 0, sizeof(FQuadVertex)));
        Elements.Add(FVertexElement(0, STRUCT_OFFSET(FQuadVertex, UV), VET_Float2, 1, sizeof(FQuadVertex)));
        VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
    }
    virtual void ReleaseRHI()
    {
        VertexDeclarationRHI.SafeRelease();
    }
};

extern TGlobalResource<FQuadVertexDeclaration> GQuadVertexDeclaration;
