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
#include <array>
#include <vector>
#include <functional>


namespace Tundra 
{

using callback_id = uint64_t;
using key = uint8_t;

class InputManager
{

public:

	/**
	 * @brief Invokes all callbacks for the passed key.
	 * 
	 * @param target_key Key to look up the attached callbacks for.
	 */
	static void invoke_callbacks(key target_key)
	{
		// This key has a delay to it.
		if(s_keys_to_delay_timestamps.at(target_key) != 0)
		{
			
		}

		const std::vector<CallbackTracker>& targ_vec 
			= s_keys_to_callback_funcs.at(target_key);

		for(const CallbackTracker& tracker : targ_vec)
		{
			tracker.function(tracker.obj_instance);
		}
	}

	/**
	 * @brief Removes a registered callback for a specific key by ID.
	 *
	 * Searches for a callback tied to the given key and callback ID. If found,
	 * removes it from the internal list. 
	 *
	 * @param target_key The key associated with the callback.
	 * @param id_of_callback The unique ID of the callback to remove.
	 */
	static void remove_callback(key target_key, callback_id id_of_callback)
	{
		std::vector<CallbackTracker>::iterator it = 
			s_keys_to_callback_funcs.at(target_key).begin();

		while(it != s_keys_to_callback_funcs.at(target_key).end())
		{
			// ID match.
			if(it->unique_ID == id_of_callback) 
			{
				s_keys_to_callback_funcs.at(target_key).erase(it);
				break;
			}

			// These are not the IDs you are looking for.
			++it;
		}
	}

	/**
	 * @brief Delays a key input for a specified amount of time.
	 * 
	 * @param target_key Key to delay.
	 * @param ms Number of miliseconds to delay.
	 */
	static void delay_key(key target_key, uint32_t ms = 200)
	{

	}

	/**
	 * @brief Registers a member function callback for a specific key.
	 *
	 * Associates a class instance method with a key. When the key is pressed,
	 * the given member function is called on the provided instance.
	 *
	 * @tparam T The class type of the object with the member function.
	 * @param target_key The key that triggers the callback (e.g., ASCII code).
	 * @param method Target function to call when the `target_key` is pressed.
	 * @param instance Pointer to the instance on which the method will be 
	 * 				   called.
	 * @return callback_id A unique ID for the callback, used for later removal.
	 */
	template<typename T>
	static callback_id register_callback(key target_key, void (T::*method)(), 
		T* instance)
	{
		s_keys_to_callback_funcs.at(target_key).push_back(_create_callback(
			method, instance));

		return s_keys_to_callback_funcs.at(target_key).back().unique_ID;
	}

private:

	using CallbackFunc = std::function<void()>;

	// # CLASSES / STRUCTS # --------------------------------------------------

	/**
	 * @brief Contains data for a stored Callback. 
	 * 
	 * This includes the callback function, instance object of that function, 
	 * the delayed timestamp and its unique ID.
	 */
	struct CallbackTracker
	{
		// Method to invoke.
		void (*function)(void* obj_instance); 
    	
		// Object instance owning the function to invoke.
		void* obj_instance; 

		// Unique ID of this Callback
		uint64_t unique_ID;
	};


	// # MEMBERS # ------------------------------------------------------------
	
	// The next ID to be generated when a unique ID is needed.
	static inline callback_id s_next_ID {};

	// Keys to their delayed timestamps. 
	static inline std::array<uint64_t, 128> s_keys_to_delay_timestamps {};

	// Keys to their list of callback containers. Each index into the array
	// represents the ascii value of the key.
	static std::array<std::vector<CallbackTracker>, 128> 
		s_keys_to_callback_funcs;


	// # METHODS # ------------------------------------------------------------


	/**
	 * @brief Creates a trampoline-based callback from a member function.
	 *
	 * Wraps a class method and instance into a CallbackTracker using a lambda
	 * trampoline. The method is stored as a function pointer and can be called
	 * later using the instance pointer.
	 *
	 * @tparam T The type of the class with the method to register.
	 * @param method Pointer to the member function (must be void()).
	 * @param instance Pointer to the instance on which the method will be 
	 * 				   called.
	 * @return CallbackTracker The wrapped function and metadata.
	 */
	template<typename T>
	static CallbackTracker _create_callback(void (T::*method)(), T* instance) 
	{
		return 
		{
			[method](void* ctx) {
				T* obj = static_cast<T*>(ctx);
				(obj->*method)();
			},
			instance,
			0,
			s_next_ID++,
		};
	}

};

}
