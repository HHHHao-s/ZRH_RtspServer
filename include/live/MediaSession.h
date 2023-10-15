#pragma once
#include <string>
#include <stdint.h>
#include "live/Rtp.h"
#include "live/H264MediaSink.h"
#include "live/Sink.h"

#define MAX_MEDIA_TRACK 2
typedef uint32_t session_id_t;

enum TrackId
{
	TrackIdNone = -1,
	TrackId0 = 0,
	TrackId1 = 1,
};

struct Track {

	std::unique_ptr<Sink> sink_;
	TrackId track_id_;
	bool alive_;
	std::list<RtpConnection*> rtp_conns_;
};

class MediaSession
{
public:
	
	MediaSession(std::string_view session_name);
	~MediaSession();

	session_id_t GetSessionId() const { return session_id_; }

	void AddSink(TrackId track_id, std::unique_ptr<Sink> sink);
	void AddRtpConnection(TrackId track_id, RtpConnection* rtp_conn);
	void RemoveRtpConnection(TrackId track_id, RtpConnection* rtp_conn);
private:

	static void SessionCb(void* arg1, void *track, void* packet, PacketType packetType);
	void handleSessionSend(Track *track, RtpPacket* packet, PacketType packetType);


	

	Track tracks_[MAX_MEDIA_TRACK];

	session_id_t session_id_;
	std::string_view session_name_;

};