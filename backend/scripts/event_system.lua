-- Event system extension example
-- Demonstrates a simple pub/sub event pattern

EventSystem = {
    listeners = {}
}

function EventSystem.on(event_name, callback)
    if not EventSystem.listeners[event_name] then
        EventSystem.listeners[event_name] = {}
    end
    table.insert(EventSystem.listeners[event_name], callback)
end

function EventSystem.emit(event_name, ...)
    local callbacks = EventSystem.listeners[event_name]
    if callbacks then
        for _, callback in ipairs(callbacks) do
            callback(...)
        end
    end
end

function EventSystem.off(event_name)
    EventSystem.listeners[event_name] = nil
end

-- Demo function
function demo_event_system()
    print("=== Event System Demo ===")
    
    -- Register listeners
    EventSystem.on("user_login", function(username)
        print("User logged in: " .. username)
    end)
    
    EventSystem.on("user_login", function(username)
        native_print("Logging login event for: " .. username)
    end)
    
    EventSystem.on("data_received", function(data, source)
        print("Received '" .. data .. "' from " .. source)
    end)
    
    -- Emit events
    EventSystem.emit("user_login", "Alice")
    EventSystem.emit("data_received", "Hello", "Server")
    
    -- Cleanup
    EventSystem.off("user_login")
    print("Event listeners cleared")
end
