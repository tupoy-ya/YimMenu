#include "gui.hpp"

#include "backend/bool_command.hpp"
#include "lua/lua_manager.hpp"
#include "natives.hpp"
#include "renderer/font_mgr.hpp"
#include "renderer/renderer.hpp"
#include "script.hpp"
#include "views/view.hpp"

#include <imgui.h>

namespace big
{
	/**
	 * @brief The later an entry comes in this enum to higher up it comes in the z-index.
	 */
	enum eRenderPriority
	{
		// low priority
		ESP,
		CONTEXT_MENU,

		// medium priority
		MENU = 0x1000,
		VEHICLE_CONTROL,
		LUA,

		// high priority
		INFO_OVERLAY = 0x2000,
		CMD_EXECUTOR,

		GTA_DATA_CACHE = 0x3000,
		ONBOARDING,

		// should remain in a league of its own
		NOTIFICATIONS = 0x4000,
	};

	gui::gui() :
	    m_is_open(false),
	    m_override_mouse(false)
	{
		g_renderer.add_dx_callback(view::notifications, eRenderPriority::NOTIFICATIONS);
		g_renderer.add_dx_callback(view::onboarding, eRenderPriority::ONBOARDING);
		g_renderer.add_dx_callback(view::gta_data, eRenderPriority::GTA_DATA_CACHE);
		g_renderer.add_dx_callback(view::cmd_executor, eRenderPriority::CMD_EXECUTOR);
		g_renderer.add_dx_callback(view::overlay, eRenderPriority::INFO_OVERLAY);

		g_renderer.add_dx_callback(esp::draw, eRenderPriority::ESP); // TODO: move to ESP service
		g_renderer.add_dx_callback(view::context_menu, eRenderPriority::CONTEXT_MENU);

		g_renderer.add_dx_callback(
		    [this] {
			    dx_on_tick();
		    },
		    eRenderPriority::MENU);

		g_renderer.add_dx_callback(
		    [] {
			    g_lua_manager->draw_always_draw_gui();
		    },
		    eRenderPriority::LUA);

		g_renderer.add_wndproc_callback([](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			g_lua_manager->trigger_event<menu_event::Wndproc>(hwnd, msg, wparam, lparam);
		});
		g_renderer.add_wndproc_callback([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			wndproc(hwnd, msg, wparam, lparam);
		});
		g_renderer.add_wndproc_callback([](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			if (g.cmd_executor.enabled && msg == WM_KEYUP && wparam == VK_ESCAPE)
			{
				bool_command::get("cmdexecutor"_J)->call({});
			}
		});


		dx_init();

		g_gui = this;
		g_renderer.rescale(g.window.gui_scale);
		g_renderer.get_font_mgr().update_required_alphabet_type(g.window.alphabet);
	}

	gui::~gui()
	{
		g_gui = nullptr;
	}

	bool gui::is_open()
	{
		return m_is_open;
	}

	void gui::toggle(bool toggle)
	{
		m_is_open = toggle;

		toggle_mouse();
	}

	void gui::override_mouse(bool override)
	{
		m_override_mouse = override;

		toggle_mouse();
	}

