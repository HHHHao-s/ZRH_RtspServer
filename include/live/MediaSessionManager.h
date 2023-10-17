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

	std::shared_ptr<MediaSession> LookMediaSession( const std::string& session_name)const {
		if (media_sessions_.count(session_name) == 0) {
			return nullptr;
		}
		
		return media_sessions_.at(session_name);
		

	}

private:

	std::unordered_map<std::string_view, std::shared_ptr<MediaSession>> media_sessions_;

};

