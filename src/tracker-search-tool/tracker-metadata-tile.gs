[indent=4]

/*
 * Copyright (C) 2009, Jamie McCracken (jamiecc at gnome org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


uses
    Gtk
    Cairo
    Pango
    TrackerUtils


class TrackerMetadataTile : EventBox
    uri : string
    category : Categories
    image : Image
    name_link : LinkButton
    path_link : LinkButton
    table : Table
    sparql : string


    /* metadata fields */
    name_label : Label
    path_label : Label
    
    info_label1 : Label
    info_value1 : Label

    info_label2 : Label
    info_value2 : Label

    info_label3 : Label
    info_value3 : Label

    info_label4 : Label
    info_value4 : Label

    info_label5 : Label
    info_value5 : Label

    info_label6 : Label
    info_value6 : Label

    info_label7 : Label
    info_value7 : Label

    info_label8 : Label
    info_value8 : Label

    _result_grid : TrackerResultGrid


    prop Query : TrackerQuery

    prop ResultGrid : TrackerResultGrid
        get
            return _result_grid
        set
            if value is not null
                _result_grid = value

                _result_grid.SelectionChanged += def (path)
                    LoadUri (path)


    init

        set_app_paintable (true)
        
        expose_event += expose
   
        border_width = 0

        table = new Table (5, 7, false)
        table.set_col_spacings (3)
        table.set_row_spacings (3)

        

        add (table)

        image = new Image.from_icon_name ("text-x-generic", IconSize.DIALOG)
        image.set_pixel_size (75)
        table.attach (image, 0, 1, 0, 5, AttachOptions.FILL, AttachOptions.FILL, 12, 0)

        name_link = new LinkButton ("")
        name_link.xalign = 0
        name_label = CreateLabel (N_("Name:"), false)
        AttachToTable (name_label, 1, 2, 0, 1, false)
        table.attach (name_link, 2, 3, 0, 1, AttachOptions.FILL, AttachOptions.FILL, 0, 0)

        path_link = new LinkButton ("")
        path_link.xalign = 0
        path_label = CreateLabel (N_("Folder:"), false)
        AttachToTable (path_label, 3, 4, 0, 1, false)
        table.attach (path_link, 4, 7, 0, 1, AttachOptions.FILL, AttachOptions.FILL, 0, 0)

        info_label1 = CreateLabel (N_("Type:"), false)
        AttachToTable (info_label1, 1, 2, 1, 2, false)

        info_value1 = CreateLabel ("-", true)
        AttachToTable (info_value1, 2, 3, 1, 2, true)

        info_label2 = CreateLabel (N_("Size:"), false)
        AttachToTable (info_label2, 3, 4, 1, 2, false)

        info_value2 = CreateLabel ("-", true)
        AttachToTable (info_value2, 4, 5, 1, 2, true)

        info_label3 = CreateLabel (N_("Modified:"), false)
        AttachToTable (info_label3, 5, 6, 1, 2, false)

        info_value3 = CreateLabel ("-", true)
        AttachToTable (info_value3, 6, 7, 1, 2, true)

        info_label4 = CreateLabel ("", false)
        AttachToTable (info_label4, 1, 2, 2, 3, false)

        info_value4 = CreateLabel ("-", true)
        AttachToTable (info_value4, 2, 3, 2, 3, true)

        info_label5 = CreateLabel ("", false)
        AttachToTable (info_label5, 3, 4, 2, 3, false)

        info_value5 = CreateLabel ("-", true)
        AttachToTable (info_value5, 4, 5, 2, 3, true)

        info_label6 = CreateLabel ("", false)
        AttachToTable (info_label6, 5, 6, 2, 3, false)

        info_value6 = CreateLabel ("-", true)
        AttachToTable (info_value6, 6, 7, 2, 3, true)
        
        sparql = "SELECT ?mimetype WHERE {<%s> nie:mimeType ?mimetype.}"

        ClearLabels ()


    def private expose (e : Gdk.EventExpose) : bool
    
        var cr = Gdk.cairo_create (self.window)

        var style = self.get_style ()
        var step1 = style.bg [StateType.NORMAL]
        var step2 = style.bg [StateType.SELECTED]
        
        w,h : double
        w = self.allocation.width
        h = self.allocation.height

	    /* clear window to base[NORMAL] */
        cr.set_operator (Operator.SOURCE)
        Gdk.cairo_set_source_color (cr, step1)
        cr.paint ()
        cr.move_to (0, 0)
        cr.set_line_width (1.0)
        cr.set_operator (Operator.OVER)

        /* main gradient */
        var pat = new Pattern.linear (0.0, 0.0, 0.0, h)
        
        pat.add_color_stop_rgba (0.0, step2.red/65535.0, step2.green/65535.0, step2.blue/65535.0, 0.05)
	                                   
        pat.add_color_stop_rgba (1.0, step2.red/65535.0, step2.green/65535.0, step2.blue/65535.0, 0.5)

        cr.rectangle (0, 0, w, h)
        cr.set_source (pat)
        cr.fill ()
	
        /* border line */
        cr.set_source_rgba (step2.red/65535.0, step2.green/65535.0, step2.blue/65535.0, 0.7)
        cr.move_to (0, 0)
        cr.line_to (w, 0)
        cr.stroke ()

        /* highlight line */
        cr.set_source_rgba (1.0, 1.0, 1.0, 0.5)
        cr.move_to (0, 1)
        cr.line_to (w, 1)
        cr.stroke ()

        return super.expose_event (e) 


    def private AttachToTable (lab : Label, l : int, r : int, t : int, b : int, e : bool)
        if e is true
            table.attach (lab, l, r, t, b, AttachOptions.FILL | AttachOptions.EXPAND , AttachOptions.FILL, 0, 0)
        else
            table.attach (lab, l, r, t, b, AttachOptions.FILL, AttachOptions.FILL, 0, 0)

    def private CreateLabel (s : string, e : bool) : Label
        var l = new Label (s)
        l.xalign = 0
        l.set_use_markup (true)

        if e is true
            l.ellipsize = Pango.EllipsizeMode.END

        return l


    def ClearLabels ()
        info_value1.set_text ("")
        info_value2.set_text ("")
        info_value3.set_text ("")
        info_value4.set_text ("")
        info_value5.set_text ("")
        info_value6.set_text ("")
        info_label4.set_text ("")
        info_label5.set_text ("")
        info_label6.set_text ("")
        
        name_link.uri = ""
        name_link.label = ""
        path_link.uri = ""
        path_link.label = ""

        name_link.set_sensitive (false)
        path_link.set_sensitive (false);

    def SetLabelValue (label : Label, val : string)
        var val1 = "<b>%s</b>".printf (val)
        label.set_markup (val1)
        label.xalign = 0
        
    def SetLabelSizeValue (label : Label, size: int64)
        var val1 = "<b>%s</b>".printf (FormatFileSize (size))
        label.set_markup (val1)
        label.xalign = 0
        
        
    def SetLabelUrnValue (label : Label, val : string)
        var value = val
        
        var values = val.split (":")

        for s in values
            value = s

        var escapes = value.split ("%20")
        value = ""
        for s in escapes
            value += s + " "

        var val1 = "<b>%s</b>".printf (value)
        label.set_markup (val1)
        label.xalign = 0
    
        


    def private GetCategory (uri : string) : Categories
        var query = "select rdf:type(?s) where { ?s nie:url \"%s\" }".printf(uri)
        var results = Query.Query (query)
        var res = ""
        for s in results
            res += s

        if res.contains ("nfo#Video") do return Categories.Video
        if res.contains ("nfo#Image") do return Categories.Image
        if res.contains ("nfo#Audio") do return Categories.Music
        if res.contains ("nfo#Document") do return Categories.Document
        if res.contains ("nfo#Software") do return Categories.Application       
        if res.contains ("nfo#Folder") do return Categories.Folder        
                         
        return Categories.File
        
    
    def private DisplayFileDetails (uri : string, mime : string)
        var file = File.new_for_uri (uri)
        var filepath = file.get_basename ()
        var displaypath = file.get_parent ()
        
        name_link.uri = uri
        name_link.label = filepath
        path_link.uri = displaypath.get_uri ()
        path_link.label = displaypath.get_path ()

        link_label : Label
        link_label = (Label) path_link.get_child ()
        link_label.set_ellipsize (EllipsizeMode.MIDDLE)

        link_label = (Label) name_link.get_child ()
        link_label.set_ellipsize (EllipsizeMode.MIDDLE)

        SetLabelValue (info_value1, mime)

        try
            var info =  file.query_info ("standard::size,time::modified", \
                                         FileQueryInfoFlags.NONE, null)

            SetLabelSizeValue (info_value2, info.get_size())

            tm : TimeVal
            info.get_modification_time (out tm)

            var val3 = "<b>%s</b>".printf (tm.to_iso8601 ())

            info_value3.set_markup (val3)

        except e:Error
            print "Could not get file info for %s", uri

        
    def private DisplayImageDetails (uri : string)
        var query = "select nfo:height(?s) nfo:width(?s) Where { ?s nie:url \"%s\" }".printf(uri)
        var result = Query.Query (query)

        info_label4.set_text (N_("Height:"))
        info_label5.set_text (N_("Width:"))

        if result is not null 
            SetLabelValue (info_value4, result[0])  
            SetLabelValue (info_value5, result[1])  
                     

        
    def private DisplayMusicDetails (uri : string)
        var query = "select nie:title(?s) nmm:performer(?s) nmm:musicAlbum(?s) Where { ?s nie:url \"%s\" }".printf(uri)
        var result = Query.Query (query)

        info_label4.set_text (N_("Title:"))
        info_label5.set_text (N_("Artist:"))
        info_label6.set_text (N_("Album:"))

        if result is not null 
            SetLabelValue (info_value4, result[0])  
            SetLabelUrnValue (info_value5, result[1])  
            SetLabelUrnValue (info_value6, result[2])  
        
    def private DisplayVideoDetails (uri : string)
        var query = "select nfo:height(?s) nfo:width(?s) nfo:duration (?s) Where { ?s nie:url \"%s\" }".printf(uri)
        var result = Query.Query (query)

        info_label4.set_text (N_("Height:"))
        info_label5.set_text (N_("Width:"))
        info_label6.set_text (N_("Duration:"))        

        if result is not null 
            SetLabelValue (info_value4, result[0])  
            SetLabelValue (info_value5, result[1])  
            SetLabelValue (info_value6, result[2])  
        
    def private DisplayDocumentDetails (uri : string)
        var query = "select nie:title(?s) nco:creator(?s) nfo:pageCount (?s) Where { ?s nie:url \"%s\" }".printf(uri)
        var result = Query.Query (query)

        info_label4.set_text (N_("Title:"))
        info_label5.set_text (N_("Author:"))
        info_label6.set_text (N_("Page count:"))        

        if result is not null 
            SetLabelValue (info_value4, result[0])  
            SetLabelValue (info_value5, result[1])  
            SetLabelValue (info_value6, result[2])      
        
    def private DisplayApplicationDetails (uri : string)
        return   
        
    def LoadUri (path : TreePath?)
        ClearLabels ()

        if path is null
            image.set_from_icon_name ("text-x-generic", IconSize.DIALOG)
            return

        name_link.set_sensitive (true)
        path_link.set_sensitive (true);

        iter : TreeIter
        id, uri, mime: weak string
        icon : Gdk.Pixbuf

        _result_grid.store.get_iter (out iter, path)
        _result_grid.store.get (iter, ResultColumns.Id, out id, ResultColumns.Uri, out uri, ResultColumns.Mime, out mime, ResultColumns.Icon, out icon)
       
        
        /* determine category type */
        var cat = GetCategory (uri)

        if cat is not Categories.Application
            DisplayFileDetails (uri, mime)
            image.set_from_pixbuf (icon)

        case cat
            when Categories.Application do DisplayApplicationDetails (uri)
            when Categories.Music do DisplayMusicDetails (uri)
            when Categories.Video do DisplayVideoDetails (uri)
            when Categories.Image do DisplayImageDetails (uri)
            when Categories.Document do DisplayDocumentDetails (uri)
            default do return

                
       