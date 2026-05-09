#pragma once
#include "core.h"
#include "offsets.h"
#include <vector>

namespace rbx {

class instance {
protected:
    uintptr_t self;
public:
    instance(uintptr_t addr = 0) : self(addr) {}

    uintptr_t address() const { return self; }
    bool present() const { return proc.valid_ptr(self); }

    std::string get_name() {
        auto name_ptr = proc.read_ptr(self + Offsets::Instance::Name);
        if (!name_ptr) return "";
        auto str_len = proc.read<int32_t>(name_ptr + Offsets::Misc::StringLength);
        if (str_len <= 0 || str_len > 255) return "";
        uintptr_t str_data = (str_len >= 16) ? proc.read_ptr(name_ptr) : name_ptr;
        char buf[256] = {0};
        ReadProcessMemory(proc.native(), (void*)str_data, buf, str_len, nullptr);
        return std::string(buf);
    }

    std::string get_class_name() {
        auto desc = proc.read_ptr(self + Offsets::Instance::ClassDescriptor);
        if (!desc) return "";
        auto name_ptr = proc.read_ptr(desc + Offsets::Instance::ClassName);
        if (!name_ptr) return "";
        auto str_len = proc.read<int32_t>(name_ptr + Offsets::Misc::StringLength);
        if (str_len <= 0 || str_len > 255) return "";
        uintptr_t str_data = (str_len >= 16) ? proc.read_ptr(name_ptr) : name_ptr;
        char buf[256] = {0};
        ReadProcessMemory(proc.native(), (void*)str_data, buf, str_len, nullptr);
        return std::string(buf);
    }

    std::vector<instance> children() {
        std::vector<instance> list;
        auto children_start = proc.read_ptr(self + Offsets::Instance::ChildrenStart);
        if (!children_start) return list;
        auto children_end = proc.read_ptr(children_start + Offsets::Instance::ChildrenEnd);
        if (!children_end) return list;
        for (auto i = proc.read_ptr(children_start); i != children_end; i += 0x10) {
            auto child = proc.read_ptr(i);
            if (child && proc.valid_ptr(child)) list.push_back(instance(child));
        }
        return list;
    }

    instance find_first_child(const std::string& name) {
        for (auto& c : children())
            if (c.get_name() == name) return c;
        return instance(0);
    }

    instance find_first_child_of_class(const std::string& name) {
        for (auto& c : children())
            if (c.get_class_name() == name) return c;
        return instance(0);
    }

    instance parent() { return instance(proc.read_ptr(self + Offsets::Instance::Parent)); }

    template<typename T> T get(uintptr_t off) const { return proc.read<T>(self + off); }
    uintptr_t get_ptr(uintptr_t off) const { return proc.read_ptr(self + off); }
};

} // rbx