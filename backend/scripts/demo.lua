-- Demo Lua script showing engine capabilities

function main()
    print("Hello from Lua!")
    
    -- Use native C++ functions
    native_print("This message is from native C++ function")
    
    local result = native_add(10, 20)
    print("10 + 20 = " .. result)
    
    local ts = native_timestamp()
    print("Current timestamp: " .. ts)
    
    -- Custom logic
    greet("World")
    calculate_sum(100)
end

function greet(name)
    print("Greetings, " .. name .. "!")
end

function calculate_sum(n)
    local sum = 0
    for i = 1, n do
        sum = sum + i
    end
    print("Sum of 1 to " .. n .. " = " .. sum)
end
