#pragma once
#include <array>
#include <chrono>
#include <random>
#include <glm/glm.hpp>

#include "Ren/RenSDL/Context.hpp"

namespace Ren::Utils
{
    template<typename T, std::size_t MAX_ENTRIES>
    class CyclingArray
    {
        T mEntries[MAX_ENTRIES];
        uint32_t mStart = 0;
        uint32_t mEnd = 0;
    public:
        inline size_t size() { return mEnd - mStart; }
        void push_back(const T& entry) 
        {
            if (mEnd >= MAX_ENTRIES)
                mStart++;

            mEntries[mEnd % MAX_ENTRIES] = entry;
            mEnd++;

            if (mStart >= MAX_ENTRIES)
            {
                mStart = 0;
                mEnd = MAX_ENTRIES;
            }
        }
        inline void clear() { mStart = mEnd = 0; }
        inline T& operator[] (const int& index) { return mEntries[(mStart + index) % MAX_ENTRIES]; }
        inline T& at(const int& index) { return (*this)[index]; }

        class iterator
        {
            uint32_t index = 0;
            CyclingArray* pArr = nullptr;
        public:
            iterator() = default;
            iterator(uint32_t index, CyclingArray* array) : index(index), pArr(array) {}

            bool operator==(const iterator& it) { return index == it.index; }
            bool operator!=(const iterator& it) { return index != it.index; }
            iterator& operator++() { index++; return *this; }
            iterator& operator--() { index--; return *this; }
            T& operator->() { return (*pArr)[index]; }
            T& operator*() { return (*pArr)[index]; }
        };

        iterator begin() { return {0, this}; }
        iterator end() { return {(uint32_t)size(), this}; }
    };

    class FpsCounter
    {
        using ValueArr = CyclingArray<float, 288>;
    public:

        void Update(float dt)
        {
            m_currentSampleTime += dt;
            m_nSamples++;
            m_values.push_back(1.0f / dt);
            
            if (m_currentSampleTime >= m_sampleTime)
            {;
                m_fps = m_nSamples / m_currentSampleTime;
                reset();
            }
        }
        inline float GetFps() { return m_fps; }
        inline void SetSampleTime(float n_sec) { m_sampleTime = n_sec; reset(); }

        void DrawPlot(Ren::ImGuiContext& context, glm::ivec2 win_size)
        {
            struct Funcs
            {
                static float value_getter(void* p_arr, int i)
                {
                    ValueArr* arr = (ValueArr*)p_arr;
                    return arr->at(i);
                }
            };

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            static bool lock_pos = true;
            if (lock_pos)
                ImGui::SetNextWindowPos(ImVec2(10.0f, win_size.y - 105.0f));
            if (ImGui::Begin("Ren::Utils::FpsCounter", nullptr, window_flags))
            {
                //ImGui::Text("FPS: %.1f", m_fps);
                char avg_fps[20];
                #ifdef _BUILD_WINDOWS
                    sprintf_s(avg_fps, "avg. %.1f", m_fps);
                #else
                    std::sprintf(avg_fps, "avg. %.1f", m_fps);
                #endif
                ImGui::PlotLines("FPS", &Funcs::value_getter, &m_values, (int)m_values.size(), 0, avg_fps, 0.0f, 200.0f, ImVec2(0.0f, 80.0f));
                
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::MenuItem("Lock position", NULL, &lock_pos);
                    ImGui::EndPopup();
                }
            }
            ImGui::End();
        }

    private:
        ValueArr m_values;
        float m_sampleTime = 2.0f;
        float m_currentSampleTime = 0.0f;
        int m_nSamples = 0;

        float m_fps{ 0.0f };

        void reset()
        {
            m_currentSampleTime = 0.0f;
            m_nSamples = 0;
        }

    };

	using timer = std::chrono::system_clock;
	using millisec = std::chrono::milliseconds;
	using seconds = std::chrono::seconds;
	using microsec = std::chrono::microseconds;
	using nanosec = std::chrono::nanoseconds;

	class Stopwatch
	{
	public:
		Stopwatch() : clock_start(), clock_end(), elapsed_time(timer::duration::zero()) {}

		void Start()
		{
			clock_start = timer::now();
		}
		void Restart()
		{
			elapsed_time = timer::duration::zero();
			Start();
		}
		void Stop()
		{
			clock_end = timer::now();
			elapsed_time = clock_end - clock_start;
		}
		void Clear()
		{
			elapsed_time = timer::duration::zero();
		}
		void Pause()
		{
			clock_end = timer::now();
			elapsed_time += clock_end - clock_start;
		}
		template<class units> long int ElapsedTime()
		{			
			return static_cast<long int>(std::chrono::duration_cast<units>(elapsed_time).count());
		}
		float ElapsedSeconds()
		{
			return float(ElapsedTime<nanosec>()) / 1e9f;
		}
		float ElapsedMilliseconds()
		{
			return float(ElapsedTime<nanosec>()) / 1e6f;
		}

	protected:

		timer::time_point clock_start;
		timer::time_point clock_end;
		timer::duration	elapsed_time;
	};

	// Returns random integer in interval <min, max>
	inline int RandomInt(int min, int max)
	{
		max = max + 1;
		return std::rand() % (max - min) + min;
	}
	inline float RandomFloat_0_1()
	{
		return (float)std::rand() / (float)RAND_MAX;
	}
	static std::random_device rd;
	static std::mt19937 gen(rd());
	inline float RandomFloat(float min, float max)
	{
		std::uniform_real_distribution<float> dis(min, max);
		return dis(gen);
	}

	inline glm::vec3 HexToRGB(uint32_t color)
	{
		return glm::vec3(
			float((color >> 16) & 0xff),
			float((color >> 8)  & 0xff),
			float((color >> 0)  & 0xff)
		);
	}
	inline glm::vec4 HexToRGBA(uint32_t color)
	{
		return glm::vec4(
			float((color >> 16) & 0xff),
			float((color >> 8)  & 0xff),
			float((color >> 0)  & 0xff),
			float((color >> 24) & 0xff)
		);
	}
} // namespace Ren::Utils
