-- String utilities extension example
-- Demonstrates how to create reusable utility functions

StringUtils = {}

function StringUtils.reverse(str)
    return string.reverse(str)
end

function StringUtils.split(str, delimiter)
    local result = {}
    for match in (str .. delimiter):gmatch("(.-)" .. delimiter) do
        table.insert(result, match)
    end
    return result
end

function StringUtils.trim(str)
    return str:match("^%s*(.-)%s*$")
end

function StringUtils.capitalize(str)
    return str:sub(1, 1):upper() .. str:sub(2):lower()
end

-- Demo function
function demo_string_utils()
    print("=== String Utils Demo ===")
    
    local text = "  hello world  "
    print("Original: '" .. text .. "'")
    print("Trimmed: '" .. StringUtils.trim(text) .. "'")
    
    local reversed = StringUtils.reverse("Lua")
    print("Reverse 'Lua': " .. reversed)
    
    local parts = StringUtils.split("a,b,c,d", ",")
    print("Split 'a,b,c,d': " .. table.concat(parts, " | "))
    
    print("Capitalize 'hELLO': " .. StringUtils.capitalize("hELLO"))
end
