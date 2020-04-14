/* plugin */

#include "gedit-word-selector.h"

#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include <string.h>
#include <stdio.h>

#define COLUMN_NAME 0

struct _GeditWordSelector
{
	GtkGrid parent_instance;

	GtkWidget *treeview;
	GtkWidget *entry;
	GtkListStore *liststore;
	GtkTreeModelFilter *treemodelfilter;
	GtkTreeSelection *treeview_selection;
};

G_DEFINE_TYPE (GeditWordSelector, gedit_word_selector, GTK_TYPE_GRID)

static void
gedit_word_selector_class_init (GeditWordSelectorClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	/* Bind class to template */
	gtk_widget_class_set_template_from_resource (widget_class,
	                                             "/org/gnome/gedit/ui/gedit-word-selector.ui");
	gtk_widget_class_bind_template_child (widget_class, GeditWordSelector, treeview);
	gtk_widget_class_bind_template_child (widget_class, GeditWordSelector, entry);
	gtk_widget_class_bind_template_child (widget_class, GeditWordSelector, liststore);
	gtk_widget_class_bind_template_child (widget_class, GeditWordSelector, treemodelfilter);
	gtk_widget_class_bind_template_child (widget_class, GeditWordSelector, treeview_selection);
}

static gboolean
visible_func (GtkTreeModel               *model,
              GtkTreeIter                *iter,
              GeditWordSelector *selector)
{
	const gchar *entry_text;
	gchar *name;
	gchar *name_normalized;
	gchar *name_casefolded;
	gchar *text_normalized;
	gchar *text_casefolded;
	gboolean visible = FALSE;

	entry_text = gtk_entry_get_text (GTK_ENTRY (selector->entry));

	if (*entry_text == '\0')
	{
		return TRUE;
	}

	gtk_tree_model_get (model, iter, COLUMN_NAME, &name, -1);

	name_normalized = g_utf8_normalize (name, -1, G_NORMALIZE_ALL);
	g_free (name);

	name_casefolded = g_utf8_casefold (name_normalized, -1);
	g_free (name_normalized);

	text_normalized = g_utf8_normalize (entry_text, -1, G_NORMALIZE_ALL);
	text_casefolded = g_utf8_casefold (text_normalized, -1);
	g_free (text_normalized);

	if (strstr (name_casefolded, text_casefolded) != NULL)
	{
		visible = TRUE;
	}

	g_free (name_casefolded);
	g_free (text_casefolded);

	return visible;
}

static void
on_entry_activate (GtkEntry                   *entry,
                   GeditWordSelector *selector)
{
	gedit_word_selector_activate_selected_word (selector);
}

static void
on_entry_changed (GtkEntry                   *entry,
                  GeditWordSelector *selector)
{
	GtkTreeIter iter;

	gtk_tree_model_filter_refilter (selector->treemodelfilter);

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (selector->treemodelfilter), &iter))
	{
		gtk_tree_selection_select_iter (selector->treeview_selection, &iter);
	}
}

static gboolean
move_selection (GeditWordSelector *selector,
                gint                      howmany)
{
	GtkTreeIter iter;
	GtkTreePath *path;
	gint *indices;
	gint ret = FALSE;

	if (!gtk_tree_selection_get_selected (selector->treeview_selection, NULL, &iter) &&
	    !gtk_tree_model_get_iter_first (GTK_TREE_MODEL (selector->treemodelfilter), &iter))
	{
		return FALSE;
	}

	path = gtk_tree_model_get_path (GTK_TREE_MODEL (selector->treemodelfilter), &iter);
	indices = gtk_tree_path_get_indices (path);

	if (indices)
	{
		gint num;
		gint idx;
		GtkTreePath *new_path;

		idx = indices[0];
		num = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (selector->treemodelfilter), NULL);

		if ((idx + howmany) < 0)
		{
			idx = 0;
		}
		else if ((idx + howmany) >= num)
		{
			idx = num - 1;
		}
		else
		{
			idx = idx + howmany;
		}

		new_path = gtk_tree_path_new_from_indices (idx, -1);
		gtk_tree_selection_select_path (selector->treeview_selection, new_path);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (selector->treeview),
		                              new_path, NULL, TRUE, 0.5, 0);
		gtk_tree_path_free (new_path);

		ret = TRUE;
	}

	gtk_tree_path_free (path);

	return ret;
}

