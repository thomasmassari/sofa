/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2015 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define SOFA_COMPONENT_ENGINE_FILTERENGINE_CPP
#include <SofaEngine/FilterEngine.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace engine
{

SOFA_DECL_CLASS(FilterEngine)

int FilterEngineClass = core::RegisterObject("Transform position of 3d points")
#ifdef SOFA_FLOAT
        .add< FilterEngine<defaulttype::Vec3fTypes> >(true) // default template
#else
        .add< FilterEngine<defaulttype::Vec3dTypes> >(true) // default template
#ifndef SOFA_DOUBLE
        .add< FilterEngine<defaulttype::Vec3fTypes> >()
#endif
#endif
#ifndef SOFA_FLOAT
        .add< FilterEngine<defaulttype::Vec1dTypes> >()

#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
        .add< FilterEngine<defaulttype::Vec1fTypes> >()

#endif //SOFA_DOUBLE
        .add< FilterEngine<defaulttype::ExtVec1fTypes> >()
        ;

#ifndef SOFA_FLOAT
template class SOFA_ENGINE_API FilterEngine<defaulttype::Vec1dTypes>;

#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_ENGINE_API FilterEngine<defaulttype::Vec1fTypes>;

#endif //SOFA_DOUBLE
template class SOFA_ENGINE_API FilterEngine<defaulttype::ExtVec1fTypes>;


} // namespace constraint

} // namespace component

} // namespace sofa

