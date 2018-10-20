/*************************************************************************/
/*  os.h                                                                 */
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

#ifndef OS_H
#define OS_H

#include "core/engine.h"
#include "core/image.h"
#include "core/io/logger.h"
#include "core/list.h"
#include "core/os/main_loop.h"
#include "core/ustring.h"
#include "core/vector.h"

#include <stdarg.h>

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class OS {

	static OS *singleton;
	String _execpath;
	List<String> _cmdline;
	bool low_processor_usage_mode;
	int low_processor_usage_mode_sleep_usec;
	bool _verbose_stdout;
	uint64_t _msec_splash;
	int _exit_code;
	char *last_error;

	void *_stack_bottom;

	CompositeLogger *_logger;

	bool restart_on_exit;
	List<String> restart_commandline;

protected:
	void _set_logger(CompositeLogger *p_logger);

public:
	typedef void (*ImeCallback)(void *p_inp, String p_text, Point2 p_selection);
	typedef bool (*HasServerFeatureCallback)(const String &p_feature);

	enum PowerState {
		POWERSTATE_UNKNOWN, /**< cannot determine power status */
		POWERSTATE_ON_BATTERY, /**< Not plugged in, running on the battery */
		POWERSTATE_NO_BATTERY, /**< Plugged in, no battery available */
		POWERSTATE_CHARGING, /**< Plugged in, charging battery */
		POWERSTATE_CHARGED /**< Plugged in, battery charged */
	};

protected:
	friend class Main;

	HasServerFeatureCallback has_server_feature_callback;

	// functions used by main to initialize/deinitialize the OS
	void add_logger(Logger *p_logger);

	virtual void initialize_core() = 0;
	virtual Error initialize_os(int p_audio_driver) = 0;

	virtual void finalize_os() = 0;
	virtual void finalize_core() = 0;

	virtual void set_cmdline(const char *p_execpath, const List<String> &p_args);

	void _ensure_user_data_dir();
	virtual bool _check_internal_feature_support(const String &p_feature) = 0;

public:
	typedef int64_t ProcessID;

	static OS *get_singleton();

	void print_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, Logger::ErrorType p_type = Logger::ERR_ERROR);
	void print(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;
	void printerr(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;

	virtual void alert(const String &p_alert, const String &p_title = "ALERT!") = 0;
	virtual String get_stdin_string(bool p_block = true) = 0;

	virtual void set_last_error(const char *p_error);
	virtual const char *get_last_error() const;
	virtual void clear_last_error();

	virtual int get_audio_driver_count() const;
	virtual const char *get_audio_driver_name(int p_driver) const;

	virtual PoolStringArray get_connected_midi_inputs();
	virtual void open_midi_inputs();
	virtual void close_midi_inputs();

	virtual Error open_dynamic_library(const String p_path, void *&p_library_handle, bool p_also_set_library_path = false) { return ERR_UNAVAILABLE; }
	virtual Error close_dynamic_library(void *p_library_handle) { return ERR_UNAVAILABLE; }
	virtual Error get_dynamic_library_symbol_handle(void *p_library_handle, const String p_name, void *&p_symbol_handle, bool p_optional = false) { return ERR_UNAVAILABLE; }

	virtual void set_low_processor_usage_mode(bool p_enabled);
	virtual bool is_in_low_processor_usage_mode() const;
	virtual void set_low_processor_usage_mode_sleep_usec(int p_usec);
	virtual int get_low_processor_usage_mode_sleep_usec() const;

	virtual String get_executable_path() const;
	virtual Error execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id = NULL, String *r_pipe = NULL, int *r_exitcode = NULL, bool read_stderr = false) = 0;
	virtual Error kill(const ProcessID &p_pid) = 0;
	virtual int get_process_id() const;

	virtual Error shell_open(String p_uri);
	virtual Error set_cwd(const String &p_cwd);

	virtual bool has_environment(const String &p_var) const = 0;
	virtual String get_environment(const String &p_var) const = 0;
	virtual bool set_environment(const String &p_var, const String &p_value) const = 0;

	virtual String get_name() = 0;
	virtual List<String> get_cmdline_args() const { return _cmdline; }
	virtual String get_model_name() const;

	// Returns the main loop from the DisplayDriver singleton
	MainLoop *get_main_loop() const;

	virtual void yield();

	enum Weekday {
		DAY_SUNDAY,
		DAY_MONDAY,
		DAY_TUESDAY,
		DAY_WEDNESDAY,
		DAY_THURSDAY,
		DAY_FRIDAY,
		DAY_SATURDAY
	};

	enum Month {
		/// Start at 1 to follow Windows SYSTEMTIME structure
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx
		MONTH_JANUARY = 1,
		MONTH_FEBRUARY,
		MONTH_MARCH,
		MONTH_APRIL,
		MONTH_MAY,
		MONTH_JUNE,
		MONTH_JULY,
		MONTH_AUGUST,
		MONTH_SEPTEMBER,
		MONTH_OCTOBER,
		MONTH_NOVEMBER,
		MONTH_DECEMBER
	};

	struct Date {

		int year;
		Month month;
		int day;
		Weekday weekday;
		bool dst;
	};

	struct Time {

		int hour;
		int min;
		int sec;
	};

	struct TimeZoneInfo {
		int bias;
		String name;
	};

	virtual Date get_date(bool local = false) const = 0;
	virtual Time get_time(bool local = false) const = 0;
	virtual TimeZoneInfo get_time_zone_info() const = 0;
	virtual uint64_t get_unix_time() const;
	virtual uint64_t get_system_time_secs() const;
	virtual uint64_t get_system_time_msecs() const;

	virtual void delay_usec(uint32_t p_usec) const = 0;
	virtual uint64_t get_ticks_usec() const = 0;
	uint32_t get_ticks_msec() const;
	uint64_t get_splash_tick_msec() const;

	virtual bool is_userfs_persistent() const { return true; }

	bool is_stdout_verbose() const;

	virtual void disable_crash_handler() {}
	virtual bool is_disable_crash_handler() const { return false; }
	virtual void initialize_debugging() {}

	virtual bool get_swap_ok_cancel() { return false; }
	virtual void dump_memory_to_file(const char *p_file);
	virtual void dump_resources_to_file(const char *p_file);
	virtual void print_resources_in_use(bool p_short = false);
	virtual void print_all_resources(String p_to_file = "");

	virtual uint64_t get_static_memory_usage() const;
	virtual uint64_t get_static_memory_peak_usage() const;
	virtual uint64_t get_dynamic_memory_usage() const;
	virtual uint64_t get_free_static_memory() const;

	virtual String get_locale() const;

	String get_safe_dir_name(const String &p_dir_name, bool p_allow_dir_separator = false) const;
	virtual String get_godot_dir_name() const;

	virtual String get_data_path() const;
	virtual String get_config_path() const;
	virtual String get_cache_path() const;

	virtual String get_user_data_dir() const;
	virtual String get_resource_dir() const;

	enum SystemDir {
		SYSTEM_DIR_DESKTOP,
		SYSTEM_DIR_DCIM,
		SYSTEM_DIR_DOCUMENTS,
		SYSTEM_DIR_DOWNLOADS,
		SYSTEM_DIR_MOVIES,
		SYSTEM_DIR_MUSIC,
		SYSTEM_DIR_PICTURES,
		SYSTEM_DIR_RINGTONES,
	};

	virtual String get_system_dir(SystemDir p_dir) const;

	virtual Error move_to_trash(const String &p_path) { return FAILED; }

	virtual void debug_break();

	virtual int get_exit_code() const;
	virtual void set_exit_code(int p_code);

	virtual int get_processor_count() const;

	virtual String get_unique_id() const;

	virtual bool can_use_threads() const;

	virtual Error dialog_show(String p_title, String p_description, Vector<String> p_buttons, Object *p_obj, String p_callback);
	virtual Error dialog_input_text(String p_title, String p_description, String p_partial, Object *p_obj, String p_callback);

	virtual OS::PowerState get_power_state();
	virtual int get_power_seconds_left();
	virtual int get_power_percent_left();

	bool has_feature(const String &p_feature);

	void set_has_server_feature_callback(HasServerFeatureCallback p_callback);

	void set_restart_on_exit(bool p_restart, const List<String> &p_restart_arguments);
	bool is_restart_on_exit_set() const;
	List<String> get_restart_on_exit_arguments() const;

	virtual bool request_permission(const String &p_name) { return true; }

	virtual void process_and_drop_events() {}
	OS();
	virtual ~OS();
};

VARIANT_ENUM_CAST(OS::PowerState);

#endif
