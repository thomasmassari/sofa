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
	: gnode(0)
	, verbose(initData(&verbose, false, "verbose", "print out traces"))
	
{
	this->f_listening.setValue(true);
}



void ZygoteBodyDisplayController::init()
{	

	if (!gnode)
		gnode = dynamic_cast<sofa::simulation::Node*>(this->getContext());

	if (!gnode) {
		PRNE("Cannot find node!");
		return;
	}

	gnode->get(filter, core::objectmodel::BaseContext::SearchDown);

	if (filter) {
		PRNS("found stochastic filter: " << filter->getName());
	}
	else
		PRNE(" no stochastic filter found!");

	gnode->get<sofa::component::visualmodel::OglModel>(&listTags, this->getTags(), sofa::core::objectmodel::BaseContext::SearchDown);

	for (size_t i = 0; i < preStochasticWrappers.size(); i++)
		PRNS("pre-stochastic filter " << preStochasticWrappers[i]->getName() << " found");

	
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

void ZygoteBodyDisplayController::handleEvent(core::objectmodel::Event *event)
{
	if (sofa::core::objectmodel::KeypressedEvent::checkEventType(event))
	{
		sofa::core::objectmodel::KeypressedEvent *kpev = static_cast<sofa::core::objectmodel::KeypressedEvent *>(event);

		//Change values for the wireless presenter (22 sx and 23 dx)
		if ((int)kpev->getKey() == 18 && visualization_flag != 0) //Sx
			visualization_flag -= 1;
	
		if ((int)kpev->getKey() == 20 && visualization_flag != 5) //Dx
			visualization_flag += 1;
			

		std::cout << visualization_flag << std::endl;
	}

	if (dynamic_cast<sofa::simulation::AnimateBeginEvent *>(event))
    {	
		
    } // AnimatedBeginEvent
}



int ZygoteBodyDisplayControllerClass = core::RegisterObject("Zygote Body Display Controller Driver")
       .add< ZygoteBodyDisplayController >();

SOFA_DECL_CLASS(ZygoteBodyDisplayController)


} // namespace controller

} // namespace component

} // namespace sofa

