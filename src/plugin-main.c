/*
Shutdown Plugin for OBS Studio
Copyright (C) 2025 Norihiro Kamae

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <stdlib.h>

#include "shutdown-plugin.h"
#include <obs-websocket-api.h>
#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

static struct shutdown_callback_data data_legacy = {
	.deprecation = "use vendor name 'shutdown-plugin' instead of 'obs-shutdown-plugin'",
};

const char *obs_module_name(void)
{
	return obs_module_text("Module.Name");
}

bool obs_module_load(void)
{
	int version_major = atoi(obs_get_version_string());
	if (version_major && version_major < LIBOBS_API_MAJOR_VER) {
		blog(LOG_ERROR, "Cancel loading plugin since OBS version '%s' is older than plugin API version %d",
		     obs_get_version_string(), LIBOBS_API_MAJOR_VER);
		return false;
	}

	/* TODO: Register your source-types, output-types, etc. here. */

	blog(LOG_INFO, "plugin loaded (plugin version %s, %s, API version %d.%d.%d)", PLUGIN_VERSION,
	     VERSION_DESCRIPTION, LIBOBS_API_MAJOR_VER, LIBOBS_API_MINOR_VER, LIBOBS_API_PATCH_VER);
	return true;
}

void obs_module_post_load()
{
	unsigned int api_version = obs_websocket_get_api_version();
	if (api_version == 0) {
		blog(LOG_ERROR, "Unable to fetch obs-websocket plugin API version.");
		return;
	}
	else if (api_version == 1) {
		blog(LOG_WARNING, "Unsupported obs-websocket plugin API version for calling requests.");
		return;
	}

	obs_websocket_vendor vendor = obs_websocket_register_vendor("shutdown-plugin");
	if (!vendor) {
		blog(LOG_ERROR,
		     "Vendor registration failed! (obs-websocket should have logged something if installed properly.)");
		return;
	}

	if (!obs_websocket_vendor_register_request(vendor, "shutdown", shutdown_callback, NULL)) {
		blog(LOG_ERROR, "Failed to register 'shutdown' request with obs-websocket.");
		return;
	}

	vendor = obs_websocket_register_vendor("obs-shutdown-plugin");
	if (vendor && !obs_websocket_vendor_register_request(vendor, "shutdown", shutdown_callback, &data_legacy)) {
		blog(LOG_ERROR, "Failed to register 'shutdown' request with obs-websocket.");
		return;
	}

	blog(LOG_INFO, "Registered 'shutdown' to obs-websocket");
}
