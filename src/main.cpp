#include <iostream>
#include "helper/ThreadPool.h"
#include "live/RtspServer.h"
#include "helper/RtspContext.h"
#include "live/MediaSession.h"
#include "live/H264MediaSource.h"
#include "live/H264MediaSink.h"
#include <string_view>
#include "live/MediaSessionManager.h"

int main()
{

	ThreadPool threadPool(4);
	
	Scheduler scheduler;
	RtspContext ctx;
	ctx.thread_pool_ = &threadPool;
	ctx.scheduler_ = &scheduler;


	std::unique_ptr<MediaSession> ptr_media_session= std::make_unique<MediaSession>("test");
	MediaSource* source = new H264MediaSource(&ctx,ROOT_DIR "/data/test.h264");
	std::unique_ptr<MediaSource> ptr;
	ptr.reset(source);
	Sink* sink = new H264MediaSink(&ctx,std::move(ptr) );
	std::unique_ptr<Sink> ptr_sink(sink);
	ptr_media_session->AddSink(TrackId0, std::move(ptr_sink));

	std::unique_ptr<MediaSessionManager>  media_session_manager = std::make_unique<MediaSessionManager>();
	media_session_manager->AddMediaSession(std::move(ptr_media_session));

	
	RtspServer server(&ctx, std::move(media_session_manager));
	
	server.Start();

	return 0;
}
