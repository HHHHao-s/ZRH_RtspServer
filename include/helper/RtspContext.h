#pragma once
#include "helper/ThreadPool.h"
#include <memory>


struct RtspContext
{
	std::shared_ptr<ThreadPool> thread_pool_;
	
};