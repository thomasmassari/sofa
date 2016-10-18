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
//
// C++ Implementation : MechanicalStateFilter
//
// Description:
//
//
// Author: ???????????????????????????????????????????????????????
//
// Copyright: See COPYING file that comes with this distribution
//
//
#define SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_CPP
#include <SofaUserInteraction/MechanicalStateFilter.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>


namespace sofa
{

namespace component
{

namespace controller
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(MechanicalStateFilter)

// Register in the Factory
int MechanicalStateFilterClass = core::RegisterObject("Filter articulation position in a mechanical object")
#ifndef SOFA_FLOAT
//.add< MechanicalStateFilter<Vec3dTypes> >()
//.add< MechanicalStateFilter<Vec2dTypes> >()
        .add< MechanicalStateFilter<Vec1dTypes> >()
        
#endif
#ifndef SOFA_DOUBLE
//.add< MechanicalStateFilter<Vec3fTypes> >()
//.add< MechanicalStateFilter<Vec2fTypes> >()
        .add< MechanicalStateFilter<Vec1fTypes> >()

#endif
        ;

#ifndef SOFA_FLOAT
//template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec3dTypes>;
//template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec2dTypes>;
template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec1dTypes>;

#endif
#ifndef SOFA_DOUBLE
//template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec3fTypes>;
//template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec2fTypes>;
template class SOFA_USER_INTERACTION_API MechanicalStateFilter<Vec1fTypes>;

#endif


} // namespace controller

} // namespace component

} // namespace sofa
