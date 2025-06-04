/**
 * @file InputManager.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-29-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <cstdint>
#include <bitset>
#include <array>
#include <vector>

#include <iostream>

#include "core/KeyCodes.hpp"
#include "core/TimeObserver.hpp"


namespace Tundra 
{

using callback_id = uint64_t;

namespace Internal
{
	struct InputManager; // Forward declaration for friend struct use.
}


/**
 * @class InputManager
 * @brief Automatically tracks inputs from the keyboard, as well as processing 
 * user specified callback invocation from said pressed keys.
 * 
 * The InputManager class manages the registration and invocation of callbacks
 * tied to key presses. It supports storing member function callbacks associated
 * with individual keys, removing them by ID, and invoking them at runtime.
 */
class InputManager
{

	/**
	 * No key range checking exists, or is needed, in any public function where
	 * the user can pass in a key value. This is becuase the parameter for the
	 * keys that the user can pass in are Tundra::KEYCODES, which are restricted
	 * to the values of [0, 127], which is the exact range of the array(s) that
	 * these functions index.
	 */

public:

	friend struct Tundra::Internal::InputManager;

	/**
	 * @brief Removes a registered callback for a specific key by ID.
	 *
	 * Searches for a callback tied to the given key and callback ID. If found,
	 * removes it from the internal list. 
	 *
	 * @param key The key associated with the callback.
	 * @param id_of_callback The unique ID of the callback to remove.
	 */
	static void remove_callback(Tundra::KEYCODE key, 
		callback_id id_of_callback)
	{
		std::vector<ICallback*>::iterator it = 
			s_keys_to_callback_funcs.at(
			static_cast<uint8_t>(key)).begin();

		while(it != s_keys_to_callback_funcs.at(
			static_cast<uint8_t>(key)).end())
		{
			// ID match.
			if((*it)->unique_id == id_of_callback) 
			{
				// Delete the ICallback
				delete *it;

				// Remove the ICallback from the vector of Callbacks.
				s_keys_to_callback_funcs.at(
					static_cast<uint8_t>(key)).erase(it);
				return;
			}

			// These are not the IDs you are looking for.
			++it;
		}

		// If the function reached this point, the ID was not found.

		// TODO: Add error output for invalid callback_id.
	}

	/**
	 * @brief Blocks a key for a specified amount of time, rendering it unable
	 * to be registered as pressed until the delay passes.
	 * 
	 * @param key Key to block.
	 * @param ms Number of miliseconds to block.
	 */
	static void block_key(Tundra::KEYCODE key, uint32_t ms = 200)
	{
		s_keys_to_blocked_timestamps[static_cast<uint8_t>(key)] = 
			TimeObserver::get_elapsed() + ms;
	}

	/**
	 * @brief Registers a member function callback for a specific key.
	 *
	 * Associates a class instance method with a key. When the key is pressed,
	 * the given member function is called on the provided instance.
	 *
	 * @tparam T The class type of the object with the member function.
	 * @param key The key that triggers the callback (e.g., ASCII code).
	 * @param method Target function to call when the `key` is pressed.
	 * @param instance Pointer to the instance on which the method will be 
	 * 				   called.
	 * @return callback_id A unique ID for the callback, used for later removal.
	 */
	template<typename T>
	static callback_id register_callback(Tundra::KEYCODE key, 
		void (T::*method)(), T* instance)
	{
		callback_id new_callback_id = s_next_ID++;

		s_keys_to_callback_funcs.at(static_cast<uint8_t>(key)).push_back(
			new Callback<T>(method, instance, new_callback_id));

		return new_callback_id;
	}

private:

	// # CLASSES / STRUCTS # --------------------------------------------------

	/**
	 * @brief Base class for storing callback data.
	 * 
	 * Each callback has its own unique ID (callback_id).
	 */
	struct ICallback
	{
		ICallback(callback_id _unique_id) : unique_id(_unique_id) {}

		virtual void invoke() = 0;
		const callback_id unique_id;
	};

	/**
	 * @brief Inherited class that offers templated function invoking for 
	 * a stored function.
	 * 
	 * @tparam T Type of class that owns the method being invoked.
	 */
	template<typename T>
	class Callback : public ICallback 
	{
	public:
		Callback(void (T::*method)(), T* instance, uint64_t id)
			: ICallback(id)
		{
			m_method = method;
			m_instance = instance;
		}

		void invoke() final 
		{
			(m_instance->*m_method)();
		}

	private:
		
		// Method to invoke.
		void (T::*m_method)();
		
		// Instance owning the invoking method. 
		T* m_instance;
	};


	// # MEMBERS # ------------------------------------------------------------
	
	// The next ID to be generated when a unique ID is needed.
	static inline callback_id s_next_ID {};

	// Bitset representing physically pressed keys.
	static inline std::bitset<128> s_pressed_keys {};
	
	// Bitset representing keys that have been registered as pressed this frame.
	static inline std::bitset<128> s_raw_pressed_keys {};

	// Keys to their delayed timestamps. 
	static inline uint64_t s_keys_to_blocked_timestamps[128] {};

	// Keys to their list of callback containers. Each index into the array
	// represents the ascii value of the key.
	static inline std::array<std::vector<ICallback*>, 128> 
		s_keys_to_callback_funcs {};


	// # METHODS # ------------------------------------------------------------

	/**
	 * @brief Invokes all callbacks for the passed key.
	 * 
	 * @param key Key to look up the attached callbacks for.
	 */
	static void invoke_callbacks(Tundra::KEYCODE key)
	{
		// Invalid key.
		if(static_cast<uint8_t>(key) > 127) return;

		const std::vector<ICallback*>& targ_vec 
			= s_keys_to_callback_funcs.at(static_cast<uint8_t>(key));

		for(ICallback* callback : targ_vec)
		{
			// tracker.function(tracker.obj_instance);
			callback->invoke();
		}
	}

	/**
	 * @brief Handles the blocked timestamp checking for a pressed key, as well
	 * as invoking the callbacks for the key if it is not delayed.
	 * 
	 * @param key Key to check
	 */
	static void handle_pressed_key_check(Tundra::KEYCODE key) 
	{
		uint64_t& blocked_timestamp = s_keys_to_blocked_timestamps[
			static_cast<uint8_t>(key)];

		// If this key is blocked
		if(TimeObserver::get_elapsed() <= blocked_timestamp) return;

		// Key is not blocked, set the blocked timestamp to 0 and invoke 
		// callbacks.
		blocked_timestamp = 0;

		invoke_callbacks(key);
	}

};


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
		for(uint8_t i = 0; i < 128; ++i)
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
