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
#ifndef SOFA_COMPONENT_ENGINE_SMOOTHMESHENGINE_INL
#define SOFA_COMPONENT_ENGINE_SMOOTHMESHENGINE_INL

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include "SmoothMeshEngine.h"
#include <sofa/helper/gl/template.h>

#include <sofa/core/visual/VisualParams.h>

namespace sofa
{

namespace component
{

namespace engine
{

template <class DataTypes>
SmoothMeshEngine<DataTypes>::SmoothMeshEngine()
    : input_position( initData (&input_position, "input_position", "Input position") )
    , input_indices( initData (&input_indices, "input_indices", "Position indices that need to be smoothed, leave empty for all positions") )
    , output_position( initData (&output_position, "output_position", "Output position") )
    , nb_iterations( initData (&nb_iterations, (unsigned int)1, "nb_iterations", "Number of iterations of laplacian smoothing") )
    , showInput( initData (&showInput, false, "showInput", "showInput") )
    , showOutput( initData (&showOutput, false, "showOutput", "showOutput") )
{

}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::init()
{
    m_topo = this->getContext()->getMeshTopology();
    if (!m_topo)
        serr << "SmoothMeshEngine requires a mesh topology" << sendl;

    addInput(&input_position);
    addOutput(&output_position);

    setDirtyValue();
}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::reinit()
{
    update();
}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::update()
{
    using sofa::core::topology::BaseMeshTopology;

    cleanDirty();

    if (!m_topo) return;

    helper::ReadAccessor< Data<VecCoord> > in(input_position);
    helper::ReadAccessor< Data<helper::vector <unsigned int > > > indices(input_indices);
    helper::WriteAccessor< Data<VecCoord> > out(output_position);

    out.resize(in.size());
    for (unsigned int i =0; i<in.size();i++) out[i] = in[i];
    
    for (unsigned int n=0; n < nb_iterations.getValue(); n++)
    {
        VecCoord t;
        t.resize(out.size());

        if(!indices.size())
        {
            for (unsigned int i = 0; i < out.size(); i++)
            {
                BaseMeshTopology::VerticesAroundVertex v = m_topo->getVerticesAroundVertex(i);
                if (v.size()>0) {
                    Coord p = Coord();
                    for (unsigned int j = 0; j < v.size(); j++)
                        p += out[v[j]];
                    t[i] = p / v.size();
                }
                else
                    t[i] = out[i];
            }
        }
        else
        {
            // init
            for (unsigned int i = 0; i < out.size(); i++)
            {
                
                t[i] = out[i];
            }            
            for(unsigned int i = 0; i < indices.size(); i++)
            {
                BaseMeshTopology::VerticesAroundVertex v = m_topo->getVerticesAroundVertex(indices[i]);
                if (v.size()>0) {
                    Coord p = Coord();
                    for (unsigned int j = 0; j < v.size(); j++)
                        p += out[v[j]];
                    t[indices[i]] = p / v.size();
                }
            }
        }
        for (unsigned int i = 0; i < out.size(); i++) out[i] = t[i];
    }

}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    using sofa::defaulttype::Vec;

    if (!vparams->displayFlags().getShowVisualModels()) return;

    bool wireframe=vparams->displayFlags().getShowWireFrame();

    sofa::core::topology::BaseMeshTopology::SeqTriangles tri = m_topo->getTriangles();

    vparams->drawTool()->saveLastState();
    vparams->drawTool()->setLightingEnabled(true);

    if (this->showInput.getValue())
    {
        helper::ReadAccessor< Data<VecCoord> > in(input_position);

        Vec4f color(1.0f, 0.76078431372f, 0.0f, 1.0f);
        Vec4f specular(0.0f, 0.0f ,0.0f ,0.0f);

        vparams->drawTool()->setMaterial(color, specular, 0.0f);

        std::vector<defaulttype::Vector3> positions;
        std::vector<defaulttype::Vector3> normals;
        std::vector<defaulttype::Vec4f> colors;


        for (unsigned int i=0; i<tri.size(); ++i)
        {
            const Vec<3,Real>& a = in[ tri[i][0] ];
            const Vec<3,Real>& b = in[ tri[i][1] ];
            const Vec<3,Real>& c = in[ tri[i][2] ];
            Vec<3,Real> n = cross((a-b),(a-c));	n.normalize();

            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(color);

            normals.push_back(n);
            normals.push_back(n);
            normals.push_back(n);

            positions.push_back(a);
            positions.push_back(b);
            positions.push_back(c);   
        }

        if (!wireframe)
            vparams->drawTool()->drawTriangles(positions, normals, colors);
        else
            vparams->drawTool()->drawLineStrip(positions, 1, color);
    }

    if (this->showOutput.getValue())
    {
        helper::ReadAccessor< Data<VecCoord> > out(output_position);

        Vec4f color(0.0f, 0.6f, 0.8f, 0.0f);
        Vec4f specular(0.0f, 0.0f, 0.0f, 0.0f);

        vparams->drawTool()->setMaterial(color, specular, 0.0f);

        std::vector<defaulttype::Vector3> positions;
        std::vector<defaulttype::Vector3> normals;
        std::vector<defaulttype::Vec4f> colors;
        
        for (unsigned int i=0; i<tri.size(); ++i)
        {
            const Vec<3,Real>& a = out[ tri[i][0] ];
            const Vec<3,Real>& b = out[ tri[i][1] ];
            const Vec<3,Real>& c = out[ tri[i][2] ];
            Vec<3,Real> n = cross((a-b),(a-c));	n.normalize();
            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(color);

            normals.push_back(n);
            normals.push_back(n);
            normals.push_back(n);

            positions.push_back(a);
            positions.push_back(b);
            positions.push_back(c);

        }

        if (!wireframe)
            vparams->drawTool()->drawTriangles(positions, normals, colors);
        else
            vparams->drawTool()->drawLineStrip(positions, 1, color);
    }

    vparams->drawTool()->restoreLastState();
}

} // namespace engine

} // namespace component

} // namespace sofa

#endif
