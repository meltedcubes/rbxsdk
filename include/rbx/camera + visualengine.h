#pragma once
#include "core.h"
#include "offsets.h"
#include "vector.h"

namespace rbx {

    class camera {
        uintptr_t self;
    public:
        camera(uintptr_t addr = 0) : self(addr) {}
        bool present() const { return self && proc.valid_ptr(self); }
        uintptr_t address() const { return self; }
        static camera get() {
            auto dm = datamodel::get();
            auto ws = dm.workspace();
            return camera(ws.find_first_child_of_class("Camera").address());
        }

        matrix4x4 view_matrix() {
            return proc.read<matrix4x4>(self + 0x130);
        }

        float fov() { return proc.read<float>(self + Offsets::Camera::FieldOfView); }
        vector3 position() { return proc.read<vector3>(self + Offsets::Camera::Position); }

        bool world_to_screen(const vector3& world, vector2& screen, const vector2& display) {
            auto vm = view_matrix();
            float* m = (float*)&vm;

            float clip_x = world.x * m[0] + world.y * m[1] + world.z * m[2] + m[3];
            float clip_y = world.x * m[4] + world.y * m[5] + world.z * m[6] + m[7];
            float clip_z = world.x * m[8] + world.y * m[9] + world.z * m[10] + m[11];
            float clip_w = world.x * m[12] + world.y * m[13] + world.z * m[14] + m[15];

            if (clip_w < 0.1f) return false;

            float inv = 1.f / clip_w;
            float ndc_x = clip_x * inv;
            float ndc_y = clip_y * inv;

            screen.x = (display.x / 2.f * ndc_x) + (ndc_x + display.x / 2.f);
            screen.y = -(display.y / 2.f * ndc_y) + (ndc_y + display.y / 2.f);

            return true;
        }     };

} // rbx