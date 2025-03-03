/*
Copyright (C) 2000-2006 Silicon Graphics, Inc.  All Rights Reserved.
Portions Copyright 2007-2010 Sun Microsystems, Inc. All rights reserved.
Portions Copyright 2009-2011 SN Systems Ltd. All rights reserved.
Portions Copyright 2008-2011 David Anderson. All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of version 2 of the GNU General
  Public License as published by the Free Software Foundation.

  This program is distributed in the hope that it would be
  useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  Further, this software is distributed without any warranty
  that it is free of the rightful claim of any third person
  regarding infringement or the like.  Any license provided
  herein, whether implied or otherwise, applies only to this
  software file.  Patent licenses, if any, provided herein
  do not apply to combinations of this program with other
  software, or any other product whatsoever.

  You should have received a copy of the GNU General Public
  License along with this program; if not, write the Free
  Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
  Boston MA 02110-1301, USA.
*/

#include "globals.h"
#include "naming.h"
#include "esb.h"
#include "esb_using_functions.h"
#include "print_sections.h"
#include "print_frames.h"
#include "sanitized.h"


/* Get all the data in .debug_funcnames
   An SGI extension.
   (For a long time, erroneously called .debug_static_funcs here).
   On error, this allows some dwarf memory leaks.
*/
extern int
print_static_funcs(Dwarf_Debug dbg,Dwarf_Error*err)
{
    Dwarf_Func *globbuf = NULL;
    Dwarf_Signed count = 0;
    int gfres = 0;
    char buf[DWARF_SECNAME_BUFFER_SIZE];
    struct esb_s truename;
    struct esb_s sanitname;

    glflags.current_section_id = DEBUG_FUNCNAMES;
    esb_constructor_fixed(&truename,buf,sizeof(buf));
    get_true_section_name(dbg,".debug_funcnames",
        &truename,TRUE);
    {
        esb_constructor(&sanitname);
        /*  Sanitized cannot be safely reused,there
            is a static buffer,
            so we make a safe copy. */
        esb_append(&sanitname,sanitized(esb_get_string(&truename)));
    }
    esb_destructor(&truename);
    if (glflags.verbose) {
        /* For best testing! */
        dwarf_return_empty_pubnames(dbg,1,err);
    }
    gfres = dwarf_get_funcs(dbg, &globbuf, &count, err);
    if (gfres == DW_DLV_ERROR) {
        printf("\n%s\n",esb_get_string(&sanitname));
        return gfres;
    } else if (gfres == DW_DLV_NO_ENTRY) {
        dwarf_return_empty_pubnames(dbg,0,err);
        esb_destructor(&sanitname);
        return gfres;
    } else {
        int pres = 0;
        int printed = FALSE;

        if (glflags.gf_do_print_dwarf && count > 0) {
            /* SGI specific so only mention if present. */
            printf("\n%s\n",esb_get_string(&sanitname));
            printed= TRUE;
        }
        pres = print_all_pubnames_style_records(dbg,
            "static-func",
            esb_get_string(&sanitname),
            (Dwarf_Global *)globbuf, count,
            err);
        dwarf_funcs_dealloc(dbg,globbuf,count);
        if (pres == DW_DLV_ERROR) {
            if (!printed) {
                printf("\n%s\n",esb_get_string(&sanitname));
            }
            dwarf_return_empty_pubnames(dbg,0,err);
            esb_destructor(&sanitname);
            return pres;
        }
    }
    dwarf_return_empty_pubnames(dbg,0,err);
    esb_destructor(&sanitname);
    return DW_DLV_OK;
}   /* print_static_funcs() */
