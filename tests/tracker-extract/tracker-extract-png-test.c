/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008, Nokia (urho.konttori@nokia.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <glib.h>

#include <tracker-extract/tracker-extract.h>

#include "tracker-extract-test-utils.h"
#include "tracker-extract-testsuite-generic.h"
#include "tracker-extract-testsuite-png.h"

int
main (int argc, char **argv) {

	TrackerExtractData *data;
	gchar *path;
	gint result;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);

	g_test_message ("Testing extract functionality");

	path = g_build_filename (MODULESDIR, "libextract-png", NULL);
	data = tracker_test_extract_get_extract (path, "image/png");
	g_free (path);

	g_test_add_data_func ("/tracker-extract/tracker-extract-png/check-extract-data",
			      data, test_tracker_extract_check_extract_data);

#if 0
	g_test_add_data_func ("/tracker-extract/tracker-extract-png/basic_size",
			      data, test_tracker_extract_png_basic_size);

/* 	g_test_add_data_func ("/tracker-extract/tracker-extract-png/xmp_exif_orientation", */
/* 			      data, test_tracker_extract_png_xmp_exif_orientation); */

	if (g_test_perf()) {
		g_test_add_data_func ("/tracker-extract/tracker-extract-png/performance",
				      data, performance_tracker_extract_png);
	}

#endif


	result = g_test_run ();

	return result;
}
