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

#include "tundra/core/KeyCodes.hpp"
#include "tundra/core/TimeObserver.hpp"


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

public:

	friend struct Tundra::Internal::InputManager;

	/**
	 * No key range checking exists, or is needed, in any public function where
	 * the user can pass in a key value. This is becuase the parameter for the
	 * keys that the user can pass in are Tundra::KEYCODES, which are restricted
	 * to the values of [0, 127], which is the exact range of the array(s) that
	 * these functions index.
	 */

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
		virtual ~ICallback() {}

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

		~Callback() final {}

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

}; // Class InputManager

} // Namespace Tundra
