// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h
// I've modified this to follow parts of the screenquad example from irrlicht 1.9

#include "IQuadSceneNode.h"

irr::scene::IQuadSceneNode::IQuadSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id) : irr::scene::ISceneNode(parent, mgr)
{
    //remove();

    Buffer = new irr::scene::SMeshBuffer;
    Buffer->Vertices.push_back(irr::video::S3DVertex(
                                   irr::core::vector3df(-1.0, 1.0, 0.0),
                                   irr::core::vector3df(0.0, 0.0, 1.0), irr::video::SColor(255,255,255,255),
                                   irr::core::vector2df(0.0, 0.0)));
    Buffer->Vertices.push_back(irr::video::S3DVertex(
                                   irr::core::vector3df(1.0, 1.0, 0.0),
                                   irr::core::vector3df(0.0, 0.0, 1.0), irr::video::SColor(255,255,255,255),
                                   irr::core::vector2df(1.0, 0.0)));
    Buffer->Vertices.push_back(irr::video::S3DVertex(
                                   irr::core::vector3df(1.0, -1.0, 0.0),
                                   irr::core::vector3df(0.0, 0.0, 1.0), irr::video::SColor(255,255,255,255),
                                   irr::core::vector2df(1.0, 1.0)));
    Buffer->Vertices.push_back(irr::video::S3DVertex(
                                   irr::core::vector3df(-1.0, -1.0, 0.0),
                                   irr::core::vector3df(0.0, 0.0, 1.0), irr::video::SColor(255,255,255,255),
                                   irr::core::vector2df(0.0, 1.0)));

    Buffer->Indices.push_back(0);
    Buffer->Indices.push_back(1);
    Buffer->Indices.push_back(2);
    Buffer->Indices.push_back(3);
    Buffer->Indices.push_back(0);
    Buffer->Indices.push_back(2);
    Buffer->recalculateBoundingBox();
    //setScale(irr::core::vector3df(20,20,20));
    updateAbsolutePosition();
    
    // turn off lighting as default
    Material.setFlag(video::EMF_LIGHTING, false);
    
	// set texture warp settings to clamp to edge pixel
	for (u32 i = 0; i < video::MATERIAL_MAX_TEXTURES; i++)
	{
		Material.TextureLayer[i].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
		Material.TextureLayer[i].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
	}
    
    // for 1.8 (and contemporary svn?)
    //Material.ZBuffer = false;
    //Material.ZWriteEnable = false;
    
    // for 1.9 (current svn)
    Material.ZBuffer = video::ECFN_DISABLED;
    Material.ZWriteEnable = video::EZW_OFF;
}

irr::scene::IQuadSceneNode::~IQuadSceneNode()
{
    delete Buffer;
}

void irr::scene::IQuadSceneNode::render()
{
	irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setMaterial(Material);

	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	// set matrices to fit the quad to full viewport
	//driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
	//driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
	//driver->setTransform(video::ETS_PROJECTION, core::IdentityMatrix);

	driver->drawMeshBuffer(Buffer);
}

void irr::scene::IQuadSceneNode::OnRegisterSceneNode()
{

}

void irr::scene::IQuadSceneNode::setMaterialTexture(irr::u32 layer, irr::video::ITexture* texture)
{
    Material.setTexture(layer, texture);
}

void irr::scene::IQuadSceneNode::setMaterialType(irr::video::E_MATERIAL_TYPE tehType)
{
    Material.MaterialType= tehType;
}

const irr::core::aabbox3d<irr::f32>& irr::scene::IQuadSceneNode::getBoundingBox() const
{
    return Buffer->getBoundingBox();
}

irr::video::SMaterial& irr::scene::IQuadSceneNode::getMaterial(irr::u32 num)
{
    return Material;
}

irr::u32 irr::scene::IQuadSceneNode::getMaterialCount() const
{
    return 1;
}
