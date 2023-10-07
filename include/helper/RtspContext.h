#pragma once
#include "helper/ThreadPool.h"
#include "helper/Scheduler.h"
#include <memory>




struct RtspContext
{
	std::shared_ptr<ThreadPool> thread_pool_;
	std::shared_ptr<Scheduler> scheduler_;

};