static gboolean
on_entry_key_press_event (GtkWidget                  *entry,
                          GdkEventKey                *event,
                          GeditWordSelector *selector)
{
	if (event->keyval == GDK_KEY_Down)
	{
		return move_selection (selector, 1);
	}
	else if (event->keyval == GDK_KEY_Up)
	{
		return move_selection (selector, -1);
	}
	else if (event->keyval == GDK_KEY_Page_Down)
	{
		return move_selection (selector, 5);
	}
	else if (event->keyval == GDK_KEY_Page_Up)
	{
		return move_selection (selector, -5);
	}

	return FALSE;
}

static void
on_row_activated (GtkTreeView                *tree_view,
                  GtkTreePath                *path,
                  GtkTreeViewColumn          *column,
                  GeditWordSelector *selector)
{
	gedit_word_selector_activate_selected_word (selector);
}

static void
gedit_word_selector_init (GeditWordSelector *selector)
{
	GtkTreeIter iter;

	gtk_widget_init_template (GTK_WIDGET (selector));

	gtk_tree_model_filter_set_visible_func (selector->treemodelfilter,
	                                        (GtkTreeModelFilterVisibleFunc)visible_func,
	                                        selector,
	                                        NULL);

	g_signal_connect (selector->entry, "activate",
	                  G_CALLBACK (on_entry_activate), selector);
	g_signal_connect (selector->entry, "changed",
	                  G_CALLBACK (on_entry_changed), selector);
	g_signal_connect (selector->entry, "key-press-event",
	                  G_CALLBACK (on_entry_key_press_event), selector);

	g_signal_connect (selector->treeview, "row-activated",
	                  G_CALLBACK (on_row_activated), selector);

	/* Populate tree model */
	gtk_list_store_append (selector->liststore, &iter);
	gtk_list_store_set (selector->liststore, &iter,
	                    COLUMN_NAME, NULL, -1);
}

GeditWordSelector *
gedit_word_selector_new ()
{
	return g_object_new (GEDIT_TYPE_WORD_SELECTOR, NULL);
}

/* adds a word to a list
 * usage: gedit-word-selectorin function gedit_word_selector_build_list()
 */

void
gedit_word_selector_add(GtkListStore *store, const gchar *str) 
{
	GtkTreeIter iter;
 
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, COLUMN_NAME, str, -1);
}

/* adds all matching words for the set of characters searched to a list
 * usage: gedit-window.c in function on_popover_shown() and on_shortcut_pressed_popover_shown()
 */
        
void
gedit_word_selector_build_list (GeditWordSelector *selector)
{
    GtkTreeIter iter;
    
    /* splchk_get_string_from_buffer() declared and defined in splchk_funcions.h and splchk_funcions.c */
    
    gchar *chars_typed = splchk_get_string_from_buffer ();
    
    //selector = gedit_word_selector_get_instance_private (selector);
    gtk_list_store_clear (selector->liststore);
  
	gint i; 
	gint flag = 0;
	gchar *lang;
	gchar *word;
	FILE *fp;
	gchar *lang_name = splchk_ret_lang_name();
	for(i = 0; i < 5; i++) {
		lang = splchk_get_lang(i);
		if(!strcmp(lang_name, lang)) {
			fp = splchk_open_file(i);
			while(fscanf(fp, "%s", word) != EOF) {
				if(strstr(word, chars_typed) == word) {
					gedit_word_selector_add(selector->liststore, word);
				}
			}
			fclose(fp);
			flag = 1;
			break;
		}
	}
	if(!flag) {
		fp = splchk_open_file(i);
		while(fscanf(fp, "%s", word) != EOF) {
			if(strstr(word, chars_typed) == word) {
				gedit_word_selector_add(selector->liststore, word);
			}
		fclose(fp);
		}
	}
 
    /* select first item */
    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (selector->treemodelfilter), &iter))
    {
        gtk_tree_selection_select_iter (selector->treeview_selection, &iter);
    }
}

void
gedit_word_selector_activate_selected_word (GeditWordSelector *selector)
{
	gchar *word;
	GtkTreeIter iter;

	g_return_if_fail (GEDIT_IS_WORD_SELECTOR (selector));

	if (!gtk_tree_selection_get_selected (selector->treeview_selection, NULL, &iter))
	{
		return;
	}

	gtk_tree_model_get (GTK_TREE_MODEL (selector->treemodelfilter), &iter,
	                    COLUMN_NAME, &word,
	                    -1);
        
        /* splchk_replace_text() declared and defined in splchk_functions.h and splchk_funcions.c */
        
        splchk_replace_text(word);
        gtk_widget_hide(GTK_WIDGET(selector));
        
	if (word != NULL)
	{
		g_free (word);
	}
}

/* plugin */
