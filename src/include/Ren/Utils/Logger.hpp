/**
 * Logger implementation designed to be portable and a copy-paste solution for other projects.
 * Use:
 *  - Make sure you have added LogListener
 *      - Example: Ren::LogEmmiter::AddListener<Ren::StreamLogger>(StreamArr{ stdout });
 *  - Then you can use macros LOG_* for logging or Ren::LogEmmiter::Log() member function.
 * If you need extra functionality, you can define your own child of LogListener base class (see definition below).
 */

#pragma once
#include <string>
#include <filesystem>
#include <thread>
#include <ctime>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>

#define LOG_I(message) Ren::LogEmmiter::Log(Ren::LogLevel::info, message, __FILE__, __LINE__)
#define LOG_S(message) Ren::LogEmmiter::Log(Ren::LogLevel::status, message, __FILE__, __LINE__)
#define LOG_W(message) Ren::LogEmmiter::Log(Ren::LogLevel::warning, message, __FILE__, __LINE__)
#define LOG_E(message) Ren::LogEmmiter::Log(Ren::LogLevel::error, message, __FILE__, __LINE__)
#define LOG_C(message) Ren::LogEmmiter::Log(Ren::LogLevel::critical, message, __FILE__, __LINE__)

using StreamArr = std::vector<std::FILE*>;  // Used in Ren::StreamLogger

namespace Ren {
    namespace Utils {
        struct TimeInfo {
            int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
            std::tm tim{};

            TimeInfo(std::time_t t = std::time(nullptr)) {
                tim = *localtime(&t);
                year = tim.tm_year + 1900;
                month = tim.tm_mon + 1;
                day = tim.tm_mday;
                hour = tim.tm_hour;
                minute = tim.tm_min;
                second = tim.tm_sec;
            }
            std::string to_string() const {
                char str[21] = {};
                std::snprintf(str, 20, "%04i-%02i-%02i %02i:%02i:%02i", year, month, day, hour, minute, second);
                str[20] = '\0';
                return str;
            }
        };
    }

    // Error -> When something fails, but program can still go on.
    // Critical -> Program cannot continue with this problem.
    enum class LogLevel : int { info = 0, status = 1, warning = 2, error = 3, critical = 4 };
    static constexpr const char* LOG_LEVEL_STRINGS[] = { "Info", "Status", "Warning", "Error", "Critical" };

    // Holds data of single log entry.
    struct LogInfo {
        Utils::TimeInfo time_info{ Utils::TimeInfo() };
        LogLevel level{ LogLevel::info };
        std::thread::id thread{ std::this_thread::get_id() };
        std::filesystem::path file{ "" };
        uint32_t line{ 0 };
        std::string message{ "" };
    };

    // Base class for log listeners.
    class LogListener {
    public:
        virtual ~LogListener() {}
        virtual void OnLog(const LogInfo& log) = 0;
    };

    // Outputs logs to (C) file streams.
    class StreamLogger : public LogListener {
    public:
        StreamArr m_Streams{ };

        StreamLogger(StreamArr streams = { stdout }) {
            for (auto&& s : streams)
                m_Streams.push_back(s);
        }

        void OnLog(const LogInfo& log) override {
            for (std::FILE* stream : m_Streams)
                std::fprintf(stream, "%s    %12s    %15s:%-4i    %s\n",
                    log.time_info.to_string().c_str(),
                    LOG_LEVEL_STRINGS[(int)log.level],
                    log.file.filename().string().c_str(), log.line,
                    log.message.c_str()
            );
        };

        void Erase(std::FILE* file) {
            int i = 0;
            for (i = 0; i < (int)m_Streams.size(); i++)
                if (m_Streams[i] == file)
                    break;
            if (i < (int)m_Streams.size())
                m_Streams.erase(m_Streams.begin() + i);
        }
    };

    // Emits logs with given argument.
    class LogEmmiter {
    public:
        static void Log(LogLevel level, std::string message, std::filesystem::path file, uint32_t line) {
            LogInfo log;
            log.level = level;
            log.message = message;
            log.file = file;
            log.line = line;

            for (auto&& [id, listener] : s_logListeners)
                listener->OnLog(log);
        }

        // Create instance of a specified type (which must be a child of LogListener) with args as arguments for construction.
        template<typename T, typename... Args>
        static T* AddListener(Args... args) {
            int id = getID<T>();
            if (s_logListeners.count(id) != 0)
                return dynamic_cast<T*>(s_logListeners[id].get());

            s_logListeners[id] = std::shared_ptr<LogListener>(new T(std::forward<Args>(args)...));
            return dynamic_cast<T*>(s_logListeners[id].get());
        }
        // Create Instance of specified type (which must be a child of LogListener) using initializer list for construction.
        template<typename T, typename... Args, typename I>
        static T* AddListener(std::initializer_list<I> init_list) {
            int id = getID<T>();
            if (s_logListeners.count(id) != 0)
                return dynamic_cast<T*>(s_logListeners[id].get());

            s_logListeners[id] = std::shared_ptr<LogListener>(new T(init_list));
            return dynamic_cast<T*>(s_logListeners[id].get());
        }
        // Create instance of a specified type (which must be a child of LogListener).
        template<typename T>
        static T* AddListener() {
            int id = getID<T>();
            if (s_logListeners.count(id) != 0)
                return dynamic_cast<T*>(s_logListeners[id].get());

            s_logListeners[id] = std::shared_ptr<LogListener>(new T());
            return dynamic_cast<T*>(s_logListeners[id].get());
        }

        template<typename T>
        static T* GetListener() {
            int id = getID<T>();
            if (s_logListeners.count(id) != 0)
                return dynamic_cast<T*>(s_logListeners[id].get());
            else
                return nullptr;
        }

        template<typename T>
        static void RemoveListener() {
            s_logListeners.erase(getID<T>());
        }

    protected:
        inline static std::unordered_map<int, std::shared_ptr<LogListener>> s_logListeners{};
        inline static int s_lastID{ 0 };

        template<typename T>
        static int getID() {
            static int current_id = s_lastID++;
            return current_id;
        }
    };
}
