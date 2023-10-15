#include <iostream>
#include "helper/ThreadPool.h"
#include "live/RtspServer.h"
#include "helper/RtspContext.h"
#include "live/MediaSession.h"
#include "live/H264MediaSource.h"
#include "live/H264MediaSink.h"
#include <string_view>

int main()
{

	ThreadPool threadPool(4);
	
	Scheduler scheduler;
	RtspContext ctx;
	ctx.thread_pool_ = &threadPool;
	ctx.scheduler_ = &scheduler;

	MediaSession media_session("test");
	MediaSource* source = new H264MediaSource(&ctx,ROOT_DIR "/data/test.h264");
	std::unique_ptr<MediaSource> ptr;
	ptr.reset(source);
	Sink* sink = new H264MediaSink(&ctx,std::move(ptr) );
	std::unique_ptr<Sink> ptr_sink(sink);
	media_session.AddSink(TrackId0, std::move(ptr_sink));

	
	
	RtspServer server(&ctx, &media_session);
	
	server.Start();

	return 0;
}
