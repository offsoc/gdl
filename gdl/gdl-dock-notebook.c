/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "gdl-tools.h"
#include "gdl-dock.h"
#include "gdl-dock-notebook.h"
#include "gdl-dock-tablabel.h"


/* Private prototypes */

static void  gdl_dock_notebook_class_init    (GdlDockNotebookClass *klass);
static void  gdl_dock_notebook_init          (GdlDockNotebook *notebook);

static void  gdl_dock_notebook_add           (GtkContainer *container,
					      GtkWidget    *widget);
static void  gdl_dock_notebook_remove        (GtkContainer *container,
					      GtkWidget    *widget);
static void  gdl_dock_notebook_forall        (GtkContainer *container,
					      gboolean      include_internals,
					      GtkCallback   callback,
					      gpointer      callback_data);

static void  gdl_dock_notebook_auto_reduce   (GdlDockItem *item);

static void  gdl_dock_notebook_set_orientation (GdlDockItem    *item,
					        GtkOrientation  orientation);
					       
static void  gdl_dock_notebook_layout_save   (GdlDockItem *item,
                                              xmlNodePtr   node);
                                              
static void  gdl_dock_notebook_layout_save_foreach (GtkWidget *widget,
                                                    gpointer   data);

static void  gdl_dock_notebook_hide          (GdlDockItem *item);

static void  gdl_dock_notebook_hide_foreach  (GtkWidget *widget,
                                              gpointer   data);

static gchar *gdl_dock_notebook_get_pos_hint (GdlDockItem      *item,
                                              GdlDockItem      *caller,
                                              GdlDockPlacement *position);

/* Class variables and definitions */

static GdlDockItemClass *parent_class = NULL;


/* Private functions */

static void
gdl_dock_notebook_class_init (GdlDockNotebookClass *klass)
{
    GtkObjectClass    *object_class;
    GtkWidgetClass    *widget_class;
    GtkContainerClass *container_class;
    GdlDockItemClass  *item_class;

    object_class = (GtkObjectClass *) klass;
    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;
    item_class = (GdlDockItemClass *) klass;

    parent_class = gtk_type_class (gdl_dock_item_get_type ());

    container_class->add = gdl_dock_notebook_add;
    container_class->remove = gdl_dock_notebook_remove;
    container_class->forall = gdl_dock_notebook_forall;

    item_class->auto_reduce = gdl_dock_notebook_auto_reduce;
    item_class->set_orientation = gdl_dock_notebook_set_orientation;    
    item_class->save_layout = gdl_dock_notebook_layout_save;
    item_class->item_hide = gdl_dock_notebook_hide;
    item_class->get_pos_hint = gdl_dock_notebook_get_pos_hint;
}

static void
gdl_dock_notebook_init (GdlDockNotebook *notebook)
{
    notebook->notebook = NULL;
}

static void
gdl_dock_notebook_add (GtkContainer *container,
		       GtkWidget    *widget)
{
    GdlDockNotebook *notebook;
    GdlDockItem     *item;
    GtkWidget       *label;

    g_return_if_fail (container != NULL);
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GDL_IS_DOCK_NOTEBOOK (container));
    g_return_if_fail (GDL_IS_DOCK_ITEM (widget));

    notebook = GDL_DOCK_NOTEBOOK (container);
    item = GDL_DOCK_ITEM (widget);

    GDL_DOCK_ITEM_CHECK_AND_BIND (item, notebook);
    if (GDL_DOCK_ITEM_IS_FLOATING (item))
        gdl_dock_item_window_sink (item);

    label = gdl_dock_item_get_tablabel (item);
    if (!label) {
        if (item->long_name)
            label = gtk_label_new (item->long_name);
        else
            label = gtk_label_new (item->name);
    } else if (GDL_IS_DOCK_TABLABEL (label)) {
        gdl_dock_tablabel_deactivate (GDL_DOCK_TABLABEL (label));
        /* hide the item drag handle, as we will use the tablabel's */
        gdl_dock_item_hide_handle (item);
    };

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook->notebook), 
    			      widget, label);
}

