/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009, Nokia (urho.konttori@nokia.com)
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

#ifndef __TRACKER_EXTRACT_TESTSUITE_JPEG_H__
#define __TRACKER_EXTRACT_TESTSUITE_JPEG_H__

#include <tracker-extract/tracker-extract.h>

void test_tracker_extract_jpeg_basic_size       (gconstpointer data);
void test_tracker_extract_jpeg_exif_size        (gconstpointer data);
void test_tracker_extract_jpeg_exif_orientation (gconstpointer data);
void test_tracker_extract_jpeg_exif_flash       (gconstpointer data);

void test_tracker_extract_jpeg_exif_tags        (gconstpointer data);

void test_tracker_extract_jpeg_performance      (gconstpointer data);

#endif
