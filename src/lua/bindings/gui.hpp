#pragma once
#include "gui/button.hpp"
#include "gui/checkbox.hpp"
#include "gui/gui_element.hpp"
#include "gui/input_float.hpp"
#include "gui/input_int.hpp"
#include "gui/input_string.hpp"
#include "gui/raw_imgui_callback.hpp"
#include "gui/sameline.hpp"
#include "gui/separator.hpp"
#include "gui/text.hpp"
#include "lua/lua_module.hpp"

namespace lua::gui
{
	// Lua API: Class
	// Name: tab
	// Class for representing a tab within the GUI.
	class tab
	{
		rage::joaat_t m_tab_hash;

	public:
		tab(rage::joaat_t hash) :
			m_tab_hash(hash)
		{
		}

		rage::joaat_t hash() const;

		// Lua API: Function
		// Class: tab
		// Name: clear
		// Clear the tab of all its custom lua content that you own.
		void clear(sol::this_state state)
		{
			auto module = sol::state_view(state)["!this"].get<big::lua_module*>();

			if (module->m_gui.contains(m_tab_hash))
				module->m_gui[m_tab_hash] = {};
		}


		// Lua API: Function
		// Class: tab
		// Name: add_button
		// Param: name: string: Text written inside the button.
		// Param: callback: function: function that will be called when the button is clicked.
		// Add a button to the gui tab.
		std::shared_ptr<lua::gui::button> add_button(const std::string& name, sol::protected_function callback, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_text
		// Param: name: string: Text that will be written.
		// Returns: text: The text object instance.
		// Add text to the gui tab.
		std::shared_ptr<lua::gui::text> add_text(const std::string& name, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_checkbox
		// Param: name: string: Text that will be written next to the checkbox.
		// Returns: checkbox: The checkbox object instance.
		// Add a checkbox widget to the gui tab.
		std::shared_ptr<lua::gui::checkbox> add_checkbox(const std::string& name, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_sameline
		// Returns: sameline: The sameline object instance.
		// Add a ImGui::SameLine.
		std::shared_ptr<lua::gui::sameline> add_sameline(sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_separator
		// Returns: separator: The separator object instance.
		// Add a ImGui::Separator.
		std::shared_ptr<lua::gui::separator> add_separator(sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_input_int
		// Param: name: string: Text that will be written next to the input field.
		// Returns: input_int: The input_int object instance.
		// Add a ImGui::InputInt.
		std::shared_ptr<lua::gui::input_int> add_input_int(const std::string& name, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_input_float
		// Param: name: string: Text that will be written next to the input field.
		// Returns: input_float: The input_float object instance.
		// Add a ImGui::InputFloat.
		std::shared_ptr<lua::gui::input_float> add_input_float(const std::string& name, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_input_string
		// Param: name: string: Text that will be written next to the input field.
		// Returns: input_string: The input_string object instance.
		// Add a ImGui::InputText.
		std::shared_ptr<lua::gui::input_string> add_input_string(const std::string& name, sol::this_state state);

		// Lua API: Function
		// Class: tab
		// Name: add_imgui
		// Param: imgui_rendering: function: Function that will be called every rendering frame, you can call ImGui functions in it, please check the ImGui.md documentation file for more info.
		// Registers a function that will be called every rendering frame, you can call ImGui functions in it, please check the ImGui.md documentation file for more info.
		// **Example Usage:**
		// ```lua
		// tab:add_imgui(function()
		//   if ImGui.Begin("My Custom Window") then
		//       if ImGui.Button("Label") then
		//         script.run_in_fiber(function(script)
		//           -- call natives in there
		//         end)
		//       end
		//
		//       ImGui.End()
		//   end
		// end)
		// ```
		std::shared_ptr<lua::gui::raw_imgui_callback> add_imgui(sol::protected_function imgui_rendering, sol::this_state state);
	};

	void bind(sol::state& state);
}