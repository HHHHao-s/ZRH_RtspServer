#pragma once
#include "live/MediaSession.h"
#include <memory>
#include <unordered_map>
#include <string_view>

class MediaSessionManager
{
public:
	MediaSessionManager();
	~MediaSessionManager();

	void AddMediaSession(std::unique_ptr<MediaSession> media_session) {
		media_sessions_.emplace(media_session->GetSessionName(), std::move(media_session));
	}

	MediaSession* LookMediaSession( const std::string& session_name)const {
		auto it = media_sessions_.find(session_name);
		if (it == media_sessions_.end()) {
			return nullptr;
		}
		return it->second.get();
	}

private:

	std::unordered_map<std::string_view, std::unique_ptr<MediaSession>> media_sessions_;

};

