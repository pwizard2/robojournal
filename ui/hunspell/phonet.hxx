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

/*  phonetic.c - generic replacement aglogithms for phonetic transformation
    Copyright (C) 2000 Bjoern Jacke

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License version 2.1 as published by the Free Software Foundation;
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; If not, see
    <http://www.gnu.org/licenses/>.

    Changelog:

    2000-01-05  Bjoern Jacke <bjoern at j3e.de>
                Initial Release insprired by the article about phonetic
                transformations out of c't 25/1999

    2007-07-26  Bjoern Jacke <bjoern at j3e.de>
		Released under MPL/GPL/LGPL tri-license for Hunspell
		
    2007-08-23  Laszlo Nemeth <nemeth at OOo>
                Porting from Aspell to Hunspell using C-like structs
*/

#ifndef __PHONETHXX__
#define __PHONETHXX__

#define HASHSIZE          256
#define MAXPHONETLEN      256
#define MAXPHONETUTF8LEN  (MAXPHONETLEN * 4)

struct phonetable {
  char utf8;
  cs_info * lang;
  int num;
  char * * rules;
  int hash[HASHSIZE];
};

void init_phonet_hash(phonetable & parms);

int phonet (const char * inword, char * target,
              int len, phonetable & phone);

#endif
