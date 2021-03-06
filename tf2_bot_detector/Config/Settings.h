#pragma once

#include "Networking/HTTPClient.h"
#include "ChatWrappers.h"
#include "Clock.h"
#include "ReleaseChannel.h"
#include "SteamID.h"

#include <nlohmann/json_fwd.hpp>

#include <filesystem>
#include <optional>
#include <vector>

namespace srcon
{
	class async_client;
}

namespace tf2_bot_detector
{
	void to_json(nlohmann::json& j, const ReleaseChannel& d);
	void from_json(const nlohmann::json& j, ReleaseChannel& d);

	struct AutoDetectedSettings
	{
		SteamID GetLocalSteamID() const;
		SteamID m_LocalSteamIDOverride;

		std::filesystem::path GetSteamDir() const;
		std::filesystem::path m_SteamDirOverride;

		std::filesystem::path GetTFDir() const;
		std::filesystem::path m_TFDirOverride;
	};

	struct GotoProfileSite
	{
		std::string m_Name;
		std::string m_ProfileURL;

		std::string CreateProfileURL(const SteamID& id) const;
	};

	struct GeneralSettings
	{
		bool m_AutoChatWarnings = true;
		bool m_AutoChatWarningsConnecting = false;
		bool m_AutoVotekick = true;
		float m_AutoVotekickDelay = 15;
		bool m_AutoMark = true;

		bool m_SleepWhenUnfocused = true;
		bool m_AutoTempMute = true;
		bool m_AutoLaunchTF2 = false;

		bool m_LazyLoadAPIData = true;

		std::optional<ReleaseChannel> m_ReleaseChannel;

		constexpr auto GetAutoVotekickDelay() const { return std::chrono::duration<float>(m_AutoVotekickDelay); }

		const std::string& GetSteamAPIKey() const { return m_SteamAPIKey; }
		void SetSteamAPIKey(std::string key);

	private:
		std::string m_SteamAPIKey;
	};

	class Settings final : public AutoDetectedSettings, public GeneralSettings
	{
	public:
		Settings();

		void LoadFile();
		bool SaveFile() const;

		// Settings that are not saved in the config file because we want them to
		// reset to these defaults when the tool is reopened
		struct Unsaved
		{
			~Unsaved();

			bool m_DebugShowCommands = false;

			uint32_t m_ChatMsgWrappersToken{};
			std::optional<ChatWrappers> m_ChatMsgWrappers;
			std::unique_ptr<srcon::async_client> m_RCONClient;

		} m_Unsaved;

		std::optional<bool> m_AllowInternetUsage;
		const HTTPClient* GetHTTPClient() const;

		std::vector<GotoProfileSite> m_GotoProfileSites;

		struct Logging
		{
			bool m_RCONPackets = false;
			bool m_DiscordRichPresence = false;

		} m_Logging;

		struct Discord
		{
			bool m_EnableRichPresence = true;

		} m_Discord;

		struct Theme
		{
			struct Colors
			{
				std::array<float, 4> m_ScoreboardCheaterBG = { 1, 0, 1, 1 };
				std::array<float, 4> m_ScoreboardSuspiciousBG = { 1, 1, 0, 1 };
				std::array<float, 4> m_ScoreboardExploiterBG = { 0, 1, 1, 1 };
				std::array<float, 4> m_ScoreboardRacistBG = { 1, 1, 1, 1 };
				std::array<float, 4> m_ScoreboardYouFG = { 0, 1, 0, 1 };
				std::array<float, 4> m_ScoreboardConnectingFG = { 1, 1, 0, 0.5f };

				std::array<float, 4> m_ScoreboardFriendlyTeamBG = { 0.19704340398311615f, 0.5180000066757202f, 0.25745877623558044f, 0.5f };
				std::array<float, 4> m_ScoreboardEnemyTeamBG = { 0.8270000219345093f, 0.42039787769317627f, 0.38951700925827026f, 0.5f };

				std::array<float, 4> m_ChatLogYouFG = { 0, 1, 0, 1 };
				std::array<float, 4> m_ChatLogFriendlyTeamFG = { 0.7f, 1, 0.7f, 1 };
				std::array<float, 4> m_ChatLogEnemyTeamFG = { 1, 0.535f, 0.5f, 1 };
			} m_Colors;

		} m_Theme;

	private:
		mutable std::shared_ptr<HTTPClient> m_HTTPClient;
	};
}
