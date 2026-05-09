#pragma once
#include "instance.h"

namespace rbx {

    class player : public instance {
    public:
        player(uintptr_t addr = 0) : instance(addr) {}

        static player local() {
            auto dm = datamodel::get();
            auto players = dm.find_first_child_of_class("Players");
            auto lp = players.get_ptr(Offsets::Player::LocalPlayer);
            return player(lp);
        }

        std::string display_name() {
            auto val = get_ptr(Offsets::Player::DisplayName);

            if (val > 0x10000 && val < 0x7FFFFFFF0000) {
                auto len = proc.read<int32_t>(val + Offsets::Misc::StringLength);
                if (len > 0 && len < 255) {
                    uintptr_t data = (len >= 16) ? proc.read_ptr(val) : val;
                    char buf[256] = {0};
                    ReadProcessMemory(proc.native(), (void*)data, buf, len, nullptr);

                    for (int i = 0; i < len; i++) {
                        if ((unsigned char)buf[i] > 127 || (unsigned char)buf[i] < 32) {
                            if (buf[i] != 0) return "";
                        }
                    }
                    return std::string(buf);
                }
            }

            int32_t len = proc.read<int32_t>(self + Offsets::Player::DisplayName + 0x10);
            if (len > 0 && len < 255) {
                char buf[256] = {0};
                ReadProcessMemory(proc.native(), (void*)(self + Offsets::Player::DisplayName), buf, len, nullptr);

                for (int i = 0; i < len; i++) {
                    if ((unsigned char)buf[i] > 127 || (unsigned char)buf[i] < 32) {
                        if (buf[i] != 0) return "the name contains unicode, indicating that it is invalid and rbxsdk's memory has failed to read it. this is a known bug.";
                    }
                }
                return std::string(buf);
            }

            return "";
        }

        uint64_t user_id() { return get<uint64_t>(Offsets::Player::UserId); }
        int team_color() { return get<int>(Offsets::Player::TeamColor); }

        instance character() { return instance(get_ptr(Offsets::Player::ModelInstance)); }

        vector3 position() {
            auto c = character();
            if (!c.present()) return {0,0,0};
            auto root = c.find_first_child("HumanoidRootPart");
            if (!root.present()) return {0,0,0};
            auto prim = root.get_ptr(Offsets::BasePart::Primitive);
            if (!prim) return {0,0,0};
            return rbx::proc.read<vector3>(prim + Offsets::Primitive::Position);
        }
    };

} // rbx