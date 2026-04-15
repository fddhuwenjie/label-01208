-- Data validator extension example
-- Demonstrates input validation patterns

Validator = {}

function Validator.is_string(value)
    return type(value) == "string"
end

function Validator.is_number(value)
    return type(value) == "number"
end

function Validator.is_positive(value)
    return type(value) == "number" and value > 0
end

function Validator.in_range(value, min, max)
    return type(value) == "number" and value >= min and value <= max
end

function Validator.not_empty(value)
    if type(value) == "string" then
        return #value > 0
    elseif type(value) == "table" then
        return next(value) ~= nil
    end
    return value ~= nil
end

function Validator.matches_pattern(value, pattern)
    if type(value) ~= "string" then return false end
    return string.match(value, pattern) ~= nil
end

function Validator.validate(data, rules)
    local errors = {}
    
    for field, rule_list in pairs(rules) do
        local value = data[field]
        
        for _, rule in ipairs(rule_list) do
            local valid, message = rule(value, field)
            if not valid then
                table.insert(errors, message)
            end
        end
    end
    
    return #errors == 0, errors
end

-- Rule builders
function Validator.required()
    return function(value, field)
        if value == nil or value == "" then
            return false, field .. " is required"
        end
        return true
    end
end

function Validator.min_length(len)
    return function(value, field)
        if type(value) == "string" and #value < len then
            return false, field .. " must be at least " .. len .. " characters"
        end
        return true
    end
end

function Validator.max_value(max)
    return function(value, field)
        if type(value) == "number" and value > max then
            return false, field .. " must be at most " .. max
        end
        return true
    end
end

-- Demo function
function demo_validator()
    print("=== Data Validator Demo ===")
    
    -- Simple validations
    print("is_string('hello'): " .. tostring(Validator.is_string("hello")))
    print("is_positive(-5): " .. tostring(Validator.is_positive(-5)))
    print("in_range(50, 1, 100): " .. tostring(Validator.in_range(50, 1, 100)))
    
    -- Complex validation
    local user_data = {
        username = "ab",
        age = 150,
        email = nil
    }
    
    local rules = {
        username = { Validator.required(), Validator.min_length(3) },
        age = { Validator.required(), Validator.max_value(120) },
        email = { Validator.required() }
    }
    
    local valid, errors = Validator.validate(user_data, rules)
    
    print("\nValidating user data:")
    print("Valid: " .. tostring(valid))
    if not valid then
        print("Errors:")
        for _, err in ipairs(errors) do
            print("  - " .. err)
        end
    end
end
