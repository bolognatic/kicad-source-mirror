/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2018 jp.charras at wanadoo.fr
 * Copyright (C) 2013 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <confirm.h>
#include <refdes_utils.h>

#include <sch_edit_frame.h>
#include <sch_reference_list.h>
#include <kicad_string.h>
#include <class_library.h>
#include <symbol_lib_table.h>

#include <netlist.h>
#include "netlist_exporter_orcadpcb2.h"


bool NETLIST_EXPORTER_ORCADPCB2::WriteNetlist( const wxString& aOutFileName,
                                               unsigned aNetlistOptions )
{
    (void)aNetlistOptions;      //unused
    FILE* f = NULL;
    wxString    field;
    wxString    footprint;
    int         ret = 0;        // zero now, OR in the sign bit on error
    wxString    netName;


    if( ( f = wxFopen( aOutFileName, wxT( "wt" ) ) ) == NULL )
    {
        wxString msg;
        msg.Printf( _( "Failed to create file \"%s\"" ), aOutFileName );
        DisplayError( NULL, msg );
        return false;
    }

    std::vector< SCH_REFERENCE > cmpList;

    ret |= fprintf( f, "( { %s created  %s }\n",
                        NETLIST_HEAD_STRING, TO_UTF8( DateAndTime() ) );

    // Create netlist module section
    m_ReferencesAlreadyFound.Clear();

    SCH_SHEET_LIST sheetList = m_schematic->GetSheets();

    for( unsigned i = 0;  i < sheetList.size();  i++ )
    {
        SCH_SHEET_PATH sheet = sheetList[i];

        // Process component attributes
        for( auto item : sheet.LastScreen()->Items().OfType( SCH_COMPONENT_T ) )
        {
            SCH_COMPONENT* comp = findNextComponent( item, &sheet );

            if( !comp )
                continue;

            CreatePinList( comp, &sheet );

            if( comp->GetPartRef() && comp->GetPartRef()->GetFootprints().GetCount() != 0  )
                cmpList.push_back( SCH_REFERENCE( comp, comp->GetPartRef().get(), sheet ) );

            footprint = comp->GetFootprint( &sheet );
            footprint.Replace( wxT( " " ), wxT( "_" ) );

            if( footprint.IsEmpty() )
                footprint = wxT( "$noname" );

            ret |= fprintf( f, " ( %s %s",
                            TO_UTF8( sheet.PathAsString() + comp->m_Uuid.AsString() ),
                            TO_UTF8( footprint ) );

            field = comp->GetRef( &sheet );

            ret |= fprintf( f, "  %s", TO_UTF8( field ) );

            field = comp->GetValue( &sheet );
            field.Replace( wxT( " " ), wxT( "_" ) );

            ret |= fprintf( f, " %s", TO_UTF8( field ) );

            ret |= fprintf( f, "\n" );

            // Write pin list:
            for( const PIN_INFO& pin : m_SortedComponentPinList )
            {
                netName = pin.netName;
                netName.Replace( wxT( " " ), wxT( "_" ) );

                ret |= fprintf( f, "  ( %4.4s %s )\n", TO_UTF8( pin.num ), TO_UTF8( netName ) );
            }

            ret |= fprintf( f, " )\n" );
        }
    }

    ret |= fprintf( f, ")\n*\n" );

    fclose( f );

    return ret >= 0;
}
