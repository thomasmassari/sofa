#include "Tasks.h"
#include "TaskSchedulerBoost.h"
#include <boost/thread.hpp>

namespace sofa
{

	namespace simulation
	{


		Task::Task(const Task::Status* pStatus) 
			: m_Status(pStatus)
, execTime(ctime_t(),ctime_t()), execThreadIndex(-1)
{
}

bool Task::runTask(WorkerThread* thread)
{
    execThreadIndex = thread->getThreadIndex();
    execTime.first = sofa::helper::system::thread::CTime::getFastTime();
    bool res = run(thread);
    execTime.second = sofa::helper::system::thread::CTime::getFastTime();
    return res;
}

const char* Task::getName()
		{
    return "Task";
		}

		Task::~Task()
		{
			//m_Status;
            //delete this;
		}

		Task::Color Task::getColor()
		{
			return Color(0.5f,0.5f,0.5f,1.0f);
		}


	} // namespace simulation

} // namespace sofa
