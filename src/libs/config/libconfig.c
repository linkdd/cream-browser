/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  @defgroup libconfig Cream-Browser configuration
  @ingroup libcream
  @brief Functions to load the configuration file into a structure #cream_config_t

  @{
 */

#include "libconfig.h"

#define CREAM_CONFIG_ERROR cream_config_error_quark ()

/*! \enum CreamCfgError */
typedef enum
{
     CREAM_CFG_ERROR_OPTS,    /*!< Option error */
     CREAM_CFG_ERROR_PARSE,   /*!< Parsing error (from confuse) */
     CREAM_CFG_ERROR_FAILED   /*!< Unknow error */
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

/*!
  \var static cfg_opt_t config_user_agent_set_opts[]
  \brief Options for the section '/global/user-agent/set()'
 */
static cfg_opt_t config_user_agent_set_opts[] =
{
     CFG_STR ("name", "", CFGF_NONE),
     CFG_END ()
};

/*!
  \var static cfg_opt_t config_user_agent_opts[]#
  \brief Options for the section '/global/user-agent'
 */
static cfg_opt_t config_user_agent_opts[] =
{
     CFG_SEC ("set", config_user_agent_set_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

/*!
  \var static cfg_opt_t config_global_opts[]
  \brief Options for the section '/global'
 */
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

/*!
  \var static cfg_opt_t config_handlers_set_opts[]
  \brief Options for the section '/handler/set()'
 */
static cfg_opt_t config_handlers_set_opts[] =
{
     CFG_STR ("handler", "", CFGF_NONE),
     CFG_END ()
};

/*!
  \var static cfg_opt_t config_handlers_opts[]
  \brief Options for the section '/handlers'
 */
static cfg_opt_t config_handlers_opts[] =
{
     CFG_SEC ("set", config_handlers_set_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

/*!
  \var static cfg_opt_t config_keys_bind_opts[]
  \brief Options for the section '/keys/bind()'
 */
static cfg_opt_t config_keys_bind_opts[] =
{
     CFG_STR ("cmd", "", CFGF_NONE),
     CFG_END ()
};

/*!
  \var static cfg_opt_t config_keys_opts[]
  \brief Options for the section '/keys'
 */
static cfg_opt_t config_keys_opts[] =
{
     CFG_SEC ("bind", config_keys_bind_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END ()
};

static cfg_opt_t config_vte_opts[] =
{
    CFG_STR  ("shell",   NULL,                  CFGF_NONE),
    CFG_BOOL ("reverse", cfg_false,		CFGF_NONE),
    CFG_BOOL ("doublebuffer", cfg_true,		CFGF_NONE),
    CFG_BOOL ("hints", cfg_true,		CFGF_NONE),
    CFG_END ()
};

/*!
  \var static cfg_opt_t config_root_opts[]
  \brief Options for all the configuration
 */
static cfg_opt_t config_root_opts[] =
{
     CFG_SEC ("global",   config_global_opts,   CFGF_NONE),
     CFG_SEC ("handlers", config_handlers_opts, CFGF_NONE),
     CFG_SEC ("keys",     config_keys_opts,     CFGF_NONE),
     CFG_SEC ("vte",      config_vte_opts,      CFGF_NONE),
     CFG_END ()
};

/*!
  \fn gboolean cream_config_load (gchar *path, struct cream_config_t *cfg, GError **error)
  \brief Load a new configuration file and store it in a structure #cream_config_t

  \param path Path of the configuration file
  \param cfg Pointer to the structure to store the new configuration
  \param error GError object or NULL
  \return TRUE on succes, FALSE if failed
 */
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
	  cfg_t *vte;
     } cream_cfg;

     if (path == NULL || cfg == NULL)
     {
          g_set_error (error, CREAM_CONFIG_ERROR, CREAM_CFG_ERROR_OPTS, "Can't load configuration (path not found or memory not allocated).");
          return FALSE;
     }

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
     cfg->global.encoding   = g_strdup (cfg_getstr (cream_cfg.global, "encodage"));
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

/*!
  \fn void cream_config_free (struct cream_config_t *cfg)
  \brief Free memory used by the structure #cream_config_t

  \param cfg Pointer to the structure previously allocated by #cream_config_load()
 */
void cream_config_free (struct cream_config_t *cfg)
{
     struct user_agent_t *tmp_ua = NULL;
     struct handler_t *tmp_handler = NULL;
     struct key_t *tmp_key = NULL;

     g_return_if_fail (cfg != NULL);

     free (cfg->global.homepage);
     free (cfg->global.encoding);
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
}

/*! @} */
