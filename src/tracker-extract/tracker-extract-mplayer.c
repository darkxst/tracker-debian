/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2006, Edward Duffy (eduffy@gmail.com)
 * Copyright (C) 2006, Laurent Aguerreche (laurent.aguerreche@free.fr)
 * Copyright (C) 2008, Nokia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "config.h"

#include <string.h>

#include <glib.h>

#include <libtracker-common/tracker-os-dependant.h>

#include "tracker-main.h"
#include "tracker-escape.h"

static void extract_mplayer (const gchar *filename,
			     GHashTable  *metadata);

static TrackerExtractData data[] = {
	{ "audio/*", extract_mplayer },
	{ "video/*", extract_mplayer },
	{ NULL, NULL }
};

static gchar *video_tags[][2] = {
	{ "ID_VIDEO_HEIGHT",	"Video:Height"		},
	{ "ID_VIDEO_WIDTH",	"Video:Width"		},
	{ "ID_VIDEO_FPS",	"Video:FrameRate"	},
	{ "ID_VIDEO_CODEC",	"Video:Codec"		},
	{ "ID_VIDEO_BITRATE",	"Video:Bitrate"		},
	{ NULL,			NULL			}
};

static gchar *audio_tags[][2] = {
#ifdef ENABLE_DETAILED_METADATA
	{ "ID_AUDIO_BITRATE",	"Audio:Bitrate"		},
	{ "ID_AUDIO_RATE",	"Audio:Samplerate"	},
	{ "ID_AUDIO_CODEC",	"Audio:Codec"		},
	{ "ID_AUDIO_NCH",	"Audio:Channels"	},
#endif /* ENABLE_DETAILED_METADATA */
	{ NULL,			NULL			}
};

/* Some of "info_tags" tags can belong to Audio or/and video or none, so 3 cases :
 * 1/ tag does not belong to audio nor video, it is a general tag ;
 * 2/ tag only belongs to audio ;
 * 3/ tag can belong to audio and video. If current media has video we will associate
 *    tag to Video, otherwise to Audio if it has audio.
 */
static gchar *info_tags[][3] = {
#ifdef ENABLE_DETAILED_METADATA
	{ "Comment",		"Audio:Comment",	"Video:Comment"	},
#endif /* ENABLE_DETAILED_METADATA */
	{ "Title",		"Audio:Title",		"Video:Title"	},
	{ "Genre",		"Audio:Genre",		NULL		},
	{ "Track",		"Audio:TrackNo",	NULL		},
	{ "Artist",		"Audio:Performer",	"Video:Author"	},
	{ "Album",		"Audio:Album",		NULL		},
	{ "Year",		"Audio:ReleaseDate",	NULL		},
	{ "copyright",		"File:Copyright",	NULL		},
	{ NULL,			NULL,			NULL		}
};

static void
copy_hash_table_entry (gpointer key,
		       gpointer value,
		       gpointer user_data)
{
	g_hash_table_insert (user_data,
			     g_strdup (key),
			     tracker_escape_metadata (value));
}

