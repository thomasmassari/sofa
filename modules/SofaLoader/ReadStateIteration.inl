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
#ifndef SOFA_COMPONENT_MISC_READSTATEIERATION_INL
#define SOFA_COMPONENT_MISC_READSTATEIERATION_INL

#include <SofaLoader/ReadStateIteration.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>

#include <string.h>
#include <sstream>

namespace sofa
{

namespace component
{

namespace misc
{

ReadStateIteration::ReadStateIteration()
    : f_filename( initData(&f_filename, "filename", "output file name"))
    , f_interval( initData(&f_interval, 0.0, "interval", "time duration between inputs"))    
    , f_shift( initData(&f_shift, 0.0, "shift", "shift between times in the file and times when they will be read"))
    , f_loop( initData(&f_loop, false, "loop", "set to 'true' to re-read the file when reaching the end"))
    , f_scalePos( initData(&f_scalePos, 1.0, "scalePos", "scale the input mechanical object"))
	, f_pointList(initData(&f_pointList, "pointList", "Point List"))
	, f_currentPointList(initData(&f_currentPointList, "currentPointList", "Current Point List"))
    , mmodel(NULL)
    , infile(NULL)
#ifdef SOFA_HAVE_ZLIB
    , gzfile(NULL)
#endif
    , nextTime(0)
    , lastTime(0)
    , loopTime(0)
	, m_currentStep(0)
	, m_nbStep(0)
{
    this->f_listening.setValue(true);
}

ReadStateIteration::~ReadStateIteration()
{
    if (infile)
        delete infile;
#ifdef SOFA_HAVE_ZLIB
    if (gzfile)
        gzclose(gzfile);
#endif
}

void ReadStateIteration::init()
{
    reset();


	float m_endTime = 0.72;//find end time from file (0.18/0.36/0.72 for fast/normal/slow heart beating)

	float currentTime = 0.0;
	while (currentTime <= m_endTime)
	{
		memProcessReadStateIteration(currentTime);
		currentTime += this->getContext()->getDt();
		m_nbStep++;
	}
	const helper::vector<helper::vector<sofa::defaulttype::Vector3> >& pointList = f_pointList.getValue();
	const helper::vector<sofa::defaulttype::Vector3>& linePointList = pointList[0];
	f_currentPointList.setValue(linePointList);
	m_currentStep++;
	/*std::cout << pointList.size() << std::endl;
	for (unsigned int i = 0; i < pointList.size(); i++)
		std::cout << pointList[i].size() << std::endl;*/
}

void ReadStateIteration::reset()
{
    mmodel = this->getContext()->getMechanicalState();
    if (infile)
    {
        delete infile;
        infile = NULL;
    }
#ifdef SOFA_HAVE_ZLIB
    if (gzfile)
    {
        gzclose(gzfile);
        gzfile = NULL;
    }
#endif

    const std::string& filename = f_filename.getFullPath();
    if (filename.empty())
    {
        serr << "ERROR: empty filename"<<sendl;
    }
#ifdef SOFA_HAVE_ZLIB
    else if (filename.size() >= 3 && filename.substr(filename.size()-3)==".gz")
    {
        gzfile = gzopen(filename.c_str(),"rb");
        if( !gzfile )
        {
            serr << "Error opening compressed file "<<filename<<sendl;
        }
    }
#endif
    else
    {
        infile = new std::ifstream(filename.c_str());
        if( !infile->is_open() )
        {
            serr << "Error opening file "<<filename<<sendl;
            delete infile;
            infile = NULL;
        }
    }
    nextTime = 0;
    lastTime = 0;
    loopTime = 0;


}

void ReadStateIteration::handleEvent(sofa::core::objectmodel::Event* event)
{
    if (/* simulation::AnimateBeginEvent* ev = */simulation::AnimateBeginEvent::checkEventType(event))
    {
        //processReadStateIteration();


		const helper::vector<helper::vector<sofa::defaulttype::Vector3> >& pointList = f_pointList.getValue();
		const helper::vector<sofa::defaulttype::Vector3>& linePointList = pointList[m_currentStep%m_nbStep];

		if (linePointList.size() == 0)
		{
			const helper::vector<sofa::defaulttype::Vector3>& linePointList = pointList[m_currentStep%m_nbStep-1];
			f_currentPointList.setValue(linePointList);
			m_currentStep++;
		}
		else
		f_currentPointList.setValue(linePointList);
		m_currentStep++;

		//for (unsigned int i = 0; i < linePointList.size(); i++)
			

    }
    if (/* simulation::AnimateEndEvent* ev = */simulation::AnimateEndEvent::checkEventType(event))
    {

    }
}



void ReadStateIteration::setTime(double time)
{
    if (time+getContext()->getDt()*0.5 < lastTime) {reset();}
}

void ReadStateIteration::changeProcessReadStateIteration(double time)
{
    if (time == lastTime) return;
    setTime(time);
    processReadStateIteration();
}

bool ReadStateIteration::readNext(double time, std::vector<std::string>& validLines)
{
    if (!mmodel) return false;
    if (!infile
#ifdef SOFA_HAVE_ZLIB
        && !gzfile
#endif
       )
        return false;
    lastTime = time;
    validLines.clear();
    std::string line, cmd;
    while (nextTime <= time)
    {
#ifdef SOFA_HAVE_ZLIB
        if (gzfile)
        {
            if (gzeof(gzfile))
            {
                if (!f_loop.getValue())
                    break;
                gzrewind(gzfile);
                loopTime = nextTime;
            }
            //getline(gzfile, line);
            line.clear();
            char buf[4097];
            buf[0] = '\0';
            while (gzgets(gzfile,buf,sizeof(buf))!=NULL && buf[0])
            {
                size_t l = strlen(buf);
                if (buf[l-1] == '\n')
                {
                    buf[l-1] = '\0';
                    line += buf;
                    break;
                }
                else
                {
                    line += buf;
                    buf[0] = '\0';
                }
            }
        }
        else
#endif
        if (infile)
        {
            if (infile->eof())
            {
                if (!f_loop.getValue())
                    break;
                infile->clear();
                infile->seekg(0);
                loopTime = nextTime;
            }
            getline(*infile, line);
        }
        //sout << "line= "<<line<<sendl;
        std::istringstream str(line);
        str >> cmd;
        if (cmd == "T=")
        {
            str >> nextTime;
            nextTime += loopTime;
            //sout << "next time: " << nextTime << sendl;
            if (nextTime <= time)
                validLines.clear();
        }
        if (nextTime <= time)
            validLines.push_back(line);
    }
    return true;
}

void ReadStateIteration::processReadStateIteration()
{
    double time = this->getContext()->getTime() + f_shift.getValue();
	//std::cout << "T " << time << std::endl;
    std::vector<std::string> validLines;
    if (!readNext(time, validLines)) return;
    bool updated = false;
    for (std::vector<std::string>::iterator it=validLines.begin(); it!=validLines.end(); ++it)
    {
        std::istringstream str(*it);
        std::string cmd;
        str >> cmd;
        if (cmd == "X=")
        {
            mmodel->readVec(core::VecId::position(), str);            
            mmodel->applyScale(f_scalePos.getValue(), f_scalePos.getValue(), f_scalePos.getValue());

            updated = true;
        }
        else if (cmd == "V=")
        {
            mmodel->readVec(core::VecId::velocity(), str);
            updated = true;
        }
    }

    if (updated)
    {
        //sout<<"update from file"<<sendl;
        /*sofa::simulation::MechanicalPropagatePositionAndVelocityVisitor action1(core::MechanicalParams::defaultInstance());
        this->getContext()->executeVisitor(&action1);
        sofa::simulation::UpdateMappingVisitor action2(core::MechanicalParams::defaultInstance());
        this->getContext()->executeVisitor(&action2);*/
    }
}

void ReadStateIteration::memProcessReadStateIteration(const SReal currentTime)
{
	double time = currentTime + f_shift.getValue();
	std::vector<std::string> validLines;
	if (!readNext(time, validLines)) return;
	bool updated = false;
	helper::vector<helper::vector<sofa::defaulttype::Vector3> >& pointList = *f_pointList.beginEdit();
	helper::vector<sofa::defaulttype::Vector3> currentList;

	//std::cout << validLines.size() << std::endl;
	for (std::vector<std::string>::iterator it = validLines.begin(); it != validLines.end(); ++it)
	{
		std::istringstream str(*it);
		std::string cmd;
		str >> cmd;
		if (cmd == "X=")
		{
			sofa::defaulttype::Vector3 coord;

			while (str >> coord)
			{
				currentList.push_back(coord);
			}

			//mmodel->readVec(core::VecId::position(), str);
		}
		
	}
	pointList.push_back(currentList);
	f_pointList.endEdit();
}


} // namespace misc

} // namespace component

} // namespace sofa

#endif
