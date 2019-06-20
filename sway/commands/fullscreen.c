#include <strings.h>
#include "log.h"
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/view.h"
#include "sway/tree/workspace.h"
#include "util.h"

// fullscreen [enable|disable|toggle] [global]
struct cmd_results *cmd_fullscreen(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "fullscreen", EXPECTED_AT_MOST, 2))) {
		return error;
	}
	if (!root->outputs->length) {
		return cmd_results_new(CMD_FAILURE,
				"Can't run this command while there's no outputs connected.");
	}
	struct sway_node *node = config->handler_context.node;
	struct sway_container *container = config->handler_context.container;
	struct sway_workspace *workspace = config->handler_context.workspace;
	if (node->type == N_WORKSPACE && workspace->tiling->length == 0) {
		return cmd_results_new(CMD_FAILURE,
				"Can't fullscreen an empty workspace");
	}

	// If in the scratchpad, operate on the highest container
	if (container && !container->workspace) {
		while (container->parent) {
			container = container->parent;
		}
	}

	bool is_fullscreen = container &&
		container->fullscreen_mode != FULLSCREEN_NONE &&
		container->fullscreen_mode != FULLSCREEN_VIEW;
	bool enable = !is_fullscreen;

	enum sway_fullscreen_mode mode = container->default_fullscreen_mode;
	// If we're explicitely being called to fullscreen always do it even
	// if the default is to do nothing.
	if (mode == FULLSCREEN_NONE || mode == FULLSCREEN_VIEW){
		mode = FULLSCREEN_WORKSPACE;
	}
	if (argc >= 1) {
		if (strcasecmp(argv[0], "global") == 0) {
			mode = FULLSCREEN_GLOBAL;
		} else {
			enable = parse_boolean(argv[0], is_fullscreen);
		}
	}

	if (argc >= 2) {
		if (strcasecmp(argv[1], "global") == 0) {
			mode = FULLSCREEN_GLOBAL;
		}
	}

	if (enable && node->type == N_WORKSPACE) {
		// Wrap the workspace's children in a container so we can fullscreen it
		container = workspace_wrap_children(workspace);
		workspace->layout = L_HORIZ;
		seat_set_focus_container(config->handler_context.seat, container);
	}

	container_set_fullscreen(container, enable ? mode : FULLSCREEN_NONE);
	arrange_root();

	return cmd_results_new(CMD_SUCCESS, NULL);
}
