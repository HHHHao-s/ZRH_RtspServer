#include "live/MediaSession.h"

MediaSession::MediaSession(std::string_view session_name){
	session_name_ = session_name;


}

MediaSession::~MediaSession() {

}

void MediaSession::AddSink(TrackId track_id, std::unique_ptr<Sink> sink) {

	if (track_id == TrackId0) {
		tracks_[0].sink_ = std::move(sink);
		tracks_[0].track_id_= track_id;
		tracks_[0].alive_ = true;
		tracks_[0].sink_->setSessionCb(SessionCb, this, &tracks_[0]);
		return;
	}
	else {
		tracks_[1].sink_ = std::move(sink);
		tracks_[1].track_id_ = track_id;
		tracks_[1].alive_ = true;
		tracks_[1].sink_->setSessionCb(SessionCb, this, &tracks_[1]);
		return;
	}
	
}
void MediaSession::AddRtpConnection(TrackId track_id, RtpConnection* rtp_conn) {
	if (track_id == TrackId0) {
		tracks_[0].rtp_conns_.push_back(rtp_conn);
		return;
	}
	else {
		tracks_[1].rtp_conns_.push_back(rtp_conn);
		return;
	}

}

void MediaSession::SessionCb(void* arg1, void *track, void* packet, PacketType packetType) {
	MediaSession* session = (MediaSession*)arg1;
	session->handleSessionSend((Track*)track, (RtpPacket*)packet, packetType);

}
void MediaSession::handleSessionSend(Track* track, RtpPacket* packet, PacketType packetType) {
	
	if (!track->alive_) {
		return;
	}
	packet->rtpHeader.seq = htons(packet->rtpHeader.seq);
	packet->rtpHeader.timestamp = htonl(packet->rtpHeader.timestamp);
	packet->rtpHeader.ssrc = htonl(packet->rtpHeader.ssrc);
	for (auto& it : track->rtp_conns_) {
		if (it->SendFrame(packet)<0) {
			LOG_INFO("发送失败");
		}
	}

}

void MediaSession::RemoveRtpConnection(TrackId track_id, RtpConnection* rtp_conn) {
	if (track_id == TrackId0) {
		tracks_[0].rtp_conns_.remove(rtp_conn);
		return;
	}
	else {
		tracks_[1].rtp_conns_.remove(rtp_conn);
		return;
	}
}