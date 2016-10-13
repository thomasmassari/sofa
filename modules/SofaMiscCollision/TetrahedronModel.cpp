/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_COMPONENT_COLLISION_TETRAHEDRONMODEL_CPP
#include <SofaMiscCollision/TetrahedronModel.h>
#include <sofa/core/visual/VisualParams.h>
#include <SofaBaseCollision/CubeModel.h>
#include <sofa/simulation/Node.h>
#include <SofaBaseTopology/RegularGridTopology.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/core/ObjectFactory.h>
#include <vector>
#include <iostream>
#include <SofaMeshCollision/BarycentricContactMapper.inl>
#include <sofa/helper/Factory.inl>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(TetrahedronModel)

int TetrahedronModelClass = core::RegisterObject("collision model using a tetrahedral mesh, as described in BaseMeshTopology")
        .add< TetrahedronModel >()
        .addAlias("Tetrahedron")
        ;

TetrahedronModel::TetrahedronModel()
    : tetra(NULL), mstate(NULL)
{
    enum_type = TETRAHEDRON_TYPE;
}

void TetrahedronModel::resize(int size)
{
    this->core::CollisionModel::resize(size);
    elems.resize(size);
    if (getPrevious() != NULL) getPrevious()->resize(0); // force recomputation of bounding tree
}

void TetrahedronModel::init()
{
    _topology = this->getContext()->getMeshTopology();

    this->CollisionModel::init();
    mstate = dynamic_cast< core::behavior::MechanicalState<Vec3Types>* > (getContext()->getMechanicalState());

    if (mstate==NULL)
    {
        serr<<"TetrahedronModel requires a Vec3 Mechanical Model" << sendl;
        return;
    }

    if (!_topology)
    {
        serr<<"TetrahedronModel requires a BaseMeshTopology" << sendl;
        return;
    }

    tetra = &_topology->getTetrahedra();
    resize(tetra->size());

}

void TetrahedronModel::handleTopologyChange()
{
    resize(_topology->getNbTetrahedra());
}

void TetrahedronModel::draw(const core::visual::VisualParams* vparams)
{
    if (mstate && _topology && vparams->displayFlags().getShowCollisionModels())
    {
        vparams->drawTool()->saveLastState();

        if (vparams->displayFlags().getShowWireFrame())
            vparams->drawTool()->setPolygonMode(sofa::core::visual::DrawTool::FACE_FRONT_AND_BACK, true);

        vparams->drawTool()->setLighting(true);
        //Enable<GL_BLEND> blending;
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        defaulttype::Vec4f diffuse(getColor4f()[0], getColor4f()[1], getColor4f()[2], getColor4f()[3]);
        defaulttype::Vec4f emissive(0.0f, 0.0f, 0.0f, 0.0f);
        defaulttype::Vec4f specular(1.0f, 1.0f, 1.0f, 1.0f);

        vparams->drawTool()->setMaterial(diffuse, specular, 20.0, emissive);;

        std::vector<defaulttype::Vector3> positionsTetras;
        std::vector<defaulttype::Vector3> positionsLines;

        for (int i=0; i<size; i++)
        {
            Tetrahedron t(this, i);

            glBegin(GL_TRIANGLES);
            defaulttype::Vector3 p[4];
            p[0] = defaulttype::Vector3(t.p1()[0], t.p1()[1], t.p1()[2]);
            p[1] = defaulttype::Vector3(t.p2()[0], t.p2()[1], t.p2()[2]);
            p[2] = defaulttype::Vector3(t.p3()[0], t.p3()[1], t.p3()[2]);
            p[3] = defaulttype::Vector3(t.p4()[0], t.p4()[1], t.p4()[2]);

            defaulttype::Vector3 c = (p[0] + p[1] + p[2] + p[3])*0.25f;
            for (unsigned int j = 0; j < 4;j++)
            {
                p[j] += (c - p[j])*0.1f;
                positionsTetras.push_back(p[j]);
            }

            defaulttype::Vector3 n[4];
            n[0] = cross(p[2] - p[0], p[1] - p[0]); n[0].normalize();
            n[1] = cross(p[3] - p[0], p[2] - p[0]); n[1].normalize();
            n[2] = cross(p[1] - p[0], p[3] - p[0]); n[2].normalize();
            n[3] = cross(p[2] - p[1], p[3] - p[1]); n[3].normalize();

            if (vparams->displayFlags().getShowNormals())
            {
                defaulttype::Vector3 pn;
                pn = (p[0] + p[1] + p[2])*(1.0 / 3.0);
                positionsLines.push_back(pn);
                positionsLines.push_back(pn + n[0] * 0.1);
                pn = (p[0] + p[2] + p[3])*(1.0 / 3.0);
                positionsLines.push_back(pn);
                positionsLines.push_back(pn + n[1] * 0.1);
                pn = (p[0] + p[3] + p[1])*(1.0 / 3.0);
                positionsLines.push_back(pn);
                positionsLines.push_back(pn + n[2] * 0.1);
                pn = (p[1] + p[2] + p[3])*(1.0 / 3.0);
                positionsLines.push_back(pn);
                positionsLines.push_back(pn + n[3] * 0.1);
            }
        }

        vparams->drawTool()->drawTetrahedra(positionsTetras, defaulttype::Vec4f(diffuse));
        vparams->drawTool()->drawLines(positionsLines, 1.0, defaulttype::Vec4f(diffuse));

        vparams->drawTool()->setLighting(false);
        if (vparams->displayFlags().getShowWireFrame())
            vparams->drawTool()->setPolygonMode(sofa::core::visual::DrawTool::FACE_FRONT_AND_BACK, false);

        vparams->drawTool()->restoreLastState();

    }
    if (getPrevious()!=NULL && vparams->displayFlags().getShowBoundingCollisionModels())
        getPrevious()->draw(vparams);
}