static void
extract_mplayer (const gchar *filename,
		 GHashTable  *metadata)
{
	gchar *argv[10];
	gchar *mplayer;

	argv[0] = g_strdup ("mplayer");
	argv[1] = g_strdup ("-identify");
	argv[2] = g_strdup ("-frames");
	argv[3] = g_strdup ("0");
	argv[4] = g_strdup ("-vo");
	argv[5] = g_strdup ("null");
	argv[6] = g_strdup ("-ao");
	argv[7] = g_strdup ("null");
	argv[8] = g_strdup (filename);
	argv[9] = NULL;

	if (tracker_spawn (argv, 10, &mplayer, NULL)) {
		GPatternSpec  *pattern_ID_AUDIO_ID;
		GPatternSpec  *pattern_ID_VIDEO_ID;
		GPatternSpec  *pattern_ID_AUDIO;
		GPatternSpec  *pattern_ID_VIDEO;
		GPatternSpec  *pattern_ID_CLIP_INFO_NAME;
		GPatternSpec  *pattern_ID_CLIP_INFO_VALUE;
		GPatternSpec  *pattern_ID_LENGTH;
		GHashTable    *tmp_metadata_audio;
		GHashTable    *tmp_metadata_video;
		gboolean       has_audio, has_video;
		gchar	      *duration;
		gchar	     **lines, **line;

		pattern_ID_AUDIO_ID = g_pattern_spec_new ("ID_AUDIO_ID=*");
		pattern_ID_VIDEO_ID = g_pattern_spec_new ("ID_VIDEO_ID=*");
		pattern_ID_AUDIO = g_pattern_spec_new ("ID_AUDIO*=*");
		pattern_ID_VIDEO = g_pattern_spec_new ("ID_VIDEO*=*");
		pattern_ID_CLIP_INFO_NAME = g_pattern_spec_new ("ID_CLIP_INFO_NAME*=*");
		pattern_ID_CLIP_INFO_VALUE = g_pattern_spec_new ("ID_CLIP_INFO_VALUE*=*");
		pattern_ID_LENGTH = g_pattern_spec_new ("ID_LENGTH=*");

		tmp_metadata_audio = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
		tmp_metadata_video = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

		has_audio = has_video = FALSE;

		duration = NULL;

		lines = g_strsplit (mplayer, "\n", -1);

		for (line = lines; *line; ++line) {
			if (g_pattern_match_string (pattern_ID_AUDIO_ID, *line)) {
				has_audio = TRUE;
			}

			else if (g_pattern_match_string (pattern_ID_VIDEO_ID, *line)) {
				has_video = TRUE;
			}

			else if (g_pattern_match_string (pattern_ID_AUDIO, *line)) {
				gint i;

				for (i = 0; audio_tags[i][0]; i++) {
					if (g_str_has_prefix (*line, audio_tags[i][0])) {
						g_hash_table_insert (metadata,
								     g_strdup (audio_tags[i][1]),
								     tracker_escape_metadata ((*line) + strlen (audio_tags[i][0]) + 1));
						break;
					}
				}
			}

			else if (g_pattern_match_string (pattern_ID_VIDEO, *line)) {
				gint i;

				for (i = 0; video_tags[i][0]; i++) {
					if (g_str_has_prefix (*line, video_tags[i][0])) {
						g_hash_table_insert (metadata,
								     g_strdup (video_tags[i][1]),
								     tracker_escape_metadata ((*line) + strlen (video_tags[i][0]) + 1));
						break;
					}
				}
			}

			else if (g_pattern_match_string (pattern_ID_CLIP_INFO_NAME, *line)) {
				const char *next_line;

				next_line = *(line + 1);

				if (next_line) {
					if (g_pattern_match_string (pattern_ID_CLIP_INFO_VALUE, next_line)) {
						gint i;

						for (i = 0; info_tags[i][0]; i++) {
							if (g_str_has_suffix (*line, info_tags[i][0])) {
								gchar *equal_char_pos, *data;

								equal_char_pos = strchr (next_line, '=');

								data = g_strdup (equal_char_pos + 1);

								if (data) {
									if (data[0] != '\0') {
										g_hash_table_insert (tmp_metadata_audio,
												     g_strdup (info_tags[i][1]),
												     data);

										if (info_tags[i][2]) {
											g_hash_table_insert (tmp_metadata_video,
													     g_strdup (info_tags[i][2]),
													     g_strdup (data));
										}
									} else {
										g_free (data);
									}
								}

								break;
							}
						}

						line++;
					}
				}
			}

			else if (g_pattern_match_string (pattern_ID_LENGTH, *line)) {
				gchar *equal_char_pos;

				equal_char_pos = strchr (*line, '=');

				duration = g_strdup (equal_char_pos + 1);
			}
		}

		g_pattern_spec_free (pattern_ID_AUDIO_ID);
		g_pattern_spec_free (pattern_ID_VIDEO_ID);
		g_pattern_spec_free (pattern_ID_AUDIO);
		g_pattern_spec_free (pattern_ID_VIDEO);
		g_pattern_spec_free (pattern_ID_CLIP_INFO_NAME);
		g_pattern_spec_free (pattern_ID_CLIP_INFO_VALUE);
		g_pattern_spec_free (pattern_ID_LENGTH);

		if (has_video) {
			if (tmp_metadata_video) {
				g_hash_table_foreach (tmp_metadata_video, 
						      copy_hash_table_entry, 
						      metadata);
				g_hash_table_destroy (tmp_metadata_video);
			}

			if (duration) {
				g_hash_table_insert (metadata, g_strdup ("Video:Duration"), duration);
			}
		} else if (has_audio) {
			if (tmp_metadata_video) {
				g_hash_table_foreach (tmp_metadata_audio, 
						      copy_hash_table_entry, 
						      metadata);
				g_hash_table_destroy (tmp_metadata_audio);
			}

			if (duration) {
				g_hash_table_insert (metadata, g_strdup ("Audio:Duration"), duration);
			}
		}
	}
}

TrackerExtractData *
tracker_get_extract_data (void)
{
	return data;
}
