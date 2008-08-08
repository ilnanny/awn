/*
 *  Copyright (C) 2008 Neil Jagdish Patel <njpatel@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 *  Author : Neil Jagdish Patel <njpatel@gmail.com>
 *
 */

#include "config.h"

#include "awn-applet-manager.h"

G_DEFINE_TYPE (AwnAppletManager, awn_applet_manager, GTK_TYPE_BOX) 

#define AWN_APPLET_MANAGER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, \
  AWN_TYPE_APPLET_MANAGER, AwnAppletManagerPrivate))

struct _AwnAppletManagerPrivate
{
  AwnConfigClient *client;

  AwnOrientation orient;

  GtkWidgetClass *klass;
};

enum 
{
  PROP_0,

  PROP_CLIENT,
  PROP_ORIENT
};


/* 
 * FORWARDS
 */
static void awn_applet_manager_set_orient (AwnAppletManager *manager, 
                                           gint              orient);

/*
 * GOBJECT CODE 
 */
static void
awn_applet_manager_constructed (GObject *object)
{
  AwnAppletManagerPrivate *priv;
  
  priv = AWN_APPLET_MANAGER_GET_PRIVATE (object); 

  gtk_box_pack_start (GTK_BOX (object), gtk_label_new ("Applet 1"),
                      0, FALSE, FALSE);
  gtk_box_pack_start (GTK_BOX (object), gtk_label_new ("Applet 2"),
                      0, FALSE, FALSE);
  
}

static void
awn_applet_manager_size_request (GtkWidget *widget, GtkRequisition *requisition){
  AwnAppletManagerPrivate *priv = AWN_APPLET_MANAGER (widget)->priv;
  
  priv->klass->size_request (widget, requisition);
}

static void
awn_applet_manager_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  AwnAppletManagerPrivate *priv = AWN_APPLET_MANAGER (widget)->priv;
  
  priv->klass->size_allocate (widget, allocation);
}

static void
awn_applet_manager_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  AwnAppletManagerPrivate *priv;

  g_return_if_fail (AWN_IS_APPLET_MANAGER (object));
  priv = AWN_APPLET_MANAGER (object)->priv;

  switch (prop_id)
  {
    case PROP_CLIENT:
      g_value_set_pointer (value, priv->client);
      break;
    case PROP_ORIENT:
      g_value_set_int (value, priv->orient);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
awn_applet_manager_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  AwnAppletManager *manager = AWN_APPLET_MANAGER (object);
  AwnAppletManagerPrivate *priv;

  g_return_if_fail (AWN_IS_APPLET_MANAGER (object));
  priv = AWN_APPLET_MANAGER (object)->priv;

  switch (prop_id)
  {
    case PROP_CLIENT:
      priv->client =  g_value_get_pointer (value);
      break;
    case PROP_ORIENT:
      awn_applet_manager_set_orient (manager, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}


static void
awn_applet_manager_dispose (GObject *object)
{
  G_OBJECT_CLASS (awn_applet_manager_parent_class)->dispose (object);
}

static void
awn_applet_manager_class_init (AwnAppletManagerClass *klass)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *wid_class = GTK_WIDGET_CLASS (klass);
  
  obj_class->constructed   = awn_applet_manager_constructed;
  obj_class->dispose       = awn_applet_manager_dispose;
  obj_class->get_property  = awn_applet_manager_get_property;
  obj_class->set_property  = awn_applet_manager_set_property;

  wid_class->size_request  = awn_applet_manager_size_request;
  wid_class->size_allocate = awn_applet_manager_size_allocate;
    
  /* Add properties to the class */
  g_object_class_install_property (obj_class,
    PROP_CLIENT,
    g_param_spec_pointer ("client",
                          "Client",
                          "The AwnConfigClient",
                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (obj_class,
    PROP_ORIENT,
    g_param_spec_int ("orient",
                      "Orient",
                      "The orientation of the panel",
                      0, 3, AWN_ORIENT_BOTTOM,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
 
  g_type_class_add_private (obj_class, sizeof (AwnAppletManagerPrivate));
}

static void
awn_applet_manager_init (AwnAppletManager *manager)
{
  AwnAppletManagerPrivate *priv;

  priv = manager->priv = AWN_APPLET_MANAGER_GET_PRIVATE (manager);
}

GtkWidget *
awn_applet_manager_new_from_config (AwnConfigClient *client)
{
  GtkWidget *manager;
  
  manager = g_object_new (AWN_TYPE_APPLET_MANAGER,
                         "homogeneous", FALSE,
                         "spacing", 0,
                         "client", client,
                         NULL);
  return manager;
}

/*
 * PROPERTY SETTERS
 */

/*
 * Update the box class
 */
static void 
awn_applet_manager_set_orient (AwnAppletManager *manager, 
                               gint              orient)
{
  AwnAppletManagerPrivate *priv = manager->priv;

  priv->orient = orient;

  switch (priv->orient)
  {
    case AWN_ORIENT_TOP:
    case AWN_ORIENT_BOTTOM:
      priv->klass = GTK_WIDGET_CLASS (gtk_type_class (GTK_TYPE_HBOX));
      break;
    
    case AWN_ORIENT_RIGHT:
    case AWN_ORIENT_LEFT:
      priv->klass = GTK_WIDGET_CLASS (gtk_type_class (GTK_TYPE_VBOX));
      break;

    default:
      g_assert_not_reached ();
      priv->klass = NULL;
      break;
  }
}
