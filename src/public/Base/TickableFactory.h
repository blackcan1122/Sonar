#pragma once
#include "Base/Core.h"

class Tickable;

/*
* 
* Class for handling Object Registry which should be called from a Class
* 
*/
class TickableFactory {
public:

    // Using Weak_ptr since adding them to this Array doesnt affect counting of objects
    static std::vector<Tickable*> GlobalTickables;

    static void CleanUpTickables()
    {
        // Entferne alle Einträge, die nullptr sind
        auto it = std::remove(GlobalTickables.begin(), GlobalTickables.end(), nullptr);
        GlobalTickables.erase(it, GlobalTickables.end());
    }

    static void Unregister(Tickable* Tickable)
    {
        auto it = std::find(GlobalTickables.begin(), GlobalTickables.end(), Tickable);

        if (it == GlobalTickables.end())
        {
            std::cout << "There were issues Removing Object from Container" << std::endl;
            return;
        }
        
        GlobalTickables.erase(it);
        std::cout << "Sucessfully Deleted" << std::endl;
    }

    // Create a static function to register tickable objects
    static void Register(Tickable* tickable) {
        GlobalTickables.push_back(tickable);
    }

    // Get a reference to the container of tickable objects
    static std::vector<Tickable*> GetTickables() {
        return GlobalTickables;
    }

};
