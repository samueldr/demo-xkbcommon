/*
 * XKB_CONFIG_ROOT, XKB_EXTRA_PATH, XDG_CONFIG_DIR, HOME
 * https://xkbcommon.org/doc/current/group__include-path.html
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

struct xkb_keymap *keymap;
struct xkb_state *state;

struct xkb_compose_state *compose_state = NULL;
 
void error(char* msg) {
	fprintf(stderr, "error: %s\n", msg);
}

void press(int keycode, int direction) {
	xkb_keysym_t keysym;
	char keysym_name[64];
	char key_character[64];
	enum xkb_state_component changed;

	// Record the change
	changed = xkb_state_update_key(state, keycode, direction);

	// Get information
	keysym = xkb_state_key_get_one_sym(state, keycode);

	// Get the current character string
	xkb_state_key_get_utf8(state, keycode, key_character, sizeof(key_character));

	printf(":: -> key code %2x direction %1d\n", keycode, direction);

	// Maybe override keysym and current character string from compose
	if (direction == XKB_KEY_UP) {
		if (compose_state) {
			xkb_compose_state_feed(compose_state, keysym);

			int result = 0;
			result = xkb_compose_state_get_status(compose_state);
			if (result == XKB_COMPOSE_COMPOSED) {
				xkb_compose_state_get_utf8(compose_state, key_character, sizeof(key_character));
				keysym = xkb_compose_state_get_one_sym(compose_state);
			}
		}

		// Get keysym name
		xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));

		// Print info
		if (strcmp(key_character, "")) {
			printf(":: -> Char entered: [%s] key name: '%s'\n", key_character, keysym_name);
		}
	}
}

int main(int argc, char* argv[]) {
	struct xkb_context *ctx;

	ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (!ctx) {
		error("Could not create xkb_context.");
		return 1;
	}

	printf("Created xkb_context.\n");

	struct xkb_rule_names names = {
#if 1
		.layout = "ca",
#else
		// Example RMLVO for Icelandic Dvorak.
		.rules = NULL,
		.model = "pc105",
		.layout = "is",
		.variant = "dvorak",
		.options = "terminate:ctrl_alt_bksp"
#endif
	};

	keymap = xkb_keymap_new_from_names(ctx, &names,
			XKB_KEYMAP_COMPILE_NO_FLAGS);
	if (!keymap) {
		error("Could not create xkb_keymap.");
		return 1;
	}

	printf("Created keymap.\n");

	state = xkb_state_new(keymap);
	if (!state) {
		error("Could not create xkb_state");
		return 1;
	}

	printf("Created xkb_state.\n");

	const char *locale;
	locale = getenv("LC_ALL");
	if (!locale || !*locale)
		locale = getenv("LC_CTYPE");
	if (!locale || !*locale)
		locale = getenv("LANG");
	if (!locale || !*locale)
		locale = "C";


	printf("LOCALE=%s\n", locale);

	// Try setting up compose_State, but DON'T exit if it fails.
	// We can probably continue without one, though dead keys won't work.
	struct xkb_compose_table *compose_table;
	compose_table = xkb_compose_table_new_from_locale(ctx, locale, XKB_COMPOSE_COMPILE_NO_FLAGS);

	if (!compose_table) {
		error("Could not create xkb_compose_table.");
	}
	else {
		printf("Configured locale and xkb_compose_table\n");

		compose_state = xkb_compose_state_new(compose_table, XKB_COMPOSE_STATE_NO_FLAGS);
	}

#if 1
	xkb_keysym_t keysym;
	char keysym_name[64];
	char key_character[64];
	enum xkb_state_component changed;

	for (int keycode = 0; keycode <= (2<<15); keycode++) {
#	if 0
		// Randomize Shift press:
		// 0x32 == Shift_L
		changed = xkb_state_update_key(state, 0x32, (keycode%2 ? XKB_KEY_UP  : XKB_KEY_DOWN));
#	endif

		// Press the key
		changed = xkb_state_update_key(state, keycode, XKB_KEY_DOWN);

		// Get information
		keysym = xkb_state_key_get_one_sym(state, keycode);

		xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
		xkb_state_key_get_utf8(state, keycode, key_character, sizeof(key_character));

		if (strcmp("NoSymbol", keysym_name)) {
			printf("Key code: = %02x -> '%1s' -> '%s'\n", keycode, key_character, keysym_name);
		}

		// Stop pressing that key!
		changed = xkb_state_update_key(state, keycode, XKB_KEY_UP);
	}
	// Reset caps lock :/
	changed = xkb_state_update_key(state, 0x42, XKB_KEY_DOWN);
	changed = xkb_state_update_key(state, 0x42, XKB_KEY_UP);
#endif

	printf("\n\nTest: Noël\n");
	// layout: ca; Noël
	press(0x3E, XKB_KEY_DOWN); /* Shift_R      */
	press(0x39, XKB_KEY_DOWN); /* N            */
	press(0x39, XKB_KEY_UP);   /* N            */
	press(0x3E, XKB_KEY_UP);   /* SHIFT_R      */
	press(0x20, XKB_KEY_DOWN); /* O            */
	press(0x20, XKB_KEY_UP);   /* O            */
	press(0x32, XKB_KEY_DOWN); /* Shift_L      */
	press(0x23, XKB_KEY_DOWN); /* dead_cedilla */
	press(0x23, XKB_KEY_UP);   /* dead_cedilla */
	press(0x32, XKB_KEY_UP);   /* Shift_L      */
	press(0x1A, XKB_KEY_DOWN); /* E            */
	press(0x1A, XKB_KEY_UP);   /* E            */
	press(0x2E, XKB_KEY_DOWN); /* L            */
	press(0x2E, XKB_KEY_UP);   /* L            */

	printf("\n\nTest: samueldr\n");
	// layout: ca: @samueldr
	press(0x6C, XKB_KEY_DOWN); /* Shift_R      */
	press(0x0b, XKB_KEY_DOWN); /* N            */
	press(0x0b, XKB_KEY_UP);   /* N            */
	press(0x6C, XKB_KEY_UP);   /* SHIFT_R      */

	press(0x27, XKB_KEY_DOWN); /* S            */
	press(0x27, XKB_KEY_UP);   /* S            */
	press(0x26, XKB_KEY_DOWN); /* A            */
	press(0x26, XKB_KEY_UP);   /* A            */
	press(0x3A, XKB_KEY_DOWN); /* M            */
	press(0x3A, XKB_KEY_UP);   /* M            */
	press(0x1E, XKB_KEY_DOWN); /* U            */
	press(0x1E, XKB_KEY_UP);   /* U            */
	press(0x1A, XKB_KEY_DOWN); /* E            */
	press(0x1A, XKB_KEY_UP);   /* E            */
	press(0x2E, XKB_KEY_DOWN); /* L            */
	press(0x2E, XKB_KEY_UP);   /* L            */
	press(0x28, XKB_KEY_DOWN); /* D            */
	press(0x28, XKB_KEY_UP);   /* D            */
	press(0x1B, XKB_KEY_DOWN); /* R            */
	press(0x1B, XKB_KEY_UP);   /* R            */
}
