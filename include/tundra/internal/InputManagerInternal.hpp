/**
 * @file InputManagerInternal.hpp.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "tundra/core/InputManager.hpp"


namespace Tundra
{

namespace Internal
{

/**
 * @struct InputManager
 * @brief Internal-only interface for low-level key state management.
 * 
 * This internal struct provides direct control over the raw and processed key 
 * state flags in Tundra::InputManager. It is intended for use by the core 
 * engine systems that process platform input events and should not be accessed 
 * by end-users.
 */
struct InputManager 
{  
	/**
	 * @brief Iterates over all keys, checking if they are pressed and handling
	 * them if they are.
	 */
	static void handle_pressed_keys()
	{
		// Parse all keys and handle them if they are pressed.
		// Skip 0 - 8, since these are keycodes that we don't need to worry
		// about.
		for(uint8_t i = 8; i < 128; ++i)
		{
			if(Tundra::InputManager::s_pressed_keys[i]) 
				Tundra::InputManager::handle_pressed_key_check(
					static_cast<Tundra::KEYCODE>(i));
		}
	}

	/**
	 * @brief Resets all raw key press flags.
	 * 
	 * Called at the end of each input processing cycle to clear the temporary
	 * state that tracks which keys are pressed this frame. This is used to 
	 * distinguish key press events from held states.
	 */
	static void reset_raw_pressed_keys() 
	{
        Tundra::InputManager::s_raw_pressed_keys.reset();
    }

	/**
	 * @brief Marks the given key as pressed in both raw and persistent state.
	 * 
	 * Called when a key is first detected as pressed. Updates both the 
	 * persistent `s_pressed_keys` and the frame-local `s_raw_pressed_keys`.
	 * 
	 * @param k The key code to mark as pressed.
	 */
    static void flag_key_pressed(Tundra::KEYCODE key) 
	{
        Tundra::InputManager::s_pressed_keys[static_cast<uint8_t>(key)] = 1;
        Tundra::InputManager::s_raw_pressed_keys[static_cast<uint8_t>(key)] = 1;
    }

	/**
	 * @brief Marks the given key as released and updates its raw state.
	 * 
	 * Called when a key is released. Updates the persistent pressed state.
	 * 
	 * @param k The key code to mark as released.
	 */
    static void flag_key_released(Tundra::KEYCODE key) 
	{
        Tundra::InputManager::s_pressed_keys[static_cast<uint8_t>(key)] = 0;

		// Set blocked timestamp to 0 since this key is released.
		Tundra::InputManager::s_keys_to_blocked_timestamps[
			static_cast<uint8_t>(key)] = 0;

		// Raw keys are cleared each frame alread, no need to do that here. 
    }
};

} // Namespace Internal

} // Namespace Tundra
