/* 
   Copyright (C) 2013 by Ronnie Sahlberg <ronniesahlberg@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include <CUnit/CUnit.h>

#include "iscsi.h"
#include "scsi-lowlevel.h"
#include "iscsi-support.h"
#include "iscsi-test-cu.h"

void
test_preventallow_2_itnexuses(void)
{
	int ret;
	struct scsi_device sd2;

	CHECK_FOR_SBC;
	CHECK_FOR_REMOVABLE;

	logging(LOG_VERBOSE, LOG_BLANK_LINE);
	logging(LOG_VERBOSE, "Test that PREVENT MEDIUM REMOVAL are seen on other nexuses as well");

	logging(LOG_VERBOSE, "Set the PREVENT flag");
	ret = preventallow(sd, 1);
	CU_ASSERT_EQUAL(ret, 0);

	logging(LOG_VERBOSE, "Try to eject the medium");
	ret = startstopunit(sd, 0, 0, 0, 0, 1, 0,
			    EXPECT_REMOVAL_PREVENTED);
	CU_ASSERT_EQUAL(ret, 0);

	logging(LOG_VERBOSE, "Verify we can still access the media.");
	ret = testunitready(sd,
			    EXPECT_STATUS_GOOD);
	CU_ASSERT_EQUAL(ret, 0);

	logging(LOG_VERBOSE, "Create a second connection to the target");
	sd2.iscsi_ctx = iscsi_context_login(initiatorname2, sd->iscsi_url, &sd->iscsi_lun);
	if (sd2.iscsi_ctx == NULL) {
		logging(LOG_VERBOSE, "Failed to login to target");
		return;
	}

	logging(LOG_VERBOSE, "Try to eject the medium on the second connection");
	ret = startstopunit(&sd2, 0, 0, 0, 0, 1, 0,
			    EXPECT_REMOVAL_PREVENTED);
	CU_ASSERT_EQUAL(ret, 0);


	logging(LOG_VERBOSE, "Logout the second connection from target");
	iscsi_logout_sync(sd2.iscsi_ctx);
	iscsi_destroy_context(sd2.iscsi_ctx);



	logging(LOG_VERBOSE, "Clear PREVENT and load medium in case target failed");
	logging(LOG_VERBOSE, "Test we can clear PREVENT flag");
	ret = preventallow(sd, 0);
	CU_ASSERT_EQUAL(ret, 0);

	logging(LOG_VERBOSE, "Load the medium");
	ret = startstopunit(sd, 0, 0, 0, 0, 1, 1,
			    EXPECT_STATUS_GOOD);
	CU_ASSERT_EQUAL(ret, 0);

}
