-- Configuration loader extension example
-- Demonstrates loading and managing application config

Config = {
    data = {},
    defaults = {
        app_name = "LuaEngine",
        version = "1.0.0",
        debug = false,
        max_connections = 100,
        timeout_ms = 5000
    }
}

function Config.load(config_table)
    Config.data = {}
    -- Apply defaults first
    for k, v in pairs(Config.defaults) do
        Config.data[k] = v
    end
    -- Override with provided config
    if config_table then
        for k, v in pairs(config_table) do
            Config.data[k] = v
        end
    end
end

function Config.get(key, default)
    local value = Config.data[key]
    if value == nil then
        return default
    end
    return value
end

function Config.set(key, value)
    Config.data[key] = value
end

function Config.dump()
    print("Current configuration:")
    for k, v in pairs(Config.data) do
        print("  " .. k .. " = " .. tostring(v))
    end
end

-- Demo function
function demo_config()
    print("=== Config Loader Demo ===")
    
    -- Load with custom overrides
    Config.load({
        debug = true,
        max_connections = 50,
        custom_setting = "my_value"
    })
    
    Config.dump()
    
    print("\nAccessing values:")
    print("app_name: " .. Config.get("app_name"))
    print("debug: " .. tostring(Config.get("debug")))
    print("unknown (with default): " .. Config.get("unknown", "fallback"))
end
