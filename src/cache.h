/*
* Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __CACHE_H
#define __CACHE_H

typedef enum
{
     CACHE_FOLDER_FAVICONS,
     CACHE_FOLDER_COOKIES,
     CACHE_FOLDER_HISTORY,
     CACHE_FOLDER_DOWNLOADS,
     CACHE_FOLDER_SESSION,
     CACHE_FOLDER_COMMANDS,
     CACHE_FOLDER_NONE
} CacheFolder;

typedef struct
{
     GHashTable *datas;
     GChecksumType algorithm;
     gchar *cache_path;
} Cache;

void cache_init (GChecksumType algorithm);
void cache_insert (CacheFolder folder, gpointer key, gpointer value);
gpointer cache_lookup (CacheFolder folder, gpointer key);

#endif /* __CACHE_H */
