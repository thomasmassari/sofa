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


#include <sofa/core/behavior/BaseController.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaUserInteraction/Controller.h>
#include <sofa/core/visual/VisualParams.h>
//#include <SofaBaseVisual/VisualModelImpl.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/ObjectFactory.h>
#include <SofaOpenglVisual/OglVariable.h>


namespace sofa
{
	namespace simulation { class Node; }

	namespace component
	{

		namespace controller
		{

			using namespace sofa::defaulttype;

			class ZygoteBodyDisplayController : public Controller
			{

			public:
				SOFA_CLASS(ZygoteBodyDisplayController, Controller);
				Data<double> scale;
				int visualization_flag;
				int old_visualization_flag;
				ZygoteBodyDisplayController();
				SReal levelAlphaTime;
				SReal levelAlphaTimeMuscle;
				bool key_just_pressed;

				void init();
				void bwdInit();
				void reset();
				void reinit();

				void draw(const sofa::core::visual::VisualParams* vparams);

			private:
				void handleEvent(core::objectmodel::Event *);
				Data<bool> verbose;
				//SReal transparencyLevel;
				void setLayersCoeff(std::string layerName, float alphaLevel, float maxAlphaLevel, float minAlphaLevel);
			};

		} // namespace controller

	} // namespace component

} // namespace sofa