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
// C++ Models: MechanicalStateFilter
//
// Description:
//
//
// Author: Pierre-Jean Bensoussan, Digital Trainers (2008)
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_INL
#define SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_INL

#include <SofaUserInteraction/MechanicalStateFilter.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>

namespace sofa
{

namespace component
{

namespace controller
{

template <class DataTypes>
MechanicalStateFilter<DataTypes>::MechanicalStateFilter()
    : indices(initData(&indices, "indices", "indices of the constraint articulation"))
	, range(initData(&range, "range", "range of freedom beetwen a max e min value for each articulation in index"))
    //, mainDirection( initData(&mainDirection, sofa::defaulttype::Vec<3,Real>((Real)0.0, (Real)0.0, (Real)-1.0), "mainDirection", "Main direction and orientation of the controlled DOF") )
{
 
}

template <class DataTypes>
void MechanicalStateFilter<DataTypes>::init()
{
    using core::behavior::MechanicalState;
    mState = dynamic_cast<MechanicalState<DataTypes> *> (this->getContext()->getMechanicalState());
	f_listening.setValue(TRUE);
    if (!mState)
        serr << "MechanicalStateFilter has no binding MechanicalState" << sendl;
}


template <class DataTypes>
void MechanicalStateFilter<DataTypes>::onBeginAnimationStep(const double /*dt*/)
{
	using sofa::defaulttype::Vec;


	if (mState)
	{
		helper::WriteAccessor<Data<VecCoord> > x = *this->mState->write(core::VecCoordId::position());
		helper::WriteAccessor<Data<VecCoord> > xfree = *this->mState->write(core::VecCoordId::freePosition());
		
		//std::cout << indices.getValue().size() << std::endl;
		/*std::cout << "Articulation 3: " << x[3].x() * 180 / M_PI << std::endl;
		std::cout << "Articulation 4: " << x[4].x() * 180 / M_PI << std::endl;
		std::cout << "Articulation 5: " << x[5].x() * 180 / M_PI << std::endl;*/

		for(int i = 0; i < indices.getValue().size(); i++)
		{
			//Max
			if ((x[indices.getValue()[i]].x() * 180 / M_PI) < range.getValue()[2 * i + 1])
				x[indices.getValue()[i]].x() = (range.getValue()[2 * i + 1]) * M_PI / 180;

			//Min
			if ((x[indices.getValue()[i]].x() * 180 / M_PI) > range.getValue()[2 * i])
				x[indices.getValue()[i]].x() = (range.getValue()[2 * i]) * M_PI / 180;
	
		}
		

		

	}


	sofa::simulation::Node *node = static_cast<sofa::simulation::Node*> (this->getContext());
	sofa::simulation::MechanicalPropagatePositionAndVelocityVisitor mechaVisitor(core::MechanicalParams::defaultInstance()); mechaVisitor.execute(node);
	sofa::simulation::UpdateMappingVisitor updateVisitor(core::ExecParams::defaultInstance()); updateVisitor.execute(node);
}



template <class DataTypes>
core::behavior::MechanicalState<DataTypes> *MechanicalStateFilter<DataTypes>::getMechanicalState() const
{
    return mState;
}



template <class DataTypes>
void MechanicalStateFilter<DataTypes>::setMechanicalState(core::behavior::MechanicalState<DataTypes> *_mState)
{
    mState = _mState;
}



template <class DataTypes>
unsigned int MechanicalStateFilter<DataTypes>::getIndex() const
{
    return index.getValue();
}



template <class DataTypes>
void MechanicalStateFilter<DataTypes>::setIndex(const unsigned int _index)
{
    index.setValue(_index);
}

} // namespace controller

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_INL
