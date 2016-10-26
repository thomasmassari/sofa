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
    transparencyLevel = 1.0;
}



void ZygoteBodyDisplayController::init()
{	

}

void ZygoteBodyDisplayController::bwdInit()
{
    sout<<"ZygoteBodyDisplayController::bwdInit()"<<sendl;	
}

void ZygoteBodyDisplayController::reset()
{
    sout<<"ZygoteBodyDisplayController::reset()" << sendl;
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

void ZygoteBodyDisplayController::getTaggedOglModel(std::string tagName)
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
            std::cout << ">>>>>>>> Found a visual node with tag: " << tagSet << " <<<<<<<<<<" << std::endl;
            oglModel->setColor(0.0, 0.0, 1.0, transparencyLevel);
        }
        else {
            std::cout << " >>>>>>>> NO visual node with tag: " << tagSet << " <<<<<<<<<<" << std::endl;
        }
    }

}

void ZygoteBodyDisplayController::handleEvent(core::objectmodel::Event *event)
{
    core::objectmodel::Tag tag("skin");
    core::objectmodel::TagSet tagSet(tag);

    if (sofa::core::objectmodel::KeypressedEvent::checkEventType(event))
	{
		sofa::core::objectmodel::KeypressedEvent *kpev = static_cast<sofa::core::objectmodel::KeypressedEvent *>(event);

		//Change values for the wireless presenter (22 sx and 23 dx)
        if ((int)kpev->getKey() == 18 && visualization_flag != 0) { //Sx
			visualization_flag -= 1;
        }
	
        if ((int)kpev->getKey() == 20 && visualization_flag != 5) { //Dx
			visualization_flag += 1;
        }
			
        switch (visualization_flag) {
        case 1:
            getTaggedOglModel("skin");
        break;

        case 2:
            getTaggedOglModel("muscles");
            getTaggedOglModel("vessels");
            getTaggedOglModel("nerves");
            getTaggedOglModel("organs");
        break;

        case 3:
            getTaggedOglModel("vessels");
            getTaggedOglModel("nerves");
            getTaggedOglModel("organs");
        break;

        case 4:
            getTaggedOglModel("vessels");
            getTaggedOglModel("organs");
        break;

        case 5:
            getTaggedOglModel("organs");
        break;

        default:
            getTaggedOglModel("skin");
            break;
        }
	}

    if (dynamic_cast<sofa::simulation::AnimateEndEvent *>(event))
    {	
        // change transparency smoothly
        std::cout << "Hello from ZygoteBodyDisplayController" << std::endl;
        transparencyLevel = transparencyLevel - 0.01;
    }
}



int ZygoteBodyDisplayControllerClass = core::RegisterObject("Zygote Body Display Controller Driver")
       .add< ZygoteBodyDisplayController >();

SOFA_DECL_CLASS(ZygoteBodyDisplayController)


} // namespace controller

} // namespace component

} // namespace sofa

