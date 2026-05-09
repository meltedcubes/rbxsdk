#include <iostream>
#include <Windows.h>
#include "include/rbx/sdk.h"

int main() {
    if (!rbx::proc.attach()) {
        printf("roblox not running\n");
        return 1;
    }
    printf("attached (pid: %d)\n", rbx::proc.process_id());
    printf("base: 0x%p\n", (void*)rbx::proc.base());
    printf("size: %d bytes\n", (int)rbx::proc.size());

    auto dm = rbx::datamodel::get();
    if (!dm.present()) {
        printf("datamodel not found\n");
        return 1;
    }
    printf("datamodel: %s\n", dm.get_name().c_str());

    auto ws = dm.workspace();
    if (ws.present()) {
        printf("workspace: %s\n", ws.get_name().c_str());
        auto ws_children = ws.children();
        printf("    children: %zu\n", ws_children.size());
        for (size_t i = 0; i < std::min(ws_children.size(), (size_t)5); i++) {
            printf("      [%zu] %s (%s)\n", i,
                ws_children[i].get_name().c_str(),
                ws_children[i].get_class_name().c_str());
        }
    }
    auto players = dm.players();
    if (players.present()) {
        printf("players: %s\n", players.get_name().c_str());
        auto player_list = players.children();
        printf("    players in game: %zu\n", player_list.size());
        for (auto& p : player_list) {
            rbx::player plr(p.address());
            printf("      %s (id: %llu)\n", plr.display_name().c_str(), plr.user_id());
        }
    }
    auto local = rbx::player::local();
    if (local.present()) {
        printf("localplayer\n");
        auto name_ptr = local.get_ptr(Offsets::Player::DisplayName);
        printf("    displayname ptr: 0x%p\n", (void*)name_ptr);
        if (name_ptr) {
            auto len = rbx::proc.read<int32_t>(name_ptr + Offsets::Misc::StringLength);
            if (len > 0 && len < 255) {
                uintptr_t data = (len >= 16) ? rbx::proc.read_ptr(name_ptr) : name_ptr;
                char buf[256] = {0};
                SIZE_T r;
                ReadProcessMemory(rbx::proc.native(), (void*)data, buf, len, &r);
                printf("    displayname: \"%s\"\n", buf);
            }
        }

        printf("    name: %s\n", local.display_name().c_str());
        printf("    id: %llu\n", local.user_id());
        printf("    team color: %d\n", local.team_color());

        auto char_inst = local.character();
        if (char_inst.present()) {
            printf("    character: %s\n", char_inst.get_name().c_str());

            auto humanoid = char_inst.find_first_child_of_class("Humanoid");
            if (humanoid.present()) {
                float hp = humanoid.get<float>(Offsets::Humanoid::Health);
                float max_hp = humanoid.get<float>(Offsets::Humanoid::MaxHealth);
                float walkspeed = humanoid.get<float>(Offsets::Humanoid::Walkspeed);
                float jumppower = humanoid.get<float>(Offsets::Humanoid::JumpPower);
                float hipheight = humanoid.get<float>(Offsets::Humanoid::HipHeight);
                int rigtype = humanoid.get<int>(Offsets::Humanoid::RigType);
                int state = humanoid.get<int>(Offsets::Humanoid::HumanoidState);
                bool sit = humanoid.get<bool>(Offsets::Humanoid::Sit);

                printf("      hp: %.0f / %.0f\n", hp, max_hp);
                printf("      walkspeed: %.0f  jumppower: %.0f  hipheight: %.1f\n", walkspeed, jumppower, hipheight);
                printf("      rigtype: %d  state: %d  sitting: %d\n", rigtype, state, sit);
            }

            auto root = char_inst.find_first_child("HumanoidRootPart");
            if (root.present()) {
                auto prim = root.get_ptr(Offsets::BasePart::Primitive);
                if (prim) {
                    auto pos = rbx::proc.read<rbx::vector3>(prim + Offsets::Primitive::Position);
                    auto size = rbx::proc.read<rbx::vector3>(prim + Offsets::Primitive::Size);
                    printf("      position: %.1f, %.1f, %.1f\n", pos.x, pos.y, pos.z);
                    printf("      size: %.1f, %.1f, %.1f\n", size.x, size.y, size.z);
                }
            }
        }
    }
    auto cam = rbx::camera::get();
    if (cam.present()) {
        printf("camera\n");
        auto vm2 = rbx::proc.read<rbx::matrix4x4>(cam.address() + 0x130);
        printf("    viewmatrix:\n"); // debug
        for (int i = 0; i < 4; i++) {
            printf("      [%.3f, %.3f, %.3f, %.3f]\n", vm2[i][0], vm2[i][1], vm2[i][2], vm2[i][3]);
        }
    }
    auto lighting = dm.lighting();
    if (lighting.present()) {
        printf("[+] Lighting\n");
        float brightness = lighting.get<float>(Offsets::Lighting::Brightness);
        printf("    brightness: %.2f", brightness);
    }
    auto players_list = dm.players();
    if (players_list.present()) {
        printf("\nplayer positions on screen:\n");
        auto cam = rbx::camera::get();
        rbx::vector2 display = {1920, 1080};

        for (auto& p : players_list.children()) {
            rbx::player plr(p.address());
            if (plr.address() == local.address()) continue;

            auto pos = plr.position();
            rbx::vector2 screen;

            if (cam.world_to_screen(pos, screen, display)) {
                printf("    %s -> (%.0f, %.0f)\n",
                    plr.display_name().c_str(), screen.x, screen.y);
            } else {
                printf("    %s -> behind camera\n", plr.display_name().c_str());
            }
        }
    }
    rbx::proc.detach();
    system("pause");
    return 0;
}