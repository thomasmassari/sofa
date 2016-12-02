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
#ifndef SOFA_COMPONENT_MISC_READSTATEIERATION_H
#define SOFA_COMPONENT_MISC_READSTATEIERATION_H
#include "config.h"

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/behavior/BaseMechanicalState.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/core/objectmodel/DataFileName.h>
#include <sofa/core/ExecParams.h>

#ifdef SOFA_HAVE_ZLIB
#include <zlib.h>
#endif

#include <fstream>

namespace sofa
{

namespace component
{

namespace misc
{

/** Read State vectors from file at each timestep
*/
class SOFA_LOADER_API ReadStateIteration: public core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(ReadStateIteration,core::objectmodel::BaseObject);

    sofa::core::objectmodel::DataFileName f_filename;
    Data < double > f_interval;
    Data < double > f_shift;
    Data < bool > f_loop;
    Data < double > f_scalePos;
	Data < helper::vector<helper::vector<sofa::defaulttype::Vector3> > > f_pointList;
	Data < helper::vector<sofa::defaulttype::Vector3> > f_currentPointList;

	unsigned int m_currentStep;
	unsigned int m_nbStep;

protected:
    core::behavior::BaseMechanicalState* mmodel;
    std::ifstream* infile;
#ifdef SOFA_HAVE_ZLIB
    gzFile gzfile;
#endif
    double nextTime;
    double lastTime;
    double loopTime;

    ReadStateIteration();

    virtual ~ReadStateIteration();
public:
    virtual void init();

    virtual void reset();

    void setTime(double time);

    virtual void handleEvent(sofa::core::objectmodel::Event* event);

    void processReadStateIteration();
	void memProcessReadStateIteration(const SReal currentTime);
    void changeProcessReadStateIteration(double time);

    /// Read the next values in the file corresponding to the last timestep before the given time
    bool readNext(double time, std::vector<std::string>& lines);

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T* obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (context->getMechanicalState() == NULL)
            return false;
        return BaseObject::canCreate(obj, context, arg);
    }


};


///Create ReadStateIteration component in the graph each time needed
class SOFA_LOADER_API ReadStateIterationCreator: public Visitor
{
public:
    ReadStateIterationCreator(const core::ExecParams* params);
    ReadStateIterationCreator(const std::string &n, bool _createInMapping, const core::ExecParams* params, bool i=true, int c=0);
    virtual Result processNodeTopDown( simulation::Node*  );

    void setSceneName(std::string &n) { sceneName = n;}
    void setCounter(int c) {counterReadStateIteration = c;};
    void setCreateInMapping(bool b) {createInMapping=b;}
    virtual const char* getClassName() const { return "ReadStateIterationCreator"; }
protected:
    void addReadStateIteration(sofa::core::behavior::BaseMechanicalState *ms, simulation::Node* gnode);
    std::string sceneName;
    std::string extension;
    bool createInMapping;
    bool init;
    int counterReadStateIteration; //avoid to have two same files if two mechanical objects has the same name
};

class SOFA_LOADER_API ReadStateIterationActivator: public Visitor
{
public:
    ReadStateIterationActivator(const core::ExecParams* params, bool active) : Visitor(params), state(active) {}
    virtual Result processNodeTopDown( simulation::Node*  );

    bool getState() const {return state;};
    void setState(bool active) {state=active;};
    virtual const char* getClassName() const { return "ReadStateIterationActivator"; }
protected:
    void changeStateReader(sofa::component::misc::ReadStateIteration *ws);

    bool state;
};

class SOFA_LOADER_API ReadStateIterationModifier: public simulation::Visitor
{
public:
    ReadStateIterationModifier(const core::ExecParams* params, double _time) : Visitor(params), time(_time) {}
    virtual Result processNodeTopDown( simulation::Node*  );

    double getTime() const { return time; }
    void setTime(double _time) { time=_time; }
    virtual const char* getClassName() const { return "ReadStateIterationModifier"; }
protected:
    void changeTimeReader(sofa::component::misc::ReadStateIteration *rs) { rs->changeProcessReadStateIteration(time); }

    double time;
};

} // namespace misc

} // namespace component

} // namespace sofa

#endif
