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
* Authors: The SOFA Team and external contributor (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/

#include "ZygoteBodyDisplayController.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/HapticDeviceEvent.h>

#include <SofaBaseVisual/VisualTransform.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/MouseEvent.h>

#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>

namespace sofa
{

namespace component
{

namespace controller
{

	using namespace sofa::defaulttype;


	// flags that the controller manager system can set to tell the graphics system to draw the instructions
	// for the player
	static bool controller_manager_screen_visible = true;
	std::string controller_manager_text_string;

	ZygoteBodyDisplayController::ZygoteBodyDisplayController()
		: verbose(initData(&verbose, false, "verbose", "print out traces"))

	{
		this->f_listening.setValue(true);
		visualization_flag = 1;
		old_visualization_flag = 0;
		key_just_pressed = false;
		levelAlphaTime = 1.0;
	}



	void ZygoteBodyDisplayController::init()
	{
		setLayersCoeff("organsCoeff", 0.0);
		setLayersCoeff("bonesCoeff", 0.0);
		setLayersCoeff("arteryCoeff", 0.0);
		setLayersCoeff("musclesCoeff", 0.0);
		//getTaggedOglModel("organs", 0.0);

	}

	void ZygoteBodyDisplayController::bwdInit()
	{
		sout << "ZygoteBodyDisplayController::bwdInit()" << sendl;
	}

	void ZygoteBodyDisplayController::reset()
	{
		sout << "ZygoteBodyDisplayController::reset()" << sendl;
		//Reset useful values
		this->reinit();
	}

	void ZygoteBodyDisplayController::reinit()
	{
		this->bwdInit();
	}

	void ZygoteBodyDisplayController::draw(const sofa::core::visual::VisualParams* vparams)
	{
		if (!vparams->displayFlags().getShowVisualModels()) return;

	}

	void ZygoteBodyDisplayController::setLayersCoeff(std::string layerName, float alphaLevel)
	{
		sofa::helper::vector<sofa::component::visualmodel::OglFloatVariable::SPtr> vectorVariables;

		this->getContext()->get<sofa::component::visualmodel::OglFloatVariable, sofa::helper::vector<sofa::component::visualmodel::OglFloatVariable::SPtr> >
			(&vectorVariables, sofa::core::objectmodel::BaseContext::Local);

		for (int i = 0; i < vectorVariables.size(); i++)
		{
			if (vectorVariables[i]->getName() == layerName)
				vectorVariables[i]->value.setValue(alphaLevel);

		}
	}

	void ZygoteBodyDisplayController::handleEvent(core::objectmodel::Event *event)
	{
		/*core::objectmodel::Tag tag("skin");
		core::objectmodel::TagSet tagSet(tag);*/

		if (sofa::core::objectmodel::KeypressedEvent::checkEventType(event) || key_just_pressed == true)
		{
			sofa::core::objectmodel::KeypressedEvent *kpev = static_cast<sofa::core::objectmodel::KeypressedEvent *>(event);

			if (key_just_pressed == false)
			{
				old_visualization_flag = visualization_flag;
				//Change values for the wireless presenter (22 sx and 23 dx) (18 sx 20 dx for keyboard)
				if ((int)kpev->getKey() == 22 && visualization_flag != 1) { //Sx
					visualization_flag -= 1;
				}

				if ((int)kpev->getKey() == 23 && visualization_flag != 5) { //Dx
					visualization_flag += 1;
				}

				key_just_pressed = true;

			}

			if (visualization_flag != old_visualization_flag)
			{
				if (visualization_flag > old_visualization_flag)
				{
					switch (visualization_flag) {
					case 1:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 0.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 1.0);
						break;

					case 2:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 0.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 1.0-levelAlphaTime);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 3:
						setLayersCoeff("organsCoeff", 1.0 - levelAlphaTime);
						setLayersCoeff("bonesCoeff", 1.0 - levelAlphaTime);
						setLayersCoeff("arteryCoeff", 1.0 - levelAlphaTime);
						setLayersCoeff("musclesCoeff", levelAlphaTime);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 4:
						setLayersCoeff("organsCoeff", 1.0);
						setLayersCoeff("bonesCoeff", 1.0);
						setLayersCoeff("arteryCoeff", levelAlphaTime);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 5:
						setLayersCoeff("organsCoeff", levelAlphaTime);
						setLayersCoeff("bonesCoeff", 1.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					default:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 0.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 1.0);
						break;
					}
				}

				if (visualization_flag < old_visualization_flag)
				{
					switch (visualization_flag) {
					case 1:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 0.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", levelAlphaTime);
						//setLayersCoeff("skinCoeff", levelAlphaTime);
						break;

					case 2:
						setLayersCoeff("organsCoeff", levelAlphaTime);
						setLayersCoeff("bonesCoeff", levelAlphaTime);
						setLayersCoeff("arteryCoeff", levelAlphaTime);
						setLayersCoeff("musclesCoeff", 1.0-levelAlphaTime);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 3:
						setLayersCoeff("organsCoeff", 1.0);
						setLayersCoeff("bonesCoeff", 1.0);
						setLayersCoeff("arteryCoeff", 1.0-levelAlphaTime);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 4:
						setLayersCoeff("organsCoeff", 1.0- levelAlphaTime);
						setLayersCoeff("bonesCoeff", 1.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					case 5:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 1.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 0.0);
						break;

					default:
						setLayersCoeff("organsCoeff", 0.0);
						setLayersCoeff("bonesCoeff", 0.0);
						setLayersCoeff("arteryCoeff", 0.0);
						setLayersCoeff("musclesCoeff", 0.0);
						//setLayersCoeff("skinCoeff", 1.0);
						break;
					}
				}
			}


		}


		if (dynamic_cast<sofa::simulation::AnimateEndEvent *>(event))
		{
			if (key_just_pressed == true)
			{
				levelAlphaTime -= 0.04;
				//std::cout << "Alpha level: " << 1.0-levelAlpha << std::endl;
				if (levelAlphaTime < -0.01)
				{
					key_just_pressed = false;
					levelAlphaTime = 1.0;
				}
			}


		}
	}



	int ZygoteBodyDisplayControllerClass = core::RegisterObject("Zygote Body Display Controller Driver")
		.add< ZygoteBodyDisplayController >();

	SOFA_DECL_CLASS(ZygoteBodyDisplayController)


} // namespace controller

} // namespace component

} // namespace sofa