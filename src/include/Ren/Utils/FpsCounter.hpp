/**
 * @file Ren/Utils/FpsCounter.hpp
 * @brief Declaration of FpsCounter object.
 */

#pragma once
#include <glm/glm.hpp>

#include "imgui.h"
#include "ren_utils/RingBuffer.hpp"

namespace Ren::Utils {
    class FpsCounter {
        const unsigned NUM_OF_SAMPLES = 288;
        using ValueArr = ren_utils::RingBuffer<float>;
    public:

        FpsCounter() : m_values(NUM_OF_SAMPLES) {}

        void Update(float dt) {
            m_currentSampleTime += dt;
            m_nSamples++;
            m_values.PushBack(1.0f / dt);

            if (m_currentSampleTime >= m_sampleTime) {
                m_fps = m_nSamples / m_currentSampleTime;
                reset();
            }
        }
        inline float GetFps() { return m_fps; }
        inline void SetSampleTime(float n_sec) { m_sampleTime = n_sec; reset(); }

        /// Draw ImGui window with plot inside it.
        void DrawPlotWindow(glm::ivec2 win_size) {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            if (m_lockPos)
                ImGui::SetNextWindowPos(ImVec2(10.0f, win_size.y - 105.0f));
            if (ImGui::Begin("Ren::Utils::FpsCounter", nullptr, window_flags))
                DrawPlot();
            ImGui::End();
        }

        /// Draw only ImGui plot.
        void DrawPlot() {
            struct Funcs {
                static float value_getter(void* p_arr, int i) {
                    ValueArr* arr = (ValueArr*)p_arr;
                    return arr->at(i);
                }
            };

            //ImGui::Text("FPS: %.1f", m_fps);
            char avg_fps[20];
            std::sprintf(avg_fps, "avg. %.1f", m_fps);
            ImGui::PlotLines("FPS", &Funcs::value_getter, &m_values, m_values.Size(), 0, avg_fps, 0.0f, 200.0f, ImVec2(0.0f, 80.0f));

            if (ImGui::BeginPopupContextWindow()) {
                ImGui::MenuItem("Lock position", NULL, &m_lockPos);
                ImGui::EndPopup();
            }
        }

    private:
        ValueArr m_values;
        float m_sampleTime = 2.0f;
        float m_currentSampleTime = 0.0f;
        int m_nSamples = 0;
        bool m_lockPos{ true };

        float m_fps{ 0.0f };

        void reset() {
            m_currentSampleTime = 0.0f;
            m_nSamples = 0;
        }

    };
} // namespace Ren::Utils
