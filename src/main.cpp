#include <iostream>
#include "helper/ThreadPool.h"
#include "live/RtspServer.h"
#include "helper/RtspContext.h"



int main()
{

	ThreadPool threadPool(4);
	
	Scheduler scheduler;
	RtspContext ctx;
	ctx.thread_pool_ = &threadPool;
	ctx.scheduler_ = &scheduler;
	
	RtspServer server(&ctx);

	server.Start();

	return 0;
}
