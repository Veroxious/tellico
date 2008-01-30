/*  This is RTF to HTML converter, implemented as a text filter, generally.
    Copyright (C) 2003 Valentin Lavrinenko, vlavrinenko@users.sourceforge.net

    available at http://rtf2html.sf.net

    Original available under the terms of the GNU LGPL2, and according
    to those terms, relicensed under the GNU GPL2 for inclusion in Tellico */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "rtf_keyword.h"

using namespace rtf;

rtf_keyword::keyword_map::keyword_map() : base_class()
{
   insert(value_type("b", rkw_b));
   insert(value_type("bin", rkw_bin));
   insert(value_type("blue", rkw_blue));
   insert(value_type("brdrnone", rkw_brdrnone));
   insert(value_type("bullet", rkw_bullet));
   insert(value_type("cb", rkw_cb));
   insert(value_type("cell", rkw_cell));
   insert(value_type("cellx", rkw_cellx));
   insert(value_type("cf", rkw_cf));
   insert(value_type("clbrdrb", rkw_clbrdrb));
   insert(value_type("clbrdrl", rkw_clbrdrl));
   insert(value_type("clbrdrr", rkw_clbrdrr));
   insert(value_type("clbrdrt", rkw_clbrdrt));
   insert(value_type("clvertalb", rkw_clvertalb));
   insert(value_type("clvertalc", rkw_clvertalc));
   insert(value_type("clvertalt", rkw_clvertalt));
   insert(value_type("clvmgf", rkw_clvmgf));
   insert(value_type("clvmrg", rkw_clvmrg));
   insert(value_type("colortbl", rkw_colortbl));
   insert(value_type("emdash", rkw_emdash));
   insert(value_type("emspace", rkw_emspace));
   insert(value_type("endash", rkw_endash));
   insert(value_type("enspace", rkw_enspace));
   insert(value_type("f", rkw_f));
   insert(value_type("fprq", rkw_fprq));
   insert(value_type("fcharset", rkw_fcharset));
   insert(value_type("fnil", rkw_fnil));
   insert(value_type("froman", rkw_froman));
   insert(value_type("fswiss", rkw_fswiss));
   insert(value_type("fmodern", rkw_fmodern));
   insert(value_type("fscript", rkw_fscript));
   insert(value_type("fdecor", rkw_fdecor));
   insert(value_type("ftech", rkw_ftech));
   insert(value_type("fbidi", rkw_fbidi));
   insert(value_type("field", rkw_field));
   insert(value_type("filetbl", rkw_filetbl));
   insert(value_type("fldrslt", rkw_fldrslt));
   insert(value_type("fonttbl", rkw_fonttbl));
   insert(value_type("footer", rkw_footer));
   insert(value_type("footerf", rkw_footerf));
   insert(value_type("fs", rkw_fs));
   insert(value_type("green", rkw_green));
   insert(value_type("header", rkw_header));
   insert(value_type("headerf", rkw_headerf));
   insert(value_type("highlight", rkw_highlight));
   insert(value_type("i", rkw_i));
   insert(value_type("info", rkw_info));
   insert(value_type("intbl", rkw_intbl));
   insert(value_type("ldblquote", rkw_ldblquote));
   insert(value_type("li", rkw_li));
   insert(value_type("line", rkw_line));
   insert(value_type("lquote", rkw_lquote));
   insert(value_type("margl", rkw_margl));
   insert(value_type("object", rkw_object));
   insert(value_type("paperw", rkw_paperw));
   insert(value_type("par", rkw_par));
   insert(value_type("pard", rkw_pard));
   insert(value_type("pict", rkw_pict));
   insert(value_type("plain", rkw_plain));
   insert(value_type("qc", rkw_qc));
   insert(value_type("qj", rkw_qj));
   insert(value_type("ql", rkw_ql));
   insert(value_type("qr", rkw_qr));
   insert(value_type("rdblquote", rkw_rdblquote));
   insert(value_type("red", rkw_red));
   insert(value_type("ri", rkw_ri));
   insert(value_type("row", rkw_row));
   insert(value_type("rquote", rkw_rquote));
   insert(value_type("sa", rkw_sa));
   insert(value_type("sb", rkw_sb));
   insert(value_type("sect", rkw_sect));
   insert(value_type("softline", rkw_softline));
   insert(value_type("stylesheet", rkw_stylesheet));
   insert(value_type("sub", rkw_sub));
   insert(value_type("super", rkw_super));
   insert(value_type("tab", rkw_tab));
   insert(value_type("title", rkw_title));
   insert(value_type("trleft", rkw_trleft));
   insert(value_type("trowd", rkw_trowd));
   insert(value_type("trrh", rkw_trrh));
   insert(value_type("ul", rkw_ul));
   insert(value_type("ulnone", rkw_ulnone));
}

rtf_keyword::keyword_map rtf_keyword::keymap;
