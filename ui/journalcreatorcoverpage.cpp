/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    

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

    6/23/13: This class is just a cover page for the NewJournalCreator class.
    Its purpose is to provide info about each type of backend RoboJournal can use.

*/


#include "journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"

JournalCreatorCoverPage::JournalCreatorCoverPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JournalCreatorCoverPage)
{
    ui->setupUi(this);

   /*
    *SAVE OLD TEXT FOR 0.6.
    *
    *<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html><head><meta name="qrichtext" content="1" /><style type="text/css">
p, li { white-space: pre-wrap; }
</style></head><body>
<p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">The Journal Creator tool allows you to create a new (blank) journal. Select your desired journal type from the list on the left:</p>
<ul style="margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;"><li  style=" margin-top:12px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Click the <span style=" font-weight:600;">SQLite Journal</span> option to create a new SQLite-based journal. SQLite stores your journal as a
<span style=" font-weight:600;">*.db</span> file on your hard drive. This option is ideal for users with little database experience or those who desire a highly-portable solution. SQLite-based journals require no additional software.</li>
<li  style=" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Click the <span style=" font-weight:600;">MySQL Journal</span> option to create a new MySQL-based journal in accordance with the traditional client-and-server database model. MySQL stores your journal on a computer designated as &quot;host&quot;; you then access and modify your data through a &quot;client&quot; computer (it is possible for client and host to be the same machine). MySQL offers much higher security and greater flexibility than SQLite but also requires you to have more database experience. The host computer <span style=" text-decoration: underline;">must</span> have a working instance of MySQL Server.</li></ul></body></html>
    **/
}

JournalCreatorCoverPage::~JournalCreatorCoverPage()
{
    delete ui;
}
