/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
 */

#include "libconfig.h"

#define CREAM_CONFIG_ERROR cream_config_error_quark ()

typedef enum
{
     CREAM_CFG_ERROR_PARSE,
     CREAM_CFG_ERROR_FAILED
} CreamCfgError;

GQuark cream_config_error_quark (void)
{
     static GQuark cream_config_error = 0;

     if (cream_config_error == 0)
     {
          cream_config_error = g_quark_from_string ("CREAM_CONFIG_ERROR");
     }

     return cream_config_error;
}

/* /global/user-agent/set() */
static cfg_opt_t config_user_agent_set_opts[] =
{
     CFG_STR ("name", "", CFGF_NONE),
     CFG_END ()
};

/* /global/user-agent/ */
static cfg_opt_t config_user_agent_opts[] =
{
     CFG_SEC ("set", config_user_agent_set_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

/* /global/ */
static cfg_opt_t config_global_opts[] =
{
     CFG_STR  ("homepage",   "http://cream-browser.net/", CFGF_NONE),
     CFG_STR  ("encodage",   "default",                   CFGF_NONE),
     CFG_BOOL ("javascript", cfg_true,                    CFGF_NONE),

     CFG_SEC  ("user-agent", config_user_agent_opts, CFGF_NONE),

     CFG_STR  ("bookmarks", "", CFGF_NONE),
     CFG_STR  ("history",   "", CFGF_NONE),
     CFG_STR  ("cookie",    "", CFGF_NONE),
     CFG_END  ()
};

/* /handlers/set() */
static cfg_opt_t config_handlers_set_opts[] =
{
     CFG_STR ("handler", "", CFGF_NONE),
     CFG_END ()
};

/* /handlers/ */
static cfg_opt_t config_handlers_opts[] =
{
     CFG_SEC ("set", config_handlers_set_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

/* /keys/bind() */
static cfg_opt_t config_keys_bind_opts[] =
{
     CFG_STR ("cmd", "", CFGF_NONE),
     CFG_END ()
};

/* /keys/ */
static cfg_opt_t config_keys_opts[] =
{
     CFG_SEC ("bind", config_keys_bind_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

/* / */
static cfg_opt_t config_root_opts[] =
{
     CFG_SEC ("global",   config_global_opts,   CFGF_NONE),
     CFG_SEC ("handlers", config_handlers_opts, CFGF_NONE),
     CFG_SEC ("keys",     config_keys_opts,     CFGF_NONE),
     CFG_END ()
};


gboolean cream_config_load (gchar *path, struct cream_config_t *cfg, GError **error)
{
     int i = 0;

     struct
     {
          cfg_t *root;
          cfg_t *global;
          cfg_t *user_agent;
          cfg_t *handlers;
          cfg_t *keys;
     } cream_cfg;

     g_return_val_if_fail (path != NULL, FALSE);

     cfg = malloc (sizeof (struct cream_config_t));
     cfg->global.user_agent = NULL;
     cfg->handlers          = NULL;
     cfg->keys              = NULL;

     cream_cfg.root = cfg_init (config_root_opts, CFGF_NONE);

     if (cfg_parse (cream_cfg.root, path) == CFG_PARSE_ERROR)
     {
          free (cfg);
          cfg = NULL;

          g_set_error (error, CREAM_CONFIG_ERROR, CREAM_CFG_ERROR_PARSE, "Failed to parse: %s", path);

          return FALSE;
     }

     cream_cfg.global     = cfg_getsec (cream_cfg.root,   "global");
     cream_cfg.user_agent = cfg_getsec (cream_cfg.global, "user-agent");
     cream_cfg.handlers   = cfg_getsec (cream_cfg.root,   "handlers");
     cream_cfg.keys       = cfg_getsec (cream_cfg.root,   "keys");

     /* parse /global/ */
     cfg->global.homepage   = g_strdup (cfg_getstr (cream_cfg.global, "homepage"));
     cfg->global.encodage   = g_strdup (cfg_getstr (cream_cfg.global, "encodage"));
     cfg->global.javascript = cfg_getbool (cream_cfg.global, "javascript");

     cfg->global.bookmarks = g_strdup (cfg_getstr (cream_cfg.global, "bookmarks"));
     cfg->global.history   = g_strdup (cfg_getstr (cream_cfg.global, "history"));
     cfg->global.cookie    = g_strdup (cfg_getstr (cream_cfg.global, "cookie"));

     /* parse /global/user-agent/ */
     for (i = 0; i < cfg_size (cream_cfg.user_agent, "set"); ++i)
     {
          struct user_agent_t *tmp = malloc (sizeof (struct user_agent_t));
          cfg_t *ua = cfg_getnsec (cream_cfg.user_agent, "set", i);

          tmp->domain = g_strdup (cfg_title (ua));
          tmp->name   = g_strdup (cfg_getstr (ua, "name"));
          tmp->next   = cfg->global.user_agent;
          cfg->global.user_agent = tmp;
     }

     /* parse /handlers/ */
     for (i = 0; i < cfg_size (cream_cfg.handlers, "set"); ++i)
     {
          struct handler_t *tmp = malloc (sizeof (struct handler_t));
          cfg_t *handler = cfg_getnsec (cream_cfg.handlers, "set", i);

          tmp->name = g_strdup (cfg_title (handler));
          tmp->cmd  = g_strdup (cfg_getstr (handler, "handler"));
          tmp->next = cfg->handlers;
          cfg->handlers = tmp;
     }

     /* parse /keys/ */
     for (i = 0; i < cfg_size (cream_cfg.keys, "bind"); ++i)
     {
          struct key_t *tmp = malloc (sizeof (struct key_t));
          cfg_t *key = cfg_getnsec (cream_cfg.keys, "bind", i);

          tmp->bind = g_strdup (cfg_title (key));
          tmp->cmd  = g_strdup (cfg_getstr (key, "cmd"));
          tmp->next = cfg->keys;
          cfg->keys = tmp;
     }

     cfg_free (cream_cfg.root);

     return TRUE;
}

void cream_config_free (struct cream_config_t *cfg)
{
     struct user_agent_t *tmp_ua = NULL;
     struct handler_t *tmp_handler = NULL;
     struct key_t *tmp_key = NULL;

     g_return_if_fail (cfg != NULL);

     free (cfg->global.homepage);
     free (cfg->global.encodage);
     free (cfg->global.bookmarks);
     free (cfg->global.history);
     free (cfg->global.cookie);

     tmp_ua = cfg->global.user_agent;
     while (tmp_ua != NULL)
     {
          struct user_agent_t *next = tmp_ua->next;

          free (tmp_ua->domain);
          free (tmp_ua->name);
          free (tmp_ua);

          tmp_ua = next;
     }

     tmp_handler = cfg->handlers;
     while (tmp_handler != NULL)
     {
          struct handler_t *next = tmp_handler->next;

          free (tmp_handler->name);
          free (tmp_handler->cmd);
          free (tmp_handler);

          tmp_handler = next;
     }

     tmp_key = cfg->keys;
     while (tmp_key != NULL)
     {
          struct key_t *next = tmp_key->next;

          free (tmp_key->bind);
          free (tmp_key->cmd);
          free (tmp_key);

          tmp_key = next;
     }

     free (cfg);
     cfg = NULL;
}

