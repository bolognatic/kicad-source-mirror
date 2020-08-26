/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004-2020 KiCad Developers.
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

#include <class_board.h>
#include <class_track.h>
#include <class_via.h>
#include <class_pad.h>

#include <common.h>

#include <drc_proto/drc_engine.h>
#include <drc_proto/drc_item.h>
#include <drc_proto/drc_rule.h>
#include <drc_proto/drc_test_provider.h>


/*
    Via/pad annular ring width test. Checks if there's sufficient copper ring around PTH/NPTH holes (vias/pads)
    Errors generated:
    - DRCE_ANNULUS

    Todo: 
    - check pad holes too.
    - pad stack support (different IAR/OAR values depending on layer)
*/

namespace test
{

class DRC_TEST_PROVIDER_ANNULUS : public DRC_TEST_PROVIDER
{
public:
    DRC_TEST_PROVIDER_ANNULUS()
    {
    }

    virtual ~DRC_TEST_PROVIDER_ANNULUS()
    {
    }

    virtual bool Run() override;

    virtual const wxString GetName() const override
    {
        return "annulus";
    };

    virtual const wxString GetDescription() const override
    {
        return "Tests pad/via annular rings";
    }

    virtual std::set<test::DRC_CONSTRAINT_TYPE_T> GetMatchingConstraintIds() const override;
};

}; // namespace test


bool test::DRC_TEST_PROVIDER_ANNULUS::Run()
{
    if( !m_drcEngine->HasCorrectRulesForId(
                test::DRC_CONSTRAINT_TYPE_T::DRC_CONSTRAINT_TYPE_ANNULUS_WIDTH ) )
    {
        ReportAux( "No annulus constraints found. Skipping check." );
        return false;
    }

    ReportStage( ( "Testing via annular rings" ), 0, 2 );

    auto checkAnnulus = [&]( BOARD_ITEM* item ) -> bool 
    {
        bool fail_min = false, fail_max = false;
        int v_min, v_max;
        auto via = dyn_cast<VIA*>( item );

        // fixme: check minimum IAR/OAR ring for THT pads too
        if( !via )
            return true;

        test::DRC_CONSTRAINT constraint = m_drcEngine->EvalRulesForItems(
                test::DRC_CONSTRAINT_TYPE_T::DRC_CONSTRAINT_TYPE_ANNULUS_WIDTH, via );

        int annulus = ( via->GetWidth() - via->GetDrillValue() ) / 2;

        if( constraint.Value().HasMin() )
        {
            v_min = constraint.Value().Min();
            fail_min = annulus < v_min;
        }

        if( constraint.Value().HasMax() )
        {
            v_max = constraint.Value().Max();
            fail_max = annulus > v_max;
        }

        if( fail_min || fail_max )
        {
            std::shared_ptr<DRC_ITEM> drcItem = DRC_ITEM::Create( DRCE_ANNULUS );
            wxString                  msg;

            msg.Printf( drcItem->GetErrorText() + _( " (%s; actual annulus %s, constraint %s %s)" ),
                    constraint.GetParentRule()->GetName(),
                    MessageTextFromValue( userUnits(), annulus, true ),
                    fail_min ? _( "minimum" ) : _( "maximum" ),
                    MessageTextFromValue( userUnits(), fail_min ? v_min : v_max, true ) );

            drcItem->SetErrorMessage( msg );
            drcItem->SetItems( item );
            drcItem->SetViolatingRule( constraint.GetParentRule() );

            ReportWithMarker( drcItem, via->GetPosition() );

            if( isErrorLimitExceeded( DRCE_ANNULUS ) )
                return false;

        }

        return true;
    };

    forEachGeometryItem( { PCB_VIA_T }, LSET::AllCuMask(), checkAnnulus );

    reportRuleStatistics();

    return true;
}


std::set<test::DRC_CONSTRAINT_TYPE_T>
test::DRC_TEST_PROVIDER_ANNULUS::GetMatchingConstraintIds() const
{
    return { DRC_CONSTRAINT_TYPE_T::DRC_CONSTRAINT_TYPE_ANNULUS_WIDTH };
}


namespace detail
{
static test::DRC_REGISTER_TEST_PROVIDER<test::DRC_TEST_PROVIDER_ANNULUS> dummy;
}