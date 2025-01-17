// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h

#include "CRenderer.h"


irr::video::CRenderer::CRenderer(irr::IrrlichtDevice* device, const irr::c8* shaderDir)
{
    Device= device;
    LightMgr= 0;

    ShaderLib= new irr::video::CShaderLibrary(shaderDir, Device);
    Materials= new irr::video::SMaterials;

    loadShaders();
    createDefaultPipeline();
    MaterialSwapper= new irr::video::CMaterialSwapper(Device->getSceneManager(), Materials);

    Device->run();
}

// TODO (entity#1#): improve
irr::video::CRenderer::~CRenderer()
{
    delete Materials;
    delete ShaderLib;
    delete LightMgr;

    for(irr::u32 i= 0; i < MRTs.size(); i++)
    {
        Device->getVideoDriver()->removeTexture(MRTs[i]);
    }

    Device->getSceneManager()->setLightManager(0);
    Device= 0;
}

void irr::video::CRenderer::createDefaultPipeline()
{
    clearMRTs();

    createMRT("deferred-mrt-color-dont-use-this-name-thanks", irr::video::ECF_A8R8G8B8);
    createMRT("deferred-mrt-normal-dont-use-this-name-thanks", irr::video::ECF_A8R8G8B8);
    createMRT("deferred-mrt-depth-dont-use-this-name-thanks", irr::video::ECF_G16R16F);

    if(LightMgr) LightMgr->drop();
    LightMgr= new irr::scene::ILightManagerCustom(Device, Materials);
    LightMgr->setMRTs(MRTs);
    Device->getSceneManager()->setLightManager(LightMgr);


    Materials->Solid= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("solid"), new irr::video::IShaderDefaultCallback);
    Materials->TransparentRef= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("transparent_alpha_ref"), new irr::video::IShaderDefaultCallback);
    Materials->Transparent= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("transparent_alpha"),
                                                                        new irr::video::IShaderDefaultCallback,
                                                                        irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    Materials->TransparentSoft= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("transparent_alpha_soft"),
                                                                            new irr::video::IShaderDefaultCallback,
                                                                            irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    Materials->Normal= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("normal"), new irr::video::IShaderDefaultCallback);
    Materials->NormalAnimated= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("normalAnimated"), new irr::video::IShaderDefaultCallback);
    Materials->Parallax= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("parallax"), new irr::video::IShaderDefaultCallback);
    Materials->DetailMap= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("detail"), new irr::video::IShaderDefaultCallback);


    //set up point lights
    irr::video::IShaderPointLightCallback* pointCallback= new irr::video::IShaderPointLightCallback(Device->getSceneManager());
    Materials->LightPoint= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("light_point"), pointCallback, irr::video::EMT_TRANSPARENT_ADD_COLOR);
    LightMgr->setLightPointMaterialType(Materials->LightPoint);
    LightMgr->setLightPointCallback(pointCallback);

    //set up spot lights
    irr::video::IShaderSpotLightCallback* spotCallback= new irr::video::IShaderSpotLightCallback(Device->getSceneManager());
    Materials->LightSpot= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("light_spot"), spotCallback, irr::video::EMT_TRANSPARENT_ADD_COLOR);
    LightMgr->setLightSpotMaterialType(Materials->LightSpot);
    LightMgr->setLightSpotCallback(spotCallback);

    //set up directional lights
    irr::video::IShaderDirectionalLightCallback* directionalCallback= new irr::video::IShaderDirectionalLightCallback(Device->getSceneManager());
    Materials->LightDirectional= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("light_directional"), directionalCallback, irr::video::EMT_TRANSPARENT_ADD_COLOR);
    LightMgr->setLightDirectionalMaterialType(Materials->LightDirectional);
    LightMgr->setLightDirectionalCallback(directionalCallback);

    //set up ambient light
    irr::video::IShaderAmbientLightCallback* ambientCallback= new irr::video::IShaderAmbientLightCallback(Device->getSceneManager());
    Materials->LightAmbient= (irr::video::E_MATERIAL_TYPE)createMaterial(ShaderLib->getShader("light_ambient"), ambientCallback, irr::video::EMT_TRANSPARENT_ADD_COLOR);
    LightMgr->setLightAmbientMaterialType(Materials->LightAmbient);
    LightMgr->setLightAmbientCallback(ambientCallback);
}

