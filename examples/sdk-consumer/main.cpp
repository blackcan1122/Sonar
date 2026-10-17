#include "Base/Object.hpp"
#include "Base/EventDispatcher.hpp"
#include "Base/Logging.h"
#include "Base/GameModeState.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Events/UIEvent.h"
#include <stdexcept>
#include <source_location>
#include <iostream>

class ConsumerObject : public Object {
    AUTOBODY(ConsumerObject, Object)
public:
    static inline bool destroyed = false;
    ~ConsumerObject() override { destroyed = true; }
};
static void require(bool value, std::source_location location = std::source_location::current()) {
    if (!value) throw std::runtime_error("SDK contract failed at line " + std::to_string(location.line()));
}
class ConsumerState : public GameModeState {};
int main() try {
    ConsumerState state;
    require(KeyEvent::StaticClass() != MouseEvent::StaticClass());
    auto object = std::make_shared<ConsumerObject>();
    auto objectClass = object->GetStaticClass();
    require(objectClass == SClass::FindClass("ConsumerObject"));
    require(*object->GetStaticClass() << Object::StaticClass());
    std::shared_ptr<IObject> base = object;
    require(std::dynamic_pointer_cast<ConsumerObject>(base) != nullptr);
    bool received = false;
    EventDispatcher dispatcher;
    dispatcher.AddListener("consumer", UIEvent::StaticClass(), [&received](std::shared_ptr<IEvent>) { received = true; });
    dispatcher.Dispatch(std::make_shared<UIEvent>());
    require(received);
    base.reset(); object.reset(); require(ConsumerObject::destroyed);
    InitLogger(); LOG_INFO(SONAR_FMT("SDK consumer: {}", "passed"));
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
