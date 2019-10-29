#ifndef __THEMERC_H__
#define __THEMERC_H__

gchar 	**trc_open	(gchar *fname);
void 	trc_close	(gchar **rcs);
gchar 	*trc_get_str	(gchar **rcs, gchar *param);
gint 	trc_get_uint	(gchar **rcs, gchar *param);

#endif
