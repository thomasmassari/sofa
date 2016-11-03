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
#ifndef SOFA_COMPONENT_ENGINE_FILTERENGINE_INL
#define SOFA_COMPONENT_ENGINE_FILTERENGINE_INL

#include <sofa/core/objectmodel/Base.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <SofaEngine/FilterEngine.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/helper/rmath.h> //M_PI

#include <cassert>

namespace sofa
{

	namespace component
	{

		namespace engine
		{

			template <class DataTypes>
			FilterEngine<DataTypes>::FilterEngine()
				: f_inputX(initData(&f_inputX, "input_position", "input array of 1d points"))
				, f_outputX(initData(&f_outputX, "output_position", "output array of 1d points"))

			{
				addInput(&f_inputX);
				addOutput(&f_outputX);
				f_listening.setValue(true);
				setDirtyValue();
			}

			template <class DataTypes>
			void FilterEngine<DataTypes>::init()
			{

			}

			template <class DataTypes>
			void FilterEngine<DataTypes>::reinit()
			{
				update();
			}

			template <class DataTypes>
			void FilterEngine<DataTypes>::handleEvent(core::objectmodel::Event *event)
			{
				if (sofa::simulation::AnimateBeginEvent::checkEventType(event))
					this->update();
			}


			template <class DataTypes>
			void FilterEngine<DataTypes>::update()
			{
				//Get input
				const VecCoord& in = f_inputX.getValue();

				cleanDirty();

				VecCoord& out = *(f_outputX.beginWriteOnly());

				//Set Output
				out.resize(in.size());
				//Set the output to the input
				std::copy(in.begin(), in.end(), out.begin());
				std::cout << "Passage" << std::endl;
				//Block articulation
				if ((in[6].x() * 180 / M_PI) < -45.0)
				{
					out[6].x() = -45.0*M_PI / 180;
				}
				f_outputX.endEdit();

			}

		} // namespace engine

	} // namespace component

} // namespace sofa

#endif