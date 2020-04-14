/* plugin */

#include <stdio.h>
#include <string.h>
#include "splchk_functions.h"

GtkTextIter start_iter, end_iter;
GtkTextView *text_view = NULL;
GtkTextBuffer *splchk_local_buffer = NULL;
gchar *lang_name;

gchar *langs[5];
gchar *lang_filenames[7];

void 
splchk_load_langs (void)
{
	langs[0] = "C";
	langs[1] = "C++";
	langs[2] = "Java";
	langs[3] = "Python";
	langs[4] = "Html";
}

void 
splchk_load_lang_filenames (void)
{
	lang_filenames[0] = "C_keywords.txt";
	lang_filenames[1] = "C++_keywords.txt";
	lang_filenames[2] = "java_keywords.txt";
	lang_filenames[3] = "python_keywords.txt";
	lang_filenames[4] = "html_keywords.txt";
	lang_filenames[5] = "20k.txt";
	lang_filenames[6] = "/usr/local/bin/gedit_words_files/";
	//printf("%s", strcat(lang_filenames[6], lang_filenames[0]));
}

/* replaces the searched set of chars in the gedit text buffer with the selected word suggestion 
 * usage: gedit-word-selector.c in function gedit_word_selector_activate_selected_word()
 */

void
splchk_replace_text (gchar str[])
{
    str[strcspn(str, "\r\n")] = 0;
    if(splchk_local_buffer == NULL) { 
        return;
    }
    gtk_text_buffer_begin_user_action (splchk_local_buffer);
    gtk_text_buffer_delete (splchk_local_buffer, &start_iter, &end_iter);
    gtk_text_buffer_insert (splchk_local_buffer, &start_iter, str, -1);
	//gtk_text_view_set_editable(text_view, TRUE);
	//gtk_text_view_set_cursor_visible(text_view, TRUE);
    gtk_text_buffer_end_user_action (splchk_local_buffer);
}

/* sets a GtkTextView pointer to the gedit text view
 * usage: gedit-tab.c in function gedit-tab-grab-focus()
 */

void 
splchk_set_text_view (GtkTextView *view)
{
    text_view = view;
}

/* inserts in a local text buffer the word in which the current position of the cursor is
 * usage: gedit-word-selector.c in function gedit_word_selector_build_list()
 */

gchar *
splchk_get_string_from_buffer (void)
{
    GtkTextIter current;
    GtkTextMark *cursor;

    splchk_local_buffer = gtk_text_view_get_buffer (text_view);

    /* Get the mark at cursor. */
    cursor = gtk_text_buffer_get_mark (splchk_local_buffer, "insert");

    /* Get the iter at cursor. */
    gtk_text_buffer_get_iter_at_mark (splchk_local_buffer, &current, cursor);
    gtk_text_iter_backward_word_start (&current);
    start_iter = current;
    gtk_text_iter_forward_word_end (&current);
    end_iter = current;

    return gtk_text_buffer_get_text(splchk_local_buffer, &start_iter, &end_iter, FALSE);
}

void 
splchk_set_lang (gchar *lang)
{
    lang_name = lang;
}

gchar *
splchk_ret_lang_name (void)
{
	return lang_name;
}

gchar *
splchk_get_lang (gint i)
{
	return langs[i];
}

FILE *
splchk_open_file (gint val)
{
	gchar *filename = strcat(lang_filenames[6], lang_filenames[val]);
	FILE *fp = fopen(filename, "r");
	return fp;
}

    
/* plugin */
