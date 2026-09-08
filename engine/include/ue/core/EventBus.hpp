#pragma once

#include "ue/core/Types.hpp"

#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace ue::core {

/// Bus de eventos desacoplado (patrón Editor-Suscriptor).
/// Los módulos se comunican publicando eventos tipados sin conocerse (DIP).
class EventBus {
public:
    using Handler = std::function<void(const void*)>;

    EventBus() = default;

    template <typename TEvent>
    int subscribe(std::function<void(const TEvent&)> handler) {
        const std::string key = typeid(TEvent).name();
        const int id = ++nextId_;
        listeners_[key].push_back({ id, [handler](const void* payload) {
            handler(*static_cast<const TEvent*>(payload));
        }});
        return id;
    }

    template <typename TEvent, typename Callable>
    int subscribe(Callable&& callable) {
        return subscribe<TEvent>(std::function<void(const TEvent&)>(std::forward<Callable>(callable)));
    }

    template <typename TEvent>
    void publish(const TEvent& event) {
        const std::string key = typeid(TEvent).name();
        const auto it = listeners_.find(key);
        if (it == listeners_.end()) {
            return;
        }
        for (auto& entry : it->second) {
            entry.handler(&event);
        }
    }

    void unsubscribe(int subscriptionId) {
        for (auto& [key, entries] : listeners_) {
            auto removed = std::remove_if(entries.begin(), entries.end(),
                                          [subscriptionId](const Entry& e) {
                                              return e.id == subscriptionId;
                                          });
            if (removed != entries.end()) {
                entries.erase(removed, entries.end());
            }
        }
    }

private:
    struct Entry {
        int id = 0;
        Handler handler;
    };

    std::map<std::string, std::vector<Entry>> listeners_;
    int nextId_ = 0;
};

} // namespace ue::core