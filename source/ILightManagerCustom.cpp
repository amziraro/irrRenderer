// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h

#include "ILightManagerCustom.h"

using namespace irr;


scene::ILightManagerCustom::ILightManagerCustom(IrrlichtDevice* device, video::SMaterials* mats)
    :Device(device),
    Materials(mats),
    FinalRender(0)
{
    //set up light mesh - sphere
    LightSphere= Device->getSceneManager()->addSphereSceneNode(1.0, 12);
    LightSphere->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    LightSphere->setMaterialFlag(video::EMF_FRONT_FACE_CULLING, true);
    LightSphere->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
    LightSphere->setMaterialFlag(video::EMF_ZBUFFER, false);
	LightSphere->setVisible(false);
	
    //set up light mesh - quad
    LightQuad= new scene::IQuadSceneNode(0, Device->getSceneManager());
	LightQuad->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
    LightQuad->setMaterialFlag(video::EMF_ZBUFFER, false);
	LightQuad->setVisible(false);
}

scene::ILightManagerCustom::~ILightManagerCustom()
{

}

void scene::ILightManagerCustom::OnPreRender(core::array<ISceneNode*> &lightList)
{
	// From Irrlicht svn r5075 (which is about contemporary with this version of irrRenderer)
	/* setRenderTarget 	( 	IRenderTarget *  	target,
	 *						const core::array< u32 > &  	activeTextureID,
	 *						bool  	clearBackBuffer,
	 *						bool  	clearDepthBuffer,
	 *						bool  	clearStencilBuffer,
	 *						SColor  	clearColor = video::SColor(255, 0, 0, 0) 
	 * 					)
	 */
    
    //Compare svn r6355 (ie: current svn)
    /* setRenderTargetEx 	( 	IRenderTarget *  	target,
	 *							u16  	clearFlag,
	 *							SColor  	clearColor = SColor(255, 0, 0, 0),
	 *							f32  	clearDepth = 1.f,
	 *							u8  	clearStencil = 0 
	 * 						)
     */
    
    //Device->getVideoDriver()->setRenderTarget(RenderTarget, RenderIndices, false, true, false, video::SColor(0, 0, 0, 0));
    Device->getVideoDriver()->setRenderTargetEx(RenderTarget, (video::ECBF_DEPTH), video::SColor(0, 0, 0, 0)); 
}

void scene::ILightManagerCustom::OnPostRender()
{

}

void scene::ILightManagerCustom::OnRenderPassPreRender(scene::E_SCENE_NODE_RENDER_PASS renderPass)
{

}

void scene::ILightManagerCustom::OnRenderPassPostRender(scene::E_SCENE_NODE_RENDER_PASS renderPass)
{
    if (renderPass == scene::ESNRP_SOLID)
    {
		//Device->getVideoDriver()->setRenderTarget(0, 0, false, false, false, 0);
        Device->getVideoDriver()->setRenderTargetEx(0, (video::ECBF_NONE), 0);
        deferred();
        //Device->getVideoDriver()->setRenderTarget(RenderTarget, RenderIndices, false, false, false, 0);
        Device->getVideoDriver()->setRenderTargetEx(RenderTarget, (video::ECBF_NONE), 0);
    }
    else if (renderPass == scene::ESNRP_TRANSPARENT)
    {
        //Device->getVideoDriver()->setRenderTarget(0, 0, false, false, false, 0);
        Device->getVideoDriver()->setRenderTargetEx(0, (video::ECBF_NONE), 0);
        deferred();
    }
}

void scene::ILightManagerCustom::OnNodePreRender(scene::ISceneNode *node)
{

}

void scene::ILightManagerCustom::OnNodePostRender(scene::ISceneNode *node)
{

}

inline void scene::ILightManagerCustom::deferred()
{
    //render ambient - also renders nodes with no lighting    
    LightQuad->setMaterialType(LightAmbientMaterial);
    LightQuad->render();

    //render dynamic lights
    for(u32 i = 0; i < Device->getVideoDriver()->getDynamicLightCount(); i++)
    {
        video::SLight light= Device->getVideoDriver()->getDynamicLight(i);

        if (light.Type == video::ELT_POINT)
        {
            LightPointCallback->updateConstants(light);
            LightSphere->setMaterialType(LightPointMaterial);
            LightSphere->setScale(core::vector3df(light.Radius*2.0));
            LightSphere->setPosition(light.Position);
            LightSphere->updateAbsolutePosition();
            LightSphere->render();
        }
        else if (light.Type == video::ELT_SPOT)
        {
            LightSpotCallback->updateConstants(light);
            LightSphere->setMaterialType(LightSpotMaterial);
            LightSphere->setScale(core::vector3df(light.Radius));
            LightSphere->setPosition(light.Position + light.Direction * light.Radius);
            LightSphere->updateAbsolutePosition();
            LightSphere->render();
        }
        else if (light.Type == video::ELT_DIRECTIONAL)
        {
            LightDirectionalCallback->updateConstants(light);
            LightQuad->setMaterialType(LightDirectionalMaterial);
            LightQuad->render();
        }
    }
}


void scene::ILightManagerCustom::setRenderTarget(video::IRenderTarget *RT)
{
    RenderTarget = RT;
    RenderIndices.clear();
    
    for (u32 i = 0; i < RT->getTexture().size(); i++)
    {
        LightSphere->setMaterialTexture(i, RT->getTexture()[i]);
        LightQuad->setMaterialTexture(i, RT->getTexture()[i]);
        RenderIndices.push_back(i);
    }
}

void scene::ILightManagerCustom::setFinalRenderTexture(video::ITexture *tex)
{
    FinalRender = tex;
}

video::ITexture* scene::ILightManagerCustom::getFinalRenderTexture() const
{
    return FinalRender;
}



void scene::ILightManagerCustom::setLightPointMaterialType(video::E_MATERIAL_TYPE &type)
{
    LightPointMaterial= type;
}

void scene::ILightManagerCustom::setLightPointCallback(video::PointCallback* callback)
{
    LightPointCallback= callback;
}

void scene::ILightManagerCustom::setLightSpotMaterialType(video::E_MATERIAL_TYPE &type)
{
    LightSpotMaterial= type;
}

void scene::ILightManagerCustom::setLightSpotCallback(video::SpotCallback* callback)
{
    LightSpotCallback= callback;
}

void scene::ILightManagerCustom::setLightDirectionalMaterialType(video::E_MATERIAL_TYPE &type)
{
    LightDirectionalMaterial= type;
}

void scene::ILightManagerCustom::setLightDirectionalCallback(video::DirectionalCallback* callback)
{
    LightDirectionalCallback= callback;
}

void scene::ILightManagerCustom::setLightAmbientMaterialType(video::E_MATERIAL_TYPE &type)
{
    LightAmbientMaterial= type;
}

void scene::ILightManagerCustom::setLightAmbientCallback(video::AmbientCallback* callback)
{
    LightAmbientCallback= callback;
}
