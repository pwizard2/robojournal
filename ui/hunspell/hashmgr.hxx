/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

    RoboJournal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RoboJournal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RoboJournal.  If not, see <http://www.gnu.org/licenses/>.
  */

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Hunspell, based on MySpell.
 *
 * The Initial Developers of the Original Code are
 * Kevin Hendricks (MySpell) and Laszlo Nemeth (Hunspell).
 * Portions created by the Initial Developers are Copyright (C) 2002-2005
 * the Initial Developers. All Rights Reserved.
 *
 * Contributor(s):
 * David Einstein
 * Davide Prina
 * Giuseppe Modugno
 * Gianluca Turconi
 * Simon Brouwer
 * Noll Janos
 * Biro Arpad
 * Goldman Eleonora
 * Sarlos Tamas
 * Bencsath Boldizsar
 * Halacsy Peter
 * Dvornik Laszlo
 * Gefferth Andras
 * Nagy Viktor
 * Varga Daniel
 * Chris Halls
 * Rene Engelhard
 * Bram Moolenaar
 * Dafydd Jones
 * Harri Pitkanen
 * Andras Timar
 * Tor Lillqvist
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef _HASHMGR_HXX_
#define _HASHMGR_HXX_

#ifndef MOZILLA_CLIENT
#include <cstdio>
#else
#include <stdio.h>
#endif

#include "ui/hunspell/htypes.hxx"
#include "ui/hunspell/filemgr.hxx"

enum flag { FLAG_CHAR, FLAG_LONG, FLAG_NUM, FLAG_UNI };

class HashMgr
{
  int             tablesize;
  struct hentry ** tableptr;
  int             userword;
  flag            flag_mode;
  int             complexprefixes;
  int             utf8;
  unsigned short  forbiddenword;
  int 		  langnum;
  char *          enc;
  char *          lang;
  struct cs_info * csconv;
  char *          ignorechars;
  unsigned short * ignorechars_utf16;
  int             ignorechars_utf16_len;
  int                 numaliasf; // flag vector `compression' with aliases
  unsigned short **   aliasf;
  unsigned short *    aliasflen;
  int                 numaliasm; // morphological desciption `compression' with aliases
  char **             aliasm;


public:
  HashMgr(const char * tpath, const char * apath, const char * key = NULL);
  ~HashMgr();

  struct hentry * lookup(const char *) const;
  int hash(const char *) const;
  struct hentry * walk_hashtable(int & col, struct hentry * hp) const;

  int add(const char * word, char * aff);
  int add_with_affix(const char * word, const char * pattern);
  int remove(const char * word);
  int decode_flags(unsigned short ** result, char * flags);
  unsigned short        decode_flag(const char * flag);
  char *                encode_flag(unsigned short flag);
  int is_aliasf();
  int get_aliasf(int index, unsigned short ** fvec);
  int is_aliasm();
  char * get_aliasm(int index);
  
private:
  int get_clen_and_captype(const char * word, int wbl, int * captype);
  int load_tables(const char * tpath, const char * key);
  int add_word(const char * word, int wbl, int wcl, unsigned short * ap,
    int al, const char * desc, bool onlyupcase);
  int load_config(const char * affpath, const char * key);
  int parse_aliasf(char * line, FileMgr * af);
  int add_hidden_capitalized_word(char * word, int wbl, int wcl,
    unsigned short * flags, int al, char * dp, int captype);
  int parse_aliasm(char * line, FileMgr * af);
  int remove_forbidden_flag(const char * word);

};

#endif
