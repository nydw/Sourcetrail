#include "utility/scheduling/TaskGroupParallel.h"

#include <thread>

TaskGroupParallel::TaskGroupParallel()
{
}

TaskGroupParallel::~TaskGroupParallel()
{
}

void TaskGroupParallel::enter()
{
	m_interrupt = false;
	m_running = false;
	m_activeTaskCount = 0;
}

Task::TaskState TaskGroupParallel::update()
{
	if (!m_running)
	{
		for (size_t i = 0; i < m_tasks.size(); i++)
		{
			std::thread(&TaskGroupParallel::processTask, this, m_tasks[i]).detach();

			std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
			m_activeTaskCount++;
		}
		m_running = true;
	}

	int activeTaskCount = 0;
	{
		std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
		activeTaskCount = m_activeTaskCount;
	}

	if (activeTaskCount == 0)
	{
		return (m_interrupt ? STATE_CANCELED : STATE_FINISHED);
	}

	return Task::STATE_RUNNING;
}

void TaskGroupParallel::exit()
{
}

void TaskGroupParallel::interrupt()
{
	m_interrupt = true;
}

void TaskGroupParallel::revert()
{
	m_interrupt = true;
}


void TaskGroupParallel::processTask(std::shared_ptr<Task> task)
{
	Task::TaskState state = Task::STATE_NEW;
	while (state != Task::STATE_FINISHED && state != Task::STATE_CANCELED)
	{
		state = task->process(m_interrupt);
	}

	{
		std::lock_guard<std::mutex> lock(m_activeTaskCountMutex);
		m_activeTaskCount--; // not safe! if exception hits this thread before this point the count is not decremented.
	}
}