/*************************************************************************/
/*  displaydriver.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "core/os/display_driver.h"
#include "os.h"

#include "input.h"

DisplayDriver *DisplayDriver::singleton = NULL;

DisplayDriver *DisplayDriver::get_singleton() {

	return singleton;
}

void DisplayDriver::set_keep_screen_on(bool p_enabled) {
	_keep_screen_on = p_enabled;
}

bool DisplayDriver::is_keep_screen_on() const {
	return _keep_screen_on;
}

void DisplayDriver::set_clipboard(const String &p_text) {

	_local_clipboard = p_text;
}
String DisplayDriver::get_clipboard() const {

	return _local_clipboard;
}

bool DisplayDriver::has_virtual_keyboard() const {

	return false;
}

void DisplayDriver::show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect) {
}

void DisplayDriver::hide_virtual_keyboard() {
}

int DisplayDriver::get_virtual_keyboard_height() const {
	return 0;
}

void DisplayDriver::set_cursor_shape(CursorShape p_shape) {
}

DisplayDriver::CursorShape DisplayDriver::get_cursor_shape() const {
	return CURSOR_ARROW;
}

void DisplayDriver::set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot) {
}

void DisplayDriver::set_no_window_mode(bool p_enable) {

	_no_window = p_enable;
}

bool DisplayDriver::is_no_window_mode_enabled() const {

	return _no_window;
}

// implement these with the canvas?
Error DisplayDriver::dialog_show(String p_title, String p_description, Vector<String> p_buttons, Object *p_obj, String p_callback) {

	while (true) {

		OS::get_singleton()->print("%ls\n--------\n%ls\n", p_title.c_str(), p_description.c_str());
		for (int i = 0; i < p_buttons.size(); i++) {
			if (i > 0) OS::get_singleton()->print(", ");
			OS::get_singleton()->print("%i=%ls", i + 1, p_buttons[i].c_str());
		};
		OS::get_singleton()->print("\n");
		String res = OS::get_singleton()->get_stdin_string().strip_edges();
		if (!res.is_numeric())
			continue;
		int n = res.to_int();
		if (n < 0 || n >= p_buttons.size())
			continue;
		if (p_obj && p_callback != "")
			p_obj->call_deferred(p_callback, n);
		break;
	};
	return OK;
};

Error DisplayDriver::dialog_input_text(String p_title, String p_description, String p_partial, Object *p_obj, String p_callback) {

	ERR_FAIL_COND_V(!p_obj, FAILED);
	ERR_FAIL_COND_V(p_callback == "", FAILED);
	OS::get_singleton()->print("%ls\n---------\n%ls\n[%ls]:\n", p_title.c_str(), p_description.c_str(), p_partial.c_str());

	String res = OS::get_singleton()->get_stdin_string().strip_edges();
	bool success = true;
	if (res == "") {
		res = p_partial;
	};

	p_obj->call_deferred(p_callback, success, res);

	return OK;
};

bool DisplayDriver::has_touchscreen_ui_hint() const {

	//return false;
	return Input::get_singleton() && Input::get_singleton()->is_emulating_touch_from_mouse();
}

void DisplayDriver::set_screen_orientation(ScreenOrientation p_orientation) {

	_orientation = p_orientation;
}

DisplayDriver::ScreenOrientation DisplayDriver::get_screen_orientation() const {

	return (DisplayDriver::ScreenOrientation)_orientation;
}

void DisplayDriver::set_icon(const Ref<Image> &p_icon) {
}

void DisplayDriver::release_rendering_thread() {
}

void DisplayDriver::make_rendering_thread() {
}

void DisplayDriver::swap_buffers() {
}

Error DisplayDriver::native_video_play(String p_path, float p_volume, String p_audio_track, String p_subtitle_track) {

	return FAILED;
};

bool DisplayDriver::native_video_is_playing() const {

	return false;
};

void DisplayDriver::native_video_pause(){

};

void DisplayDriver::native_video_unpause(){

};

void DisplayDriver::native_video_stop(){

};

void DisplayDriver::set_mouse_mode(MouseMode p_mode) {
}

DisplayDriver::MouseMode DisplayDriver::get_mouse_mode() const {

	return MOUSE_MODE_VISIBLE;
}

DisplayDriver::LatinKeyboardVariant DisplayDriver::get_latin_keyboard_variant() const {

	return LATIN_KEYBOARD_QWERTY;
}

bool DisplayDriver::is_joy_known(int p_device) {
	return true;
}

String DisplayDriver::get_joy_guid(int p_device) const {
	return "Default Joypad";
}

void DisplayDriver::set_context(int p_context) {
}

DisplayDriver::SwitchVSyncCallbackInThread DisplayDriver::switch_vsync_function = NULL;

void DisplayDriver::set_use_vsync(bool p_enable) {
	_use_vsync = p_enable;
	if (switch_vsync_function) { //if a function was set, use function
		switch_vsync_function(p_enable);
	} else { //otherwise just call here
		_set_use_vsync(p_enable);
	}
}

bool DisplayDriver::is_vsync_enabled() const {

	return _use_vsync;
}

void DisplayDriver::center_window() {

	if (is_window_fullscreen()) return;

	Point2 sp = get_screen_position(get_current_screen());
	Size2 scr = get_screen_size(get_current_screen());
	Size2 wnd = get_real_window_size();

	int x = sp.width + (scr.width - wnd.width) / 2;
	int y = sp.height + (scr.height - wnd.height) / 2;

	set_window_position(Vector2(x, y));
}

int DisplayDriver::get_video_driver_count() const {

	return 2;
}

const char *DisplayDriver::get_video_driver_name(int p_driver) const {

	switch (p_driver) {
		case VIDEO_DRIVER_GLES2:
			return "GLES2";
		case VIDEO_DRIVER_GLES3:
			return "GLES3";
		default:
			return "INVALID VIDEO DRIVER";
	}
}

void DisplayDriver::process_events() {
}

DisplayDriver::DisplayDriver() {
	singleton = this;
	_keep_screen_on = true; // set default value to true, because this had been true before godot 2.0.
	_no_window = false;
	_orientation = SCREEN_LANDSCAPE;

	_render_thread_mode = RENDER_THREAD_SAFE;

	_allow_hidpi = false;
	_allow_layered = false;
	_use_vsync = false;
}

DisplayDriver::~DisplayDriver() {
	singleton = NULL;
}
