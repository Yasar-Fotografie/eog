/* Eye Of Gnome - Image  Dialog
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "eog-dialog.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

enum {
        PROP_0,
        PROP_PARENT_WINDOW,
};

struct _EogDialogPrivate {
	GtkWidget   *dlg;
	GtkBuilder  *xml;
	GtkWindow   *parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (EogDialog, eog_dialog, G_TYPE_OBJECT)

static void
eog_dialog_construct_impl (EogDialog   *dialog,
			   const gchar *ui_file,
			   const gchar *dlg_node)
{
	EogDialogPrivate *priv;
	gchar *filename;

	g_return_if_fail (dialog != NULL);
	g_return_if_fail (EOG_IS_DIALOG (dialog));

	priv = dialog->priv;

	filename = g_build_filename (EOG_DATA_DIR, ui_file, NULL);

	priv->xml = gtk_builder_new ();
	gtk_builder_set_translation_domain (priv->xml, GETTEXT_PACKAGE);
	g_assert (gtk_builder_add_from_file (priv->xml, filename, NULL));

	g_free (filename);

	priv->dlg = GTK_WIDGET (gtk_builder_get_object (priv->xml, dlg_node));

	if (priv->parent != NULL) {
		gtk_window_set_transient_for (GTK_WINDOW (priv->dlg),
					      priv->parent);
	}
}

static void
eog_dialog_show_impl (EogDialog *dialog)
{
	g_return_if_fail (dialog != NULL);
	g_return_if_fail (EOG_IS_DIALOG (dialog));

	gtk_window_present (GTK_WINDOW (dialog->priv->dlg));
}

static void
eog_dialog_hide_impl (EogDialog *dialog)
{
	g_return_if_fail (dialog != NULL);
	g_return_if_fail (EOG_IS_DIALOG (dialog));

	gtk_widget_hide (dialog->priv->dlg);
}

static void
eog_dialog_set_property (GObject      *object,
			 guint         prop_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
	EogDialog *dialog = EOG_DIALOG (object);

	switch (prop_id) {
		case PROP_PARENT_WINDOW:
			dialog->priv->parent = g_value_get_object (value);
			break;
	}
}

static void
eog_dialog_get_property (GObject    *object,
			 guint       prop_id,
			 GValue     *value,
			 GParamSpec *pspec)
{
	EogDialog *dialog = EOG_DIALOG (object);

	switch (prop_id) {
		case PROP_PARENT_WINDOW:
			g_value_set_object (value, dialog->priv->parent);
			break;
	}
}

static void
eog_dialog_dispose (GObject *object)
{
	EogDialog *dialog;
	EogDialogPrivate *priv;

	g_return_if_fail (object != NULL);
	g_return_if_fail (EOG_IS_DIALOG (object));

	dialog = EOG_DIALOG (object);
	priv = dialog->priv;

	if (priv->dlg) {
		gtk_widget_destroy (priv->dlg);
		priv->dlg = NULL;
	}

	if (priv->xml) {
		g_object_unref (priv->xml);
		priv->xml = NULL;
	}

	G_OBJECT_CLASS (eog_dialog_parent_class)->dispose (object);
}

static void
eog_dialog_class_init (EogDialogClass *class)
{
	GObjectClass *g_object_class = (GObjectClass *) class;

	g_object_class->dispose = eog_dialog_dispose;
	g_object_class->set_property = eog_dialog_set_property;
	g_object_class->get_property = eog_dialog_get_property;

	class->construct = eog_dialog_construct_impl;
	class->show = eog_dialog_show_impl;
	class->hide = eog_dialog_hide_impl;

	g_object_class_install_property (g_object_class,
					 PROP_PARENT_WINDOW,
					 g_param_spec_object ("parent-window",
							      "Parent window",
							      "Parent window",
							      GTK_TYPE_WINDOW,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_NAME |
							      G_PARAM_STATIC_NICK |
							      G_PARAM_STATIC_BLURB));
}

static void
eog_dialog_init (EogDialog *dialog)
{
	dialog->priv = eog_dialog_get_instance_private (dialog);

	dialog->priv->dlg = NULL;
	dialog->priv->xml = NULL;
	dialog->priv->parent = NULL;
}

void
eog_dialog_construct (EogDialog   *dialog,
		      const gchar *ui_file,
		      const gchar *dlg_node)
{
	EogDialogClass *klass = EOG_DIALOG_GET_CLASS (dialog);
	klass->construct (dialog, ui_file, dlg_node);
}

void
eog_dialog_show (EogDialog *dialog)
{
	EogDialogClass *klass = EOG_DIALOG_GET_CLASS (dialog);
	klass->show (dialog);
}

void
eog_dialog_hide (EogDialog *dialog)
{
	EogDialogClass *klass = EOG_DIALOG_GET_CLASS (dialog);
	klass->hide (dialog);
}

void
eog_dialog_get_controls (EogDialog   *dialog,
			 const gchar *property_id,
			 ...)
{
	EogDialogPrivate *priv;
        GtkWidget **wptr;
        va_list varargs;

	g_return_if_fail (dialog != NULL);
	g_return_if_fail (EOG_IS_DIALOG (dialog));

	priv = dialog->priv;

        va_start (varargs, property_id);

        while (property_id != NULL)
        {
                wptr = va_arg (varargs, GtkWidget **);
                *wptr = GTK_WIDGET (gtk_builder_get_object (priv->xml,
							    property_id));

                property_id = va_arg (varargs, const gchar *);
        }

        va_end (varargs);
}
