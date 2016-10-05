/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_FIXEDTRANSLATIONCONSTRAINT_CPP
#include <SofaBoundaryCondition/FixedTranslationConstraint.inl>
#include <sofa/core/ObjectFactory.h>

#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace component
{

namespace projectiveconstraintset
{

using namespace sofa::defaulttype;
using namespace sofa::helper;

SOFA_DECL_CLASS(FixedTranslationConstraint)

int FixedTranslationConstraintClass = core::RegisterObject("Attach given rigids to their initial positions but they still can have rotations")
#ifndef SOFA_FLOAT
        .add< FixedTranslationConstraint<Rigid3dTypes> >()
        .add< FixedTranslationConstraint<Rigid2dTypes> >()
        .add< FixedTranslationConstraint<Vec6dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< FixedTranslationConstraint<Rigid3fTypes> >()
        .add< FixedTranslationConstraint<Rigid2fTypes> >()
        .add< FixedTranslationConstraint<Vec6fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Rigid3dTypes>;
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Rigid2dTypes>;
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Vec6dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Rigid3fTypes>;
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Rigid2fTypes>;
template class SOFA_BOUNDARY_CONDITION_API FixedTranslationConstraint<Vec6fTypes>;
#endif

#ifndef SOFA_FLOAT
template <>
void FixedTranslationConstraint<Vec6dTypes>::draw(const core::visual::VisualParams* vparams)
{
    const SetIndexArray & indices = f_indices.getValue();
    if (!vparams->displayFlags().getShowBehaviorModels())
        return;
    const VecCoord& x = this->mstate->read(core::ConstVecCoordId::position())->getValue();

    vparams->drawTool()->saveLastState();
    vparams->drawTool()->setLighting(false);

    std::vector<defaulttype::Vector3> positions;
    defaulttype::Vec4f color(1, 0.5, 0.5, 1);

    if (f_fixAll.getValue() == true)
    {
        for (unsigned i = 0; i < x.size(); i++)
        {
            defaulttype::Vec3d p(x[i][0], x[i][1], x[i][2]);
            positions.push_back(defaulttype::Vector3(p[0], p[1], p[2]));
        }
    }
    else
    {
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            defaulttype::Vec3d p(x[*it][0], x[*it][1], x[*it][2]);
            positions.push_back(defaulttype::Vector3(p[0], p[1], p[2]));
        }
    }

    vparams->drawTool()->drawPoints(positions, 10.0, color);

    vparams->drawTool()->restoreLastState();
}
#endif
#ifndef SOFA_DOUBLE
template <>
void FixedTranslationConstraint<Vec6fTypes>::draw(const core::visual::VisualParams* vparams)
{
    const SetIndexArray & indices = f_indices.getValue();
    if (!vparams->displayFlags().getShowBehaviorModels())
        return;
    const VecCoord& x = this->mstate->read(core::ConstVecCoordId::position())->getValue();
    vparams->drawTool()->saveLastState();
    vparams->drawTool()->setLighting(false);

    std::vector<defaulttype::Vector3> positions;
    defaulttype::Vec4f color(1, 0.5, 0.5, 1);
    if (f_fixAll.getValue() == true)
    {
        for (unsigned i = 0; i < x.size(); i++)
        {
            defaulttype::Vec3f p(x[i][0], x[i][1], x[i][2]);
            positions.push_back(defaulttype::Vector3(p[0], p[1], p[2]));
        }
    }
    else
    {
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            defaulttype::Vec3f p(x[*it][0], x[*it][1], x[*it][2]);
            positions.push_back(defaulttype::Vector3(p[0], p[1], p[2]));
        }
    }
    vparams->drawTool()->drawPoints(positions, 10.0, color);

    vparams->drawTool()->restoreLastState();
}
#endif
} // namespace projectiveconstraintset

} // namespace component

} // namespace sofa

