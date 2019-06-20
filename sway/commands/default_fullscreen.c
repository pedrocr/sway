#include <strings.h>
#include "log.h"
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/view.h"
#include "sway/tree/workspace.h"
#include "util.h"

// default_fullscreen [none|workspace|global]
struct cmd_results *cmd_default_fullscreen(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "default_fullscreen", EXPECTED_EQUAL_TO, 1))) {
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

	if (strcasecmp(argv[0], "view") == 0) {
		container->default_fullscreen_mode = FULLSCREEN_VIEW;
	} else if (strcasecmp(argv[0], "workspace") == 0) {
		container->default_fullscreen_mode = FULLSCREEN_WORKSPACE;
	} else if (strcasecmp(argv[0], "global") == 0) {
		container->default_fullscreen_mode = FULLSCREEN_GLOBAL;
	} else {
		return cmd_results_new(CMD_FAILURE,
				"Unknown fullscreen mode");
	}

	return cmd_results_new(CMD_SUCCESS, NULL);
}
