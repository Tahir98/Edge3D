#pragma once

#include "Event.h"
#include "Input/InputHandler.h"
/*
enum class EventType {
		WindowMoved, WindowResized, WindowClosed,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		KeyPressed, KeyReleased, KeyRepeated
	};
*/

namespace Edge3D {
	class KeyPressedEvent : public Event {
	private:
		int key;
	public:
		KeyPressedEvent(int key) : key(key){
			category = EventCategory::KeyEvent;
			type = EventType::KeyPressed;

			if (key <= 93) {
				InputHandler::keyboard[key - 32].second = true;
			}
			else if (key >= 256 && key <= 348) {
				InputHandler::keyboard[65 + (key - 256)].second = true;
			}
			else {
				if (key == 96) InputHandler::keyboard[62].second = true;
				if (key == 161) InputHandler::keyboard[63].second = true;
				if (key == 162) InputHandler::keyboard[64].second = true;
			}
		}

		std::string getMessage() {
			return ("Key Pressed Event, keycode:" + std::to_string(key));
		}

		int getKey() {
			return key;
		}
	};

	class KeyReleasedEvent : public Event {
	private:
		int key;
	public:
		KeyReleasedEvent(int key) : key(key) {
			category = EventCategory::KeyEvent;
			type = EventType::KeyReleased;

			if (key <= 93) {
				InputHandler::keyboard[key - 32].second = false;
			}
			else if (key >= 256 && key <= 348) {
				InputHandler::keyboard[65 + (key - 256)].second = false;
			}
			else {
				if (key == 96) InputHandler::keyboard[62].second = false;
				if (key == 161) InputHandler::keyboard[63].second = false;
				if (key == 162) InputHandler::keyboard[64].second = false;
			}
		}

		std::string getMessage() {
			return ("Key Released Event, keycode:" + std::to_string(key));
		}

		int getKey() {
			return key;
		}
	};

	class KeyRepeatedEvent : public Event {
	private:
		int key;
	public:
		KeyRepeatedEvent(int key) : key(key) {
			category = EventCategory::KeyEvent;
			type = EventType::KeyRepeated;
		}

		std::string getMessage() {
			return ("Key Repeated Event, keycode:" + std::to_string(key));
		}

		int getKey() {
			return key;
		}
	};
}
