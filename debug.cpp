#include "debug.h"
#include <utility>

DebugCuboid::DebugCuboid(irr::scene::ISceneManager *smgr, irr::core::vector3df top, irr::core::vector3df bot) {
    _mesh = new irr::scene::SMesh();
    _meshBuffer = new irr::scene::SMeshBuffer();

    irr::video::SColor color = irr::video::SColor(255,255,255,255);

    _meshBuffer->Vertices.reallocate(24);
    _meshBuffer->Vertices.set_used(24);

    _meshBuffer->Indices.reallocate(36);
    _meshBuffer->Indices.set_used(36);

    _meshBuffer->Vertices[0] = irr::video::S3DVertex(top.X,bot.Y,top.Z, 0,1,0, color, 0, 0);
    _meshBuffer->Vertices[1] = irr::video::S3DVertex(top.X,bot.Y,bot.Z, 0,1,0, color, 0, 0);
    _meshBuffer->Vertices[2] = irr::video::S3DVertex(bot.X,bot.Y,bot.Z, 0,1,0, color, 0, 0);
    _meshBuffer->Vertices[3] = irr::video::S3DVertex(bot.X,bot.Y,top.Z, 0,1,0, color, 0, 0);

    // Down
    _meshBuffer->Vertices[4] = irr::video::S3DVertex(top.X,top.Y,top.Z, 0,-1,0, color, 0, 0);
    _meshBuffer->Vertices[5] = irr::video::S3DVertex(bot.X,top.Y,top.Z, 0,-1,0, color, 0, 0);
    _meshBuffer->Vertices[6] = irr::video::S3DVertex(bot.X,top.Y,bot.Z, 0,-1,0, color, 0, 0);
    _meshBuffer->Vertices[7] = irr::video::S3DVertex(top.X,top.Y,bot.Z, 0,-1,0, color, 0, 0);

    // Right
    _meshBuffer->Vertices[8] = irr::video::S3DVertex(bot.X,top.Y,top.Z, 1,0,0, color, 0, 0);
    _meshBuffer->Vertices[9] = irr::video::S3DVertex(bot.X,bot.Y,top.Z, 1,0,0, color, 0, 0);
    _meshBuffer->Vertices[10] = irr::video::S3DVertex(bot.X,bot.Y,bot.Z, 1,0,0, color, 0, 0);
    _meshBuffer->Vertices[11] = irr::video::S3DVertex(bot.X,top.Y,bot.Z, 1,0,0, color, 0, 0);

    // Left
    _meshBuffer->Vertices[12] = irr::video::S3DVertex(top.X,top.Y,top.Z, -1,0,0, color, 0, 0);
    _meshBuffer->Vertices[13] = irr::video::S3DVertex(top.X,top.Y,bot.Z, -1,0,0, color, 0, 0);
    _meshBuffer->Vertices[14] = irr::video::S3DVertex(top.X,bot.Y,bot.Z, -1,0,0, color, 0, 0);
    _meshBuffer->Vertices[15] = irr::video::S3DVertex(top.X,bot.Y,top.Z, -1,0,0, color, 0, 0);

    // Back
    _meshBuffer->Vertices[16] = irr::video::S3DVertex(top.X,top.Y,bot.Z, 0,0,1, color, 0, 0);
    _meshBuffer->Vertices[17] = irr::video::S3DVertex(bot.X,top.Y,bot.Z, 0,0,1, color, 0, 0);
    _meshBuffer->Vertices[18] = irr::video::S3DVertex(bot.X,bot.Y,bot.Z, 0,0,1, color, 0, 0);
    _meshBuffer->Vertices[19] = irr::video::S3DVertex(top.X,bot.Y,bot.Z, 0,0,1, color, 0, 0);
    
    // Front
    _meshBuffer->Vertices[20] = irr::video::S3DVertex(top.X,top.Y,top.Z, 0,0,-1, color, 0, 0);
    _meshBuffer->Vertices[21] = irr::video::S3DVertex(top.X,bot.Y,top.Z, 0,0,-1, color, 0, 0);
    _meshBuffer->Vertices[22] = irr::video::S3DVertex(bot.X,bot.Y,top.Z, 0,0,-1, color, 0, 0);
    _meshBuffer->Vertices[23] = irr::video::S3DVertex(bot.X,top.Y,top.Z, 0,0,-1, color, 0, 0);

    for (int side = 0; side < 6; side++) {
            _meshBuffer->Indices[side * 6 + 0] = side * 4 + 0;
            _meshBuffer->Indices[side * 6 + 1] = side * 4 + 1;
            _meshBuffer->Indices[side * 6 + 2] = side * 4 + 2;
            _meshBuffer->Indices[side * 6 + 3] = side * 4 + 2;
            _meshBuffer->Indices[side * 6 + 4] = side * 4 + 3;
            _meshBuffer->Indices[side * 6 + 5] = side * 4 + 0;
    }              
    _mesh->addMeshBuffer(_meshBuffer);
    _mesh->recalculateBoundingBox();                                          


    smgr->getMeshManipulator()->setVertexColorAlpha(_mesh, 64);

    _wireNode = smgr->addMeshSceneNode(_mesh);
    _wireNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    _wireNode->setMaterialFlag(irr::video::EMF_WIREFRAME, true);  
    _wireNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);  
    _wireNode->setAutomaticCulling(irr::scene::EAC_FRUSTUM_SPHERE);

    _plainNode = smgr->addMeshSceneNode(_mesh);  
    _plainNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    _plainNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);   
    _plainNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);  
    _plainNode->setAutomaticCulling(irr::scene::EAC_FRUSTUM_SPHERE);
     
}