-- Task scheduler extension example
-- Demonstrates delayed task execution using native functions

TaskScheduler = {
    tasks = {},
    task_id = 0
}

function TaskScheduler.schedule(delay_ms, callback, name)
    TaskScheduler.task_id = TaskScheduler.task_id + 1
    local task = {
        id = TaskScheduler.task_id,
        delay = delay_ms,
        callback = callback,
        name = name or ("task_" .. TaskScheduler.task_id)
    }
    table.insert(TaskScheduler.tasks, task)
    return task.id
end

function TaskScheduler.run()
    print("Running " .. #TaskScheduler.tasks .. " scheduled tasks...")
    
    for _, task in ipairs(TaskScheduler.tasks) do
        print("Executing: " .. task.name .. " (delay: " .. task.delay .. "ms)")
        native_sleep(task.delay)
        task.callback()
    end
    
    TaskScheduler.tasks = {}
end

function TaskScheduler.clear()
    TaskScheduler.tasks = {}
end

-- Demo function
function demo_scheduler()
    print("=== Task Scheduler Demo ===")
    
    local start_time = native_timestamp()
    
    TaskScheduler.schedule(100, function()
        print("Task 1 executed!")
    end, "quick_task")
    
    TaskScheduler.schedule(200, function()
        local result = native_add(5, 10)
        print("Task 2: 5 + 10 = " .. result)
    end, "calculation_task")
    
    TaskScheduler.schedule(50, function()
        native_print("Task 3 via native print")
    end, "native_task")
    
    TaskScheduler.run()
    
    local elapsed = native_timestamp() - start_time
    print("Total execution time: " .. elapsed .. "ms")
end
