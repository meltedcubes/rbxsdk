//
// Created by voltas on 5/9/2026.
//

#ifndef RBXSDK_DATAMODEL_H
#pragma once
#include "instance.h"

namespace rbx {

    class datamodel : public instance {
    public:
        datamodel(uintptr_t addr = 0) : instance(addr) {}

        static datamodel get() {
            uintptr_t fake_dm = proc.read_ptr(proc.base() + 0x749cbd8);
            if (fake_dm && proc.valid_ptr(fake_dm)) {
                uintptr_t real_dm = proc.read_ptr(fake_dm + 0x1d0);
                if (real_dm && proc.valid_ptr(real_dm)) return datamodel(real_dm);
            }
            return datamodel(0);
        }

        instance workspace() { return instance(get_ptr(Offsets::DataModel::Workspace)); }
        instance players() { return find_first_child("Players"); }
        instance lighting() { return find_first_child("Lighting"); }
    };

} // rbx
#define RBXSDK_DATAMODEL_H

#endif //RBXSDK_DATAMODEL_H