	void gui::dx_init()
	{
		auto& style                  = ImGui::GetStyle();
		style.WindowPadding          = {10.f, 10.f};
		style.PopupRounding          = 3.f;
		style.FramePadding           = {8.f, 4.f};
		style.ItemSpacing            = {10.f, 8.f};
		style.ItemInnerSpacing       = {6.f, 6.f};
		style.TouchExtraPadding      = {0.f, 0.f};
		style.IndentSpacing          = 21.f;
		style.ScrollbarSize          = 15.f;
		style.GrabMinSize            = 8.f;
		style.WindowBorderSize       = 0.f;
		style.ChildBorderSize        = 0.f;
		style.PopupBorderSize        = 0.f;
		style.FrameBorderSize        = 0.f;
		style.TabBorderSize          = 0.f;
		style.WindowRounding         = 5.f;
		style.ChildRounding          = 2.f;
		style.FrameRounding          = 3.f;
		style.ScrollbarRounding      = 3.f;
		style.GrabRounding           = 0.f;
		style.TabRounding            = 3.f;
		style.WindowTitleAlign       = {0.5f, 0.5f};
		style.ButtonTextAlign        = {0.5f, 0.5f};
		style.DisplaySafeAreaPadding = {3.f, 3.f};

		auto& colors                          = style.Colors;
		colors[ImGuiCol_Text]                 = ImGui::ColorConvertU32ToFloat4(g.window.text_color);
		colors[ImGuiCol_TextDisabled]         = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
		colors[ImGuiCol_WindowBg]             = ImGui::ColorConvertU32ToFloat4(g.window.background_color);
		colors[ImGuiCol_ChildBg]              = ImGui::ColorConvertU32ToFloat4(g.window.background_color);
		colors[ImGuiCol_PopupBg]              = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border]               = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive]        = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark]            = ImVec4(1.00f, 0.98f, 0.95f, 0.61f);
		colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button]               = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header]               = ImVec4(0.30f, 0.29f, 0.32f, 1.00f);
		colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

		save_default_style();
	}

	void gui::dx_on_tick()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		if (m_is_open)
		{
			push_theme_colors();
			view::root(); // frame bg
			pop_theme_colors();
		}
	}

	void gui::save_default_style()
	{
		memcpy(&m_default_config, &ImGui::GetStyle(), sizeof(ImGuiStyle));
	}

	void gui::restore_default_style()
	{
		memcpy(&ImGui::GetStyle(), &m_default_config, sizeof(ImGuiStyle));
	}

	void gui::push_theme_colors()
	{
		auto button_color = ImGui::ColorConvertU32ToFloat4(g.window.button_color);
		auto button_hovered_color =
		    ImVec4(button_color.x + 0.15f, button_color.y + 0.15f, button_color.z + 0.15f, button_color.w);
		auto button_active_color =
		    ImVec4(button_color.x + 0.33f, button_color.y + 0.33f, button_color.z + 0.33f, button_color.w);
		auto tab_color = ImGui::ColorConvertU32ToFloat4(g.window.tab_color);
		auto tab_hovered_color =
		    ImVec4(tab_color.x + 0.2f, tab_color.y + 0.2f, tab_color.z + 0.2f, tab_color.w);
		auto tab_active_color =
		    ImVec4(tab_color.x + 0.1f, tab_color.y + 0.1f, tab_color.z + 0.1f, tab_color.w);
		auto frame_color = ImGui::ColorConvertU32ToFloat4(g.window.frame_color);
		auto frame_hovered_color =
		    ImVec4(frame_color.x + 0.14f, frame_color.y + 0.14f, frame_color.z + 0.14f, button_color.w);
		auto frame_active_color =
		    ImVec4(frame_color.x + 0.30f, frame_color.y + 0.30f, frame_color.z + 0.30f, button_color.w);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(g.window.background_color));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(g.window.text_color));
		ImGui::PushStyleColor(ImGuiCol_Button, button_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hovered_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_active_color);
		ImGui::PushStyleColor(ImGuiCol_Tab, tab_color);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, tab_hovered_color);
		ImGui::PushStyleColor(ImGuiCol_TabActive, tab_active_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hovered_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_active_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_hovered_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_active_color);
	}

	void gui::pop_theme_colors()
	{
		ImGui::PopStyleColor(13);
	}

	void gui::script_on_tick()
	{
		if (g_gui->m_is_open || g_gui->m_override_mouse)
		{
			for (uint8_t i = 0; i <= 6; i++)
				PAD::DISABLE_CONTROL_ACTION(2, i, true);
			PAD::DISABLE_CONTROL_ACTION(2, 106, true);
			PAD::DISABLE_CONTROL_ACTION(2, 329, true);
			PAD::DISABLE_CONTROL_ACTION(2, 330, true);

			PAD::DISABLE_CONTROL_ACTION(2, 14, true);
			PAD::DISABLE_CONTROL_ACTION(2, 15, true);
			PAD::DISABLE_CONTROL_ACTION(2, 16, true);
			PAD::DISABLE_CONTROL_ACTION(2, 17, true);
			PAD::DISABLE_CONTROL_ACTION(2, 24, true);
			PAD::DISABLE_CONTROL_ACTION(2, 69, true);
			PAD::DISABLE_CONTROL_ACTION(2, 70, true);
			PAD::DISABLE_CONTROL_ACTION(2, 84, true);
			PAD::DISABLE_CONTROL_ACTION(2, 85, true);
			PAD::DISABLE_CONTROL_ACTION(2, 99, true);
			PAD::DISABLE_CONTROL_ACTION(2, 92, true);
			PAD::DISABLE_CONTROL_ACTION(2, 100, true);
			PAD::DISABLE_CONTROL_ACTION(2, 114, true);
			PAD::DISABLE_CONTROL_ACTION(2, 115, true);
			PAD::DISABLE_CONTROL_ACTION(2, 121, true);
			PAD::DISABLE_CONTROL_ACTION(2, 142, true);
			PAD::DISABLE_CONTROL_ACTION(2, 241, true);
			PAD::DISABLE_CONTROL_ACTION(2, 261, true);
			PAD::DISABLE_CONTROL_ACTION(2, 257, true);
			PAD::DISABLE_CONTROL_ACTION(2, 262, true);
			PAD::DISABLE_CONTROL_ACTION(2, 331, true);
		}
	}

	void gui::script_func()
	{
		while (true)
		{
			g_gui->script_on_tick();
			script::get_current()->yield();
		}
	}

	void gui::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == g.settings.hotkeys.menu_toggle)
		{
			//Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui->m_is_open)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			toggle(g.settings.hotkeys.editing_menu_toggle || !m_is_open);
			if (g.settings.hotkeys.editing_menu_toggle)
				g.settings.hotkeys.editing_menu_toggle = false;
		}
	}

	void gui::toggle_mouse()
	{
		if (g_gui->m_is_open || g_gui->m_override_mouse)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
	}
}
