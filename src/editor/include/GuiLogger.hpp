#include <Ren/Ren.h>
#include <Ren/Utils/Data.hpp>
#include <Ren/Utils/Logger.hpp>
#define MAX_LOG_ENTRIES 1000

class GuiLogger : public Ren::LogListener
{
public:
    void OnLog(const Ren::LogInfo& log) override
    {
        if (m_pauseLogging)
            return;
        m_logs.push_back({ Ren::Utils::TimeInfo().to_string(), log });
        if (m_autoScroll)
            m_scrollBottom = true;
    }

    void Draw()
    {
        ImGui::Begin("Logger");
        ImVec2 a_size = ImGui::GetContentRegionMax();
        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Hideable;
        if (ImGui::BeginTable("table_log", 4, flags, ImVec2(0.0f, a_size.y - ImGui::GetTextLineHeight() * 2.0f))) {
            if (ImGui::BeginPopupContextWindow()) {
                ImGui::MenuItem("Autoscroll", "", &m_autoScroll);
                ImGui::MenuItem("Pause logging", "", &m_pauseLogging);
                ImGui::Separator();
                if (ImGui::Selectable("Clear logs"))
                    m_logs.clear();
                ImGui::EndPopup();
            }
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Timestamp");
            ImGui::TableSetupColumn("File");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Log");
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin((int)m_logs.size());
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    EntryWrapper& e = m_logs[i];

                    ImGui::TableNextRow();                    
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", e.timestamp.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%s:%i", e.entry.file.filename().string().c_str(), e.entry.line);

                    ImGui::TableNextColumn();
                    if (e.entry.level == Ren::LogLevel::warning)
                        ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "%s", Ren::LOG_LEVEL_STRINGS[(int)e.entry.level]);
                    else if (e.entry.level == Ren::LogLevel::error)
                        ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", Ren::LOG_LEVEL_STRINGS[(int)e.entry.level]);
                    else if (e.entry.level == Ren::LogLevel::info)
                        ImGui::TextColored({ 0.0f, 1.0f, 1.0f, 1.0f }, "%s", Ren::LOG_LEVEL_STRINGS[(int)e.entry.level]);
                    else if (e.entry.level == Ren::LogLevel::critical)
                        ImGui::TextColored({ 1.0f, 0.0f, 1.0f, 1.0f }, "%s", Ren::LOG_LEVEL_STRINGS[(int)e.entry.level]);
                    else
                        ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "%s", Ren::LOG_LEVEL_STRINGS[(int)e.entry.level]);

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", e.entry.message.c_str());
                }
            }

            if (m_scrollBottom && m_autoScroll)
                ImGui::SetScrollHereY(1.0f);
            m_scrollBottom = false;

            ImGui::EndTable();
        }
        ImGui::End();
    }
private:
    struct EntryWrapper { std::string timestamp; Ren::LogInfo entry; };
    Ren::Utils::CyclingArray<EntryWrapper, MAX_LOG_ENTRIES> m_logs;
    bool m_autoScroll = true;
    bool m_scrollBottom = false;
    bool m_pauseLogging = false;
};
