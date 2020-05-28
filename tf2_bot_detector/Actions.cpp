#include "Actions.h"

#include <mh/text/string_insertion.hpp>
#include <mh/text/stringops.hpp>

#include <iomanip>
#include <ostream>

using namespace tf2_bot_detector;
using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace std::string_view_literals;

KickAction::KickAction(uint16_t userID, KickReason reason) :
	GenericCommandAction("callvote", MakeArgs(userID, reason))
{
}

duration_t KickAction::GetMinInterval() const
{
	return 5s;
}

std::string KickAction::MakeArgs(uint16_t userID, KickReason reason)
{
	const char* reasonString = [&]()
	{
		switch (reason)
		{
		case KickReason::Other:     return "other";
		case KickReason::Cheating:  return "cheating";
		case KickReason::Idle:      return "idle";
		case KickReason::Scamming:  return "scamming";
		default: throw std::runtime_error("Unknown/invalid reason");
		}
	}();

	return "kick \""s << userID << ' ' << reasonString << '"';
}

GenericCommandAction::GenericCommandAction(std::string cmd, std::string args) :
	m_Command(std::move(cmd)), m_Args(std::move(args))
{
}

void GenericCommandAction::WriteCommands(ICommandWriter& writer) const
{
#if 0
	if (m_Command == "tf_lobby_debug"sv)
	{
		os << m_Command << '\n';
		return;
	}

	auto cleaned = mh::find_and_replace(m_Command, "\"", "'");
	os << "echo "s << std::quoted("[TFBD_DEBUG_CMD] "s << cleaned) << '\n';
#else
	writer.Write(m_Command, m_Args);
#endif
}

ChatMessageAction::ChatMessageAction(const std::string_view& message, ChatMessageType type) :
	GenericCommandAction(std::string(GetCommand(type)), ScrubMessage(message))
{
}

duration_t ChatMessageAction::GetMinInterval() const
{
	return 1s; // Actual cooldown is 0.66 seconds
}

std::string_view ChatMessageAction::GetCommand(ChatMessageType type)
{
	switch (type)
	{
	case ChatMessageType::Public:  return "say"sv;
	case ChatMessageType::Team:    return "say_team"sv;
	case ChatMessageType::Party:   return "say_party"sv;

	default:
		throw std::invalid_argument("Unhandled/invalid ChatMessageType");
	}
}

std::string ChatMessageAction::ScrubMessage(const std::string_view& msg)
{
	return "\""s << mh::find_and_replace(msg, "\""sv, "'"sv) << '"';
}

duration_t LobbyUpdateAction::GetMinInterval() const
{
	return 1s;
}

void LobbyUpdateAction::WriteCommands(ICommandWriter& writer) const
{
	writer.Write("tf_lobby_debug");
}