void TetrahedronModel::computeBoundingTree(int maxDepth)
{
    CubeModel* cubeModel = createPrevious<CubeModel>();
    if (!mstate || !_topology) return;
    if (!isMoving() && !cubeModel->empty()) return; // No need to recompute BBox if immobile

    Vector3 minElem, maxElem;
    const VecCoord& x = this->mstate->read(core::ConstVecCoordId::position())->getValue();

    for (int i=0; i<size; i++)
    {
        Tetrahedron t(this,i);
        const Vector3& pt1 = x[t.p1Index()];
        const Vector3& pt2 = x[t.p2Index()];
        const Vector3& pt3 = x[t.p3Index()];
        const Vector3& pt4 = x[t.p4Index()];
        Matrix3 m, minv;
        m[0] = pt2-pt1;
        m[1] = pt3-pt1;
        m[2] = pt4-pt1;
        m.transpose();
        minv.invert(m);
        elems[i].coord0 = pt1;
        elems[i].bary2coord = m;
        elems[i].coord2bary = minv;
    }

    if (maxDepth == 0)
    {
        // no hierarchy
        if (empty())
            cubeModel->resize(0);
        else
        {
            cubeModel->resize(1);
            minElem = x[0];
            maxElem = x[0];
            for (unsigned i=1; i<x.size(); i++)
            {
                const Vector3& pt1 = x[i];
                if (pt1[0] > maxElem[0]) maxElem[0] = pt1[0];
                else if (pt1[0] < minElem[0]) minElem[0] = pt1[0];
                if (pt1[1] > maxElem[1]) maxElem[1] = pt1[1];
                else if (pt1[1] < minElem[1]) minElem[1] = pt1[1];
                if (pt1[2] > maxElem[2]) maxElem[2] = pt1[2];
                else if (pt1[2] < minElem[2]) minElem[2] = pt1[2];
            }
            cubeModel->setLeafCube(0, std::make_pair(this->begin(),this->end()), minElem, maxElem); // define the bounding box of the current Tetrahedron
        }
    }
    else
    {
        cubeModel->resize(size);  // size = number of Tetrahedrons
        if (!empty())
        {
            for (int i=0; i<size; i++)
            {
                Tetrahedron t(this,i);
                const Vector3& pt1 = x[t.p1Index()];
                const Vector3& pt2 = x[t.p2Index()];
                const Vector3& pt3 = x[t.p3Index()];
                const Vector3& pt4 = x[t.p4Index()];
                for (int c = 0; c < 3; c++)
                {
                    minElem[c] = pt1[c];
                    maxElem[c] = pt1[c];
                    if (pt2[c] > maxElem[c]) maxElem[c] = pt2[c];
                    else if (pt2[c] < minElem[c]) minElem[c] = pt2[c];
                    if (pt3[c] > maxElem[c]) maxElem[c] = pt3[c];
                    else if (pt3[c] < minElem[c]) minElem[c] = pt3[c];
                    if (pt4[c] > maxElem[c]) maxElem[c] = pt4[c];
                    else if (pt4[c] < minElem[c]) minElem[c] = pt4[c];
                }
                cubeModel->setParentOf(i, minElem, maxElem); // define the bounding box of the current Tetrahedron
            }
            cubeModel->computeBoundingTree(maxDepth);
        }
    }
}

ContactMapperCreator< ContactMapper<TetrahedronModel> > TetrahedronContactMapperClass("default",true);

template class SOFA_MISC_COLLISION_API ContactMapper<TetrahedronModel, sofa::defaulttype::Vec3Types>;

} // namespace collision

} // namespace component

} // namespace sofa