static void
gdl_dock_notebook_remove (GtkContainer *container,
			  GtkWidget    *widget)
{
    GdlDockNotebook *notebook;
    GdlDockItem *item;
    guint pagenr;

    g_return_if_fail (container != NULL);
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GDL_IS_DOCK_NOTEBOOK (container));
    g_return_if_fail (GDL_IS_DOCK_ITEM (widget) || 
                      widget == GDL_DOCK_NOTEBOOK (container)->notebook);
    
    notebook = GDL_DOCK_NOTEBOOK (container);
    if (widget == notebook->notebook)
        gtk_widget_unparent (widget);
    else {
        item = GDL_DOCK_ITEM (widget);

        pagenr = gtk_notebook_page_num (GTK_NOTEBOOK (notebook->notebook), 
                                        widget);
        gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), pagenr);
    };
}

static void
gdl_dock_notebook_forall (GtkContainer *container,
			  gboolean      include_internals,
			  GtkCallback   callback,
			  gpointer      callback_data)
{
    GdlDockNotebook *notebook;

    g_return_if_fail (container != NULL);
    g_return_if_fail (GDL_IS_DOCK_NOTEBOOK (container));
    g_return_if_fail (callback != NULL);

    notebook = GDL_DOCK_NOTEBOOK (container);

    (*callback) (notebook->notebook, callback_data);
}

static void
gdl_dock_notebook_auto_reduce (GdlDockItem *item)
{
    GtkWidget       *child, *parent;
    GdlDockNotebook *nb;
    GList           *children;

    g_return_if_fail (item != NULL);
    g_return_if_fail (GDL_IS_DOCK_NOTEBOOK (item));

    parent = GTK_WIDGET (item)->parent;
    nb = GDL_DOCK_NOTEBOOK (item);

    children = gtk_container_children (GTK_CONTAINER (nb->notebook));
    /* Check if only one item remains in the notebook. */
    if (g_list_length (children) <= 1) {
        child = gtk_notebook_get_nth_page (
            GTK_NOTEBOOK (GDL_DOCK_NOTEBOOK (item)->notebook), 0);
            
        /* FIXME: make the child inherit floating state/size (?) from
           the notebook */
        /* Remove widget from notebook, remove the notebook from it's parent
           and add the widget directly to the notebook's parent. */
        gtk_widget_ref (child);
        gtk_notebook_remove_page (GTK_NOTEBOOK (nb->notebook), 0);
        gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (item));
        gtk_container_add (GTK_CONTAINER (parent), child);
        gtk_widget_unref (child);
    }
    g_list_free (children);
}

static void
gdl_dock_notebook_set_orientation (GdlDockItem    *item,
				   GtkOrientation  orientation)
{
    /* FIXME */
}

static void
gdl_dock_notebook_switch_page (GtkNotebook     *nb,
                               GtkNotebookPage *page,
                               gint             page_num,
                               gpointer         data)
{
    GdlDockNotebook *notebook;
    GtkWidget       *tablabel;

    notebook = GDL_DOCK_NOTEBOOK (data);

    /* deactivate old tablabel */
    if (nb->cur_page) {
        tablabel = nb->cur_page->tab_label;
        if (tablabel && GDL_IS_DOCK_TABLABEL (tablabel))
            gdl_dock_tablabel_deactivate (GDL_DOCK_TABLABEL (tablabel));
    };

    /* activate new label */
    tablabel = page->tab_label;
    if (tablabel && GDL_IS_DOCK_TABLABEL (tablabel))
        gdl_dock_tablabel_activate (GDL_DOCK_TABLABEL (tablabel));
}

static void
gdl_dock_notebook_layout_save (GdlDockItem *item,
                               xmlNodePtr   node)
{
    xmlNodePtr nb_node;
    gchar buffer[8];
        
    g_return_if_fail (item != NULL);
    g_return_if_fail (GDL_IS_DOCK_NOTEBOOK (item));
    
    /* Create "notebook" node. */
    nb_node = xmlNewChild (node, NULL, "notebook", NULL);
    sprintf (buffer, "%i", gtk_notebook_get_current_page (GTK_NOTEBOOK 
            (GDL_DOCK_NOTEBOOK (item)->notebook)));
    xmlSetProp (nb_node, "index", buffer);
    
    /* Add children. */
    gtk_container_foreach (GTK_CONTAINER (
        GDL_DOCK_NOTEBOOK (item)->notebook), 
        gdl_dock_notebook_layout_save_foreach,
        (gpointer) nb_node);
}

static void
gdl_dock_notebook_layout_save_foreach (GtkWidget *widget,
                                       gpointer   data)
{
    g_return_if_fail (GDL_IS_DOCK_ITEM (widget));
    
    gdl_dock_item_save_layout (GDL_DOCK_ITEM (widget), (xmlNodePtr) data);
}

