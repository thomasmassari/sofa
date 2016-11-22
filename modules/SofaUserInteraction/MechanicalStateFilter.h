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
// C++ Interface: MechanicalStateFilter
//
// Description:
//
//
// Author: Pierre-Jean Bensoussan, Digital Trainers (2008)
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_H
#define SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_H
#include "config.h"

#include <SofaUserInteraction/Controller.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <SofaBaseTopology/TopologySubsetData.h>

namespace sofa
{

namespace component
{

namespace controller
{



/**
 * @brief MechanicalStateFilter Class
 *
 * Provides a Mouse & Keyboard user control on a Mechanical State.
 * On a Rigid Particle, relative and absolute control is available.
 */
template<class DataTypes>
class MechanicalStateFilter : public Controller
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(MechanicalStateFilter,DataTypes),Controller);
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord    Coord   ;
    typedef typename DataTypes::Deriv    Deriv   ;
    typedef typename Coord::value_type   Real    ;
	typedef sofa::helper::vector<float> VecFloat;

	Data<VecFloat> indices;
	Data<VecFloat> range;

protected:
    /**
     * @brief Default Constructor.
     */
    MechanicalStateFilter();

    /**
     * @brief Default Destructor.
     */
    virtual ~MechanicalStateFilter() {};

	double kineticEnergy;
public:
	float T;
	std::ofstream outfile;
    /**
     * @brief SceneGraph callback initialization method.
     */
    void init();

  
    /**
     * @brief Begin Animation event callback.
     */
    void onBeginAnimationStep(const double dt);

    //@}

    /**
     * @brief Return the controlled MechanicalState.
     */
    core::behavior::MechanicalState<DataTypes> *getMechanicalState(void) const;

    /**
     * @brief Set a MechanicalState to the controller.
     */
    void setMechanicalState(core::behavior::MechanicalState<DataTypes> *);

    /**
     * @brief Return the index of the controlled DOF of the MechanicalState.
     */
    unsigned int getIndex(void) const;

    /**
     * @brief Set the index of the controlled DOF of the MechanicalState.
     */
    void setIndex(const unsigned int);


    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const MechanicalStateFilter<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }
protected:

    Data< unsigned int > index; ///< Controlled DOF index.

    core::behavior::MechanicalState<DataTypes> *mState; ///< Controlled MechanicalState.

    sofa::defaulttype::Vector3 position;

};

#if defined(SOFA_EXTERN_TEMPLATE) && !defined(SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_CPP)
#ifndef SOFA_FLOAT
//extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec3dTypes>;
//extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec2dTypes>;
extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec1dTypes>;


#endif
#ifndef SOFA_DOUBLE
//extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec3fTypes>;
//extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec2fTypes>;
extern template class SOFA_USER_INTERACTION_API MechanicalStateFilter<defaulttype::Vec1fTypes>;

#endif
#endif

} // namespace controller

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_CONTROLLER_MECHANICALSTATEFILTER_H