void irr::video::CRenderer::clearMRTs()
{
    for(irr::u32 i= 0; i < MRTs.size(); i++)
    {
        Device->getVideoDriver()->removeTexture(MRTs[i]);
    }
    MRTs.clear();
}

void irr::video::CRenderer::createMRT(const irr::c8* name, irr::video::ECOLOR_FORMAT format, irr::core::dimension2du dimension)
{
    if(MRTs.size() <= 4)
    {
        if(dimension.Height == 0 || dimension.Width == 0) dimension= Device->getVideoDriver()->getCurrentRenderTargetSize();
        MRTs.push_back(Device->getVideoDriver()->addRenderTargetTexture(dimension, name, format));
    }
}

irr::video::ITexture* irr::video::CRenderer::getMRT(irr::u32 index)
{
    return MRTs[index];
}

irr::u32 irr::video::CRenderer::getMRTCount() const
{
    return MRTs.size();
}

void irr::video::CRenderer::setDoFinalRenderToTexture(bool shouldI)
{
    if(shouldI && !LightMgr->getDoFinalRenderToTexture())
    {
        irr::core::dimension2du dimension= Device->getVideoDriver()->getCurrentRenderTargetSize();
        LightMgr->setRenderTexture(Device->getVideoDriver()->addRenderTargetTexture(dimension, "Final-Render-Tex"));
        LightMgr->setDoFinalRenderIntoTexture(true);
    }
    else if(LightMgr->getDoFinalRenderToTexture())
    {
        LightMgr->setDoFinalRenderIntoTexture(false);
        Device->getVideoDriver()->removeTexture(LightMgr->getRenderTexture());
    }
}

irr::video::ITexture* irr::video::CRenderer::getFinalRenderTexture() const
{
    if(LightMgr->getDoFinalRenderToTexture()) return LightMgr->getRenderTexture();
    else return 0;
}

irr::s32 irr::video::CRenderer::createMaterial(irr::video::SShaderSource shader, irr::video::IShaderConstantSetCallBack *callback, irr::video::E_MATERIAL_TYPE baseType)
{
    Device->getLogger()->log("compile shader", shader.Name.c_str(), irr::ELL_INFORMATION);
    return Device->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
               shader.SourceVertex.c_str(), "main", irr::video::EVST_VS_2_0,
               shader.SourcePixel.c_str(), "main", irr::video::EPST_PS_2_0,
               callback, baseType);
}


irr::video::CShaderLibrary* irr::video::CRenderer::getShaderLibrary() const
{
    return ShaderLib;
}


irr::video::SMaterials* irr::video::CRenderer::getMaterials() const
{
    return Materials;
}

irr::video::CMaterialSwapper* irr::video::CRenderer::getMaterialSwapper() const
{
    return MaterialSwapper;
}

irr::IrrlichtDevice* irr::video::CRenderer::getDevice() const
{
    return Device;
}

void irr::video::CRenderer::loadShaders()
{
    ShaderLib->loadShader("light_point", "light.vert", "light_point.frag");
    ShaderLib->loadShader("light_spot", "light.vert", "light_spot.frag");
    ShaderLib->loadShader("light_directional", "quad.vert", "light_directional.frag");
    ShaderLib->loadShader("light_ambient", "quad.vert", "light_ambient.frag");

    ShaderLib->loadShader("solid", "solid.vert", "solid.frag");
    ShaderLib->loadShader("transparent_alpha_ref", "transparent_alpha_ref.vert", "transparent_alpha_ref.frag");
    ShaderLib->loadShader("transparent_alpha", "transparent_alpha.vert", "transparent_alpha.frag");
    ShaderLib->loadShader("transparent_alpha_soft", "transparent_alpha_soft.vert", "transparent_alpha_soft.frag");
    ShaderLib->loadShader("normal", "normalmap.vert", "normalmap.frag");
    ShaderLib->loadShader("normalAnimated", "normalmap_animated.vert", "normalmap_animated.frag");
    ShaderLib->loadShader("parallax", "parallaxmap.vert", "parallaxmap.frag");
    ShaderLib->loadShader("detail", "terrain.vert", "terrain.frag");
}
