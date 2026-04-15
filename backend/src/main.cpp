#include "script_engine.h"
#include "native_functions.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>

// Simple logger with levels
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    static LogLevel level;
    
    static void log(LogLevel lvl, const std::string& msg) {
        if (lvl < level) return;
        
        const char* prefix[] = {"[DEBUG]", "[INFO]", "[WARN]", "[ERROR]"};
        auto now = std::time(nullptr);
        char timebuf[20];
        std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        auto& out = (lvl >= LogLevel::WARN) ? std::cerr : std::cout;
        out << timebuf << " " << prefix[static_cast<int>(lvl)] << " " << msg << std::endl;
    }
    
    static void debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
    static void info(const std::string& msg) { log(LogLevel::INFO, msg); }
    static void warn(const std::string& msg) { log(LogLevel::WARN, msg); }
    static void error(const std::string& msg) { log(LogLevel::ERROR, msg); }
};

LogLevel Logger::level = LogLevel::INFO;

struct Config {
    std::string scriptDir = "scripts";
    LogLevel logLevel = LogLevel::INFO;
    bool interactive = false;
};

Config loadConfig(const std::string& path) {
    Config cfg;
    std::ifstream file(path);
    if (!file.is_open()) return cfg;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (key == "script_dir") cfg.scriptDir = value;
        else if (key == "log_level") {
            if (value == "debug") cfg.logLevel = LogLevel::DEBUG;
            else if (value == "info") cfg.logLevel = LogLevel::INFO;
            else if (value == "warn") cfg.logLevel = LogLevel::WARN;
            else if (value == "error") cfg.logLevel = LogLevel::ERROR;
        }
        else if (key == "interactive") cfg.interactive = (value == "true");
    }
    return cfg;
}

void runInteractiveMode(ScriptEngine& engine) {
    Logger::info("Entering interactive mode. Type 'exit' to quit.");
    std::string line;
    
    while (true) {
        std::cout << "lua> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        
        if (!engine.executeString(line)) {
            Logger::error(engine.getLastError());
        }
    }
    Logger::info("Exiting interactive mode.");
}

int main(int argc, char* argv[]) {
    std::cout << "=== Lua Script Engine ===" << std::endl;
    
    // Load config
    Config cfg = loadConfig("engine.conf");
    
    // Command line overrides
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" || arg == "--interactive") {
            cfg.interactive = true;
        } else if (arg == "-d" || arg == "--debug") {
            cfg.logLevel = LogLevel::DEBUG;
        } else if (arg[0] != '-') {
            cfg.scriptDir = arg;
        }
    }
    
    Logger::level = cfg.logLevel;
    
    ScriptEngine engine;
    if (!engine.init()) {
        Logger::error("Failed to initialize engine: " + engine.getLastError());
        return 1;
    }
    
    registerNativeFunctions(engine);
    Logger::info("Native functions registered.");
    
    // Load scripts from directory
    if (!std::filesystem::exists(cfg.scriptDir)) {
        Logger::warn("Script directory not found: " + cfg.scriptDir);
        Logger::info("Create the directory and add .lua files, or specify path as argument.");
    } else {
        int loaded = 0;
        for (const auto& entry : std::filesystem::directory_iterator(cfg.scriptDir)) {
            if (entry.path().extension() == ".lua") {
                Logger::debug("Loading: " + entry.path().string());
                if (!engine.loadScript(entry.path().string())) {
                    Logger::error("Failed to load " + entry.path().string() + ": " + engine.getLastError());
                } else {
                    loaded++;
                }
            }
        }
        Logger::info("Loaded " + std::to_string(loaded) + " script(s) from " + cfg.scriptDir);
    }
    
    // Run main function if exists
    if (!engine.callFunction("main")) {
        Logger::debug("No main() function found or execution failed.");
    }
    
    // Interactive mode
    if (cfg.interactive) {
        runInteractiveMode(engine);
    }
    
    Logger::info("Engine finished.");
    return 0;
}
