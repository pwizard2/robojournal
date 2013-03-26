/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
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

#include "ui/exportpreview.h"
#include "ui_exportpreview.h"
#include "core/buffer.h"
#include <iostream>
#include <QtWebKit/QWebView>
#include <QColor>
#include <QPalette>
#include "core/settingsmanager.h"

QString ExportPreview::header_font;
QString ExportPreview::body_font;
QString ExportPreview::body_size;
QString ExportPreview::header_size;
bool ExportPreview::use_em_body;
bool ExportPreview::use_em_header;
bool ExportPreview::use_rounded;
bool ExportPreview::use_system_colors;

//###########################################################################################################
ExportPreview::ExportPreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportPreview)
{
    ui->setupUi(this);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


    QString output=PrepareHTML();

    ui->webView->setHtml(output);

    if(!Buffer::preview_geometry.isEmpty()){
        resize(Buffer::preview_geometry);
    }

}

//###########################################################################################################
ExportPreview::~ExportPreview()
{

    // remember size setting
    if(!this->isMaximized()){

        QSize geo=this->size();

        SettingsManager s;
        s.SavePreviewSize(geo);
    }


    delete ui;
}

//###########################################################################################################
// This function prepares the html code and returns it as a QString. Really simple... the original plan was to
// create a temporary file to hold the HTML code but this works just as well w/ less overhead.

QString ExportPreview::PrepareHTML(){
    using namespace std;

    QString lipsum;
    lipsum.append("<p style=\"color: blue; font-size: smaller\"><b>FYI:</b> This is the classic <em>Lorem Ipsum</em> text that has been used"
                  " for placeholder purposes throughout the print industry since the 1500s."
                  " Its purpose here is to test how large blocks of text will look with the fonts and sizes you have selected."
                  " These results are generally consistent but may vary slightly depending on which browser you use (This example is rendered with Webkit).</p>");

    lipsum.append("<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque laoreet vestibulum libero vitae accumsan. ");
    lipsum.append("Nullam et nulla nunc, non luctus elit. Etiam aliquam, turpis nec pellentesque interdum, purus nulla dictum tortor, ");
    lipsum.append("nec lacinia orci turpis ac lectus. Suspendisse dignissim, lectus nec posuere consectetur, massa quam rhoncus velit, ");
    lipsum.append("eu viverra est risus ac enim. Nullam augue lacus, imperdiet ac iaculis eu, scelerisque quis nulla. Donec tempor ");
    lipsum.append("lacus molestie arcu aliquam scelerisque. Proin vitae augue quis quam imperdiet porttitor id sed magna. Nulla non ");
    lipsum.append("urna purus. Vivamus dui diam, eleifend a laoreet sit amet, dignissim vel odio. Pellentesque adipiscing sodales ante ");
    lipsum.append("at tempor. Vestibulum libero risus, malesuada vel scelerisque ut, facilisis ac nulla. Nam congue urna non diam pretium ");
    lipsum.append("dapibus.</p>");

    lipsum.append("<p>Proin viverra odio a dolor lacinia tincidunt. Nunc tempor adipiscing dolor, id condimentum leo facilisis a. Aliquam ");
    lipsum.append("erat volutpat. Cras vitae ligula mauris. Etiam nec orci ac velit mattis semper vel ut metus. Pellentesque sagittis ");
    lipsum.append("ultricies nisi, at commodo metus posuere a. Sed neque nunc, sagittis non elementum ut, imperdiet eget purus. ");
    lipsum.append("Nulla non tortor sit amet ipsum porttitor suscipit. Maecenas ipsum erat, porta id porta ac, convallis sit amet erat. ");
    lipsum.append("Nunc libero felis, blandit id vehicula dapibus, vehicula quis erat. Nam feugiat, elit a vestibulum sagittis, diam ");
    lipsum.append("metus faucibus nulla, hendrerit ullamcorper mi nulla sit amet tellus. Praesent placerat velit quis lectus lacinia ");
    lipsum.append("sed laoreet magna sagittis. Maecenas lacinia, metus at ultrices sodales, quam orci ullamcorper neque, viverra ");
    lipsum.append("molestie metus dolor vel velit. Mauris ligula orci, ullamcorper at adipiscing ornare, euismod sed augue. Praesent ");
    lipsum.append("nec leo arcu, id elementum leo. Aliquam fringilla semper nulla, eget congue turpis volutpat sed.</p>");


    QString css_header_spec;

    if(use_em_header){
        css_header_spec=header_size + "em";
    }
    else{
        css_header_spec=header_size + "pt";
    }

    QString css_body_spec;

    if(use_em_body){
        css_body_spec=body_size + "em";
    }
    else{
        css_body_spec=body_size + "pt";
    }

    QString bgcolor;
    QString datebox_color;

    if(use_system_colors){

        // use system colors for datebox
        QPalette pal;
        QColor bg=pal.highlight().color();
        QColor fg=pal.highlightedText().color();
        bgcolor=bg.name();
        datebox_color=fg.name();
    }
    else{
        bgcolor="#e2e2e2";
        datebox_color="#000000";
    }

    QString output;

    output.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\""
                  "\"http://www.w3.org/TR/html4/strict.dtd\">\n");
    output.append("<html>\n");
    output.append("<head>\n");
    output.append("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n");
    output.append("<title>Sample Entry</title>\n");
    output.append("<style type=\"text/css\">\n");
    output.append("\tbody{\n\t\tbackground-color: #ffffff;\n\t}\n\n\n");
    output.append("\tp{\n\t\tfont-family: " + body_font + "; \n\t\tfont-size: " + css_body_spec + "; \n\t\tmargin-bottom: 1em;\n\t}\n\n\n");
    output.append("\th1{\n\t\tfont-family: " + header_font + "; \n\t\tfont-size: " + css_header_spec + ";\n\t}\n\n\n");

    if(use_rounded){
        output.append("\t.datebox{\n\t\tbackground-color: " + bgcolor + "; \n\t\tcolor: " + datebox_color + ";\n\t\tpadding: 5px; \n\t\tborder-radius: 10px; \n\t\tborder: solid 1px gray;\n\t}\n\n\n");
    }
    else{
         output.append("\t.datebox{\n\t\tbackground-color: " + bgcolor + "; \n\t\tcolor: " + datebox_color + ";\n\t\tpadding: 5px; \n\t\tborder: solid 1px gray;\n\t}\n\n\n");
    }

    output.append("\thr{\n\t\tbackground-color:silver;\n\t\tcolor: silver;\n\t\theight: 1px;\n\t\twidth:100%;\n\t\tborder: 0px;\n\t}\n\n\n");
    output.append("</style>\n");
    output.append("</head>\n");
    output.append("<body>\n");
    output.append("<h1>Sample Entry</h1>\n");
    output.append("<div class=\"datebox\">&nbsp;&nbsp;On [date] at [time], [user] wrote:</div>\n");
    output.append(lipsum);
    output.append("<hr>\n");
    output.append("<p><i><small>This file was generated by RoboJournal " +
                  Buffer::version +" on [export date & timestamp].</small></i></p>");
    output.append("</body>\n");
    output.append("</html>");


    return output;
}


