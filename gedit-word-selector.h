/* plugin */

#ifndef GEDIT_WORD_SELECTOR_H
#define GEDIT_WORD_SELECTOR_H

#include <glib-object.h>
#include "gedit-window.h"

G_BEGIN_DECLS

#define GEDIT_TYPE_WORD_SELECTOR (gedit_word_selector_get_type ())

G_DECLARE_FINAL_TYPE (GeditWordSelector, gedit_word_selector, GEDIT, WORD_SELECTOR, GtkGrid)

GeditWordSelector *gedit_word_selector_new             (void);

void                        gedit_word_selector_activate_selected_word
                                                                          (GeditWordSelector *selector);
                                                                          
void gedit_word_selector_add(GtkListStore *store, const gchar *str);

void gedit_word_selector_build_list(GeditWordSelector *selector);

GtkListStore *gedit_word_selector_get_liststore(GeditWordSelector *selector);

G_END_DECLS

#endif /* GEDIT_WORD_SELECTOR_H */

/* plugin */
