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
#include <SofaLoader/ReadStateIteration.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace misc
{

SOFA_DECL_CLASS(ReadStateIterationIteration)

using namespace defaulttype;

int ReadStateIterationClass = core::RegisterObject("Read State vectors from file at each timestep")
        .add< ReadStateIteration >();

ReadStateIterationCreator::ReadStateIterationCreator(const core::ExecParams* params)
    : Visitor(params)
    , sceneName("")
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(false)
    , init(true)
    , counterReadStateIteration(0)
{
}

ReadStateIterationCreator::ReadStateIterationCreator(const std::string &n, bool _createInMapping, const core::ExecParams* params, bool i, int c)
    : Visitor(params)
    , sceneName(n)
#ifdef SOFA_HAVE_ZLIB
    , extension(".txt.gz")
#else
    , extension(".txt")
#endif
    , createInMapping(_createInMapping)
    , init(i)
    , counterReadStateIteration(c)
{
}

//Create a Read State component each time a mechanical state is found
simulation::Visitor::Result ReadStateIterationCreator::processNodeTopDown( simulation::Node* gnode)
{
    sofa::core::behavior::BaseMechanicalState * mstate=gnode->mechanicalState;
    if (!mstate)   return Visitor::RESULT_CONTINUE;
    core::behavior::OdeSolver *isSimulated;
    mstate->getContext()->get(isSimulated);
    if (!isSimulated) return simulation::Visitor::RESULT_CONTINUE;

    //We have a mechanical state
    addReadStateIteration(mstate, gnode);
    return simulation::Visitor::RESULT_CONTINUE;
}

void ReadStateIterationCreator::addReadStateIteration(sofa::core::behavior::BaseMechanicalState *ms, simulation::Node* gnode)
{
    sofa::core::objectmodel::BaseContext* context = gnode->getContext();
    sofa::core::BaseMapping *mapping; context->get(mapping);
    if (createInMapping || mapping== NULL)
    {
        sofa::component::misc::ReadStateIteration::SPtr rs;
        context->get(rs, this->subsetsToManage, core::objectmodel::BaseContext::Local);
        if (rs == NULL)
        {
            rs = sofa::core::objectmodel::New<ReadStateIteration>();
            gnode->addObject(rs);
            for (core::objectmodel::TagSet::iterator it=this->subsetsToManage.begin(); it != this->subsetsToManage.end(); ++it)
                rs->addTag(*it);
        }

        std::ostringstream ofilename;
        ofilename << sceneName << "_" << counterReadStateIteration << "_" << ms->getName()  << "_mstate" << extension ;

        rs->f_filename.setValue(ofilename.str());  rs->f_listening.setValue(false); //Deactivated only called by extern functions
        if (init) rs->init();

        ++counterReadStateIteration;
    }
}

///if state is true, we activate all the write states present in the scene.
simulation::Visitor::Result ReadStateIterationActivator::processNodeTopDown( simulation::Node* gnode)
{
    sofa::component::misc::ReadStateIteration *rs = gnode->get< sofa::component::misc::ReadStateIteration >(this->subsetsToManage);
    if (rs != NULL) { changeStateReader(rs);}

    return simulation::Visitor::RESULT_CONTINUE;
}

void ReadStateIterationActivator::changeStateReader(sofa::component::misc::ReadStateIteration* rs)
{
    rs->reset();
    rs->f_listening.setValue(state);
}


//if state is true, we activate all the write states present in the scene. If not, we activate all the readers.
simulation::Visitor::Result ReadStateIterationModifier::processNodeTopDown( simulation::Node* gnode)
{
    using namespace sofa::defaulttype;

    sofa::component::misc::ReadStateIteration*rs = gnode->get< sofa::component::misc::ReadStateIteration>(this->subsetsToManage);
    if (rs != NULL) {changeTimeReader(rs);}

    return simulation::Visitor::RESULT_CONTINUE;
}

} // namespace misc

} // namespace component

} // namespace sofa
