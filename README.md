# rbxsdk

A usermode c++ sdk for Roblox-related memory operations

## Features

Memory reading through standard Windows apis ( will be replaced with syscalls in the future ).

Easy to use libraries.

##  Known Issues
Sometimes while reading a string, rbxsdk may return unicode characters( i replaced the case of returning unicode with a error message ).
I am working on fixing the issue.


## Quick Start

```cpp
#include "include/rbx/sdk.h"

int main() {
    rbx::proc.attach();
    
    auto dm = rbx::datamodel::get();
    auto players = dm.players();
    auto local = rbx::player::local();
    auto cam = rbx::camera::get();
    
    printf("local: %s (hp: %.0f)\n", 
        local.display_name().c_str(),
        local.character().find_first_child_of_class("Humanoid")
            .get<float>(Offsets::Humanoid::Health));
    
    rbx::vector2 screen;
    if (cam.world_to_screen(local.position(), screen, {1920, 1080})) {
        printf("screen pos: %.0f, %.0f\n", screen.x, screen.y);
    }
    
    rbx::proc.detach();
}

