AM_CPPFLAGS =                                          \
	$(BUILD_CFLAGS)                                \
	-I$(top_srcdir)/src                            \
	-I$(top_builddir)/src                          \
	-DSHAREDIR=\""$(datadir)"\"                    \
	-DLOCALEDIR=\""$(localedir)"\"                 \
	-DLIBEXEC_PATH=\""$(libexecdir)"\"             \
	$(TRACKER_MINER_FS_CFLAGS)

libexec_PROGRAMS = tracker-miner-fs

# Prefer UPower for the power data if it's available, else fall back to HAL
if HAVE_UPOWER
power_headers =                                        \
	tracker-power.h
power_sources =                                        \
	tracker-power-upower.c
else
if HAVE_HAL
power_headers =                                        \
	tracker-power.h
power_sources =                                        \
	tracker-power-hal.c
endif
endif

tracker_miner_fs_SOURCES =                             \
	$(power_sources)                               \
	$(power_headers)                               \
	tracker-config.c                               \
	tracker-config.h                               \
	tracker-main.c                                 \
	tracker-miner-files.c                          \
	tracker-miner-files.h                          \
	tracker-miner-files-index.c                    \
	tracker-miner-files-index.h                    \
	tracker-writeback-listener.c                   \
	tracker-writeback-listener.h                   \
	tracker-writeback-dispatcher.c                 \
	tracker-writeback-dispatcher.h                 \
	tracker-writeback.c                            \
	tracker-writeback.h

tracker_miner_fs_LDADD =                               \
	$(top_builddir)/src/libtracker-miner/libtracker-miner-@TRACKER_API_VERSION@.la \
	$(top_builddir)/src/libtracker-sparql-backend/libtracker-sparql-@TRACKER_API_VERSION@.la \
	$(top_builddir)/src/libtracker-extract/libtracker-extract.la \
	$(top_builddir)/src/libtracker-data/libtracker-data.la \
	$(top_builddir)/src/libtracker-common/libtracker-common.la \
	$(BUILD_LIBS)                                  \
	$(TRACKER_MINER_FS_LIBS)
