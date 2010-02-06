#!/bin/sh
glib-genmarshal --body marshaller.txt > marshal.c
glib-genmarshal --header marshaller.txt > marshal.h
