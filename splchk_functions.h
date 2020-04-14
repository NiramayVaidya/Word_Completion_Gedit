/* plugin */

#ifndef SPLCHK_FUNCTIONS_H
#define SPLCHK_FUNCTIONS_H

#include <gtk/gtk.h>
#include "gedit-word-selector.h"

G_BEGIN_DECLS

void		 splchk_replace_text					(gchar 				str[]);

void		 splchk_set_text_view					(GtkTextView 		*view);

gchar		 *splchk_get_string_from_buffer			(void);

void		 splchk_set_lang						(gchar 				*lang);

gchar 		 *splchk_ret_lang_name					(void);

gchar		 *splchk_get_lang						(gint i);

FILE 		 *splchk_open_file	(gint val);

void 		 splchk_load_langs						(void);

void 		 splchk_load_lang_filenames				(void);

G_END_DECLS

#endif /* SPLCHK_FUNCTIONS_H */

/* plugin */
