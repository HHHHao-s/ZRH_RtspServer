#pragma once
#include "helper/ThreadPool.h"
#include "helper/Scheduler.h"

#include <memory>



struct RtspContext
{
	ThreadPool* thread_pool_;
	Scheduler* scheduler_;

};