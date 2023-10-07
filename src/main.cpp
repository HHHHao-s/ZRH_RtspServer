#include <iostream>
#include "helper/ThreadPool.h"
#include "live/RtspServer.h"
#include "helper/RtspContext.h"



int main()
{

	std::shared_ptr<ThreadPool> threadPool = std::make_shared<ThreadPool>(4);
	std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>();
	std::shared_ptr<RtspContext> ctx = std::make_shared<RtspContext>();
	ctx->thread_pool_ = threadPool;
	ctx->scheduler_ = scheduler;
	
	RtspServer server(ctx);

	server.Start();

	return 0;
}
