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
				levelAlpha = 1.0;
			}



			void ZygoteBodyDisplayController::init()
			{
				getTaggedOglModel("skin", 1.0);
				getTaggedOglModel("muscles", 0.0);
				getTaggedOglModel("vessels", 0.0);
				getTaggedOglModel("nerves", 0.0);
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

			void ZygoteBodyDisplayController::getTaggedOglModel(std::string tagName, SReal transparencyLevel)
			{
				core::objectmodel::Tag tag(tagName);
				core::objectmodel::TagSet tagSet(tag);

				sofa::core::objectmodel::BaseContext* context = this->getContext();
				if (!context) {
					std::cout << "ERROR: no context found in ZygoteBodyDisplayController" << std::endl;
				}
				else {
					oglModel = NULL;
					context->get(oglModel, tagSet, core::objectmodel::BaseContext::SearchRoot);
					if (oglModel) {

						if (tagName == "skin")
							oglModel->setColor(1.0, 0.7, 0.6, transparencyLevel);
						if (tagName == "muscles")
							oglModel->setColor(0.8, 0.4, 0.2, transparencyLevel);
						if (tagName == "vessels")
							oglModel->setColor(0.7, 0.0, 0.0, transparencyLevel);
						if (tagName == "nerves")
							oglModel->setColor(1.0, 0.9, 0.4, transparencyLevel);
						
						//std::cout << ">>>>>>>> Found a visual node with tag: " << tagSet << " <<<<<<<<<<" << std::endl;
						
					}
					else {
						//std::cout << " >>>>>>>> NO visual node with tag: " << tagSet << " <<<<<<<<<<" << std::endl;
					}
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
								getTaggedOglModel("skin", 1.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 0.0);
								getTaggedOglModel("vessels", 0.0);
								//getTaggedOglModel("organs", 0.0);
								break;

							case 2:
								getTaggedOglModel("skin", levelAlpha);
								getTaggedOglModel("muscles", 1.0 - levelAlpha);
								getTaggedOglModel("nerves", 1.0 - levelAlpha);
								getTaggedOglModel("vessels", 1.0 - levelAlpha);
								//getTaggedOglModel("organs", 0.0);
								break;

							case 3:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", levelAlpha);
								getTaggedOglModel("nerves", 1.0);
								getTaggedOglModel("vessels", 1.0);
								//getTaggedOglModel("organs", 1.0);
								break;

							case 4:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", levelAlpha);
								getTaggedOglModel("vessels", 1.0);
								//getTaggedOglModel("organs", 1.0);
								break;

							case 5:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 0.0);
								getTaggedOglModel("vessels", levelAlpha);
								//getTaggedOglModel("organs", 1.0);
								break;

							default:
								getTaggedOglModel("skin", 1.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 0.0);
								getTaggedOglModel("vessels", 0.0);
								//getTaggedOglModel("organs", 0.0);
								break;
							}
						}
						
						if (visualization_flag < old_visualization_flag)
						{
							switch (visualization_flag) {
							case 1:
								getTaggedOglModel("skin", 1.0 - levelAlpha);
								getTaggedOglModel("muscles", levelAlpha);
								getTaggedOglModel("nerves", levelAlpha);
								getTaggedOglModel("vessels", levelAlpha);
								//getTaggedOglModel("organs", 0.0);
								break;

							case 2:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", 1.0 - levelAlpha);
								getTaggedOglModel("nerves", 1.0);
								getTaggedOglModel("vessels", 1.0);
								//getTaggedOglModel("organs", 0.0);
								break;

							case 3:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 1.0 - levelAlpha);
								getTaggedOglModel("vessels", 1.0);
								//getTaggedOglModel("organs", 1.0);
								break;

							case 4:
								getTaggedOglModel("skin", 0.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 0.0);
								getTaggedOglModel("vessels", 1.0 - levelAlpha);
								//getTaggedOglModel("organs", 1.0);
								break;

							default:
								getTaggedOglModel("skin", 1.0);
								getTaggedOglModel("muscles", 0.0);
								getTaggedOglModel("nerves", 0.0);
								getTaggedOglModel("vessels", 0.0);
								//getTaggedOglModel("organs", 0.0);
								break;
							}
						}
					}
					

				}


				if (dynamic_cast<sofa::simulation::AnimateEndEvent *>(event))
				{
					if (key_just_pressed == true)
					{
						levelAlpha -= 0.04;
						//std::cout << "Alpha level: " << 1.0-levelAlpha << std::endl;
						if (levelAlpha < -0.01)
						{
							key_just_pressed = false;
							levelAlpha = 1.0;
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