static void
gdl_dock_notebook_hide (GdlDockItem *item)
{
    GtkWidget *parent, *real_parent;

    real_parent = GTK_WIDGET (item)->parent;
    GDL_DOCK_ITEM_GET_PARENT (item, parent);
    
    /* Unfloat item. */
    if (GDL_DOCK_ITEM_IS_FLOATING (item))
        gdl_dock_item_window_sink (item);

    /* Remove children. */
    gtk_container_foreach (GTK_CONTAINER (
        GDL_DOCK_NOTEBOOK (item)->notebook), 
                           gdl_dock_notebook_hide_foreach,
                           (gpointer) item);
        
    if (real_parent)
        /* Remove item. */
        gtk_container_remove (GTK_CONTAINER (real_parent),
                              GTK_WIDGET (item));
    
    /* Auto reduce parent. */
    if (parent && GDL_IS_DOCK_ITEM (parent))
        gdl_dock_item_auto_reduce (GDL_DOCK_ITEM (parent));
}

static void
gdl_dock_notebook_hide_foreach (GtkWidget *widget,
                                gpointer   data)
{
    g_return_if_fail (GDL_IS_DOCK_ITEM (widget));
    
    gdl_dock_item_hide (GDL_DOCK_ITEM (widget));
}

static gchar *
gdl_dock_notebook_get_pos_hint (GdlDockItem      *item,
                                GdlDockItem      *caller,
                                GdlDockPlacement *position)
{
    GdlDockNotebook  *notebook;
    GList            *pages, *l;
    gchar            *ret_val = NULL;
    GdlDockItem      *child;
    GdlDockPlacement  place;

    g_return_val_if_fail (item != NULL, NULL);

    notebook = GDL_DOCK_NOTEBOOK (item);
    l = pages = gtk_container_children (GTK_CONTAINER (notebook->notebook));

    if (caller) {
        gboolean          caller_found = FALSE;

        while (l && !(ret_val && caller_found)) {
            /* find caller among children and a peer with name */
            child = GDL_DOCK_ITEM (l->data);
            if (child == caller)
                caller_found = TRUE;
            else if (!ret_val) 
                ret_val = gdl_dock_item_get_pos_hint (child, NULL, &place);
            
            l = l->next;
        };

        if (caller_found)
            *position = GDL_DOCK_CENTER;
        else
            g_warning (_("gdl_dock_notebook_get_pos_hint called with a caller "
                         "not among notebook's children"));

    } else {
        if (item->name)
            ret_val = g_strdup (item->name);
        else {
            /* traverse children looking for a named item */
            while (l && !ret_val) {
                child = GDL_DOCK_ITEM (l->data);
                ret_val = gdl_dock_item_get_pos_hint (child, NULL, &place);
                l = l->next;
            };
        };
    };

    g_list_free (pages);

    return ret_val;
}


/* Public interface */

GtkWidget *
gdl_dock_notebook_new (void)
{
    GdlDockNotebook *notebook;
    GtkBin          *bin;

    notebook = GDL_DOCK_NOTEBOOK 
	(gtk_type_new (gdl_dock_notebook_get_type ()));
    bin = GTK_BIN (notebook);

    /* create the container notebook */
    bin->child = notebook->notebook = gtk_notebook_new ();
    gtk_widget_set_parent (notebook->notebook, GTK_WIDGET (notebook));
    gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook->notebook), TRUE);
    gtk_signal_connect (GTK_OBJECT (notebook->notebook), "switch_page",
                        (GtkSignalFunc) gdl_dock_notebook_switch_page, 
                        (gpointer) notebook);
    gtk_widget_show (notebook->notebook);

    return GTK_WIDGET (notebook);
}

guint
gdl_dock_notebook_get_type (void)
{
    static GtkType dock_notebook_type = 0;

    if (dock_notebook_type == 0) {
        GtkTypeInfo dock_notebook_info = {
            "GdlDockNotebook",
            sizeof (GdlDockNotebook),
            sizeof (GdlDockNotebookClass),
            (GtkClassInitFunc) gdl_dock_notebook_class_init,
            (GtkObjectInitFunc) gdl_dock_notebook_init,
            /* reserved_1 */ NULL,
            /* reserved_2 */ NULL,
            (GtkClassInitFunc) NULL,
        };

        dock_notebook_type = gtk_type_unique (gdl_dock_item_get_type (), 
					      &dock_notebook_info);
    }

    return dock_notebook_type;
}
