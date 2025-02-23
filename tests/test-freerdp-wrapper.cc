/*
 * Copyright © 2012 Canonical Ltd. All rights reserved.
 *
 * Author(s): David Barth <david.barth@canonical.com>
 *
 */

#include <gtest/gtest.h>

extern "C" {

#include "mock_pam.h"
#include "mock_guest.h"

#include "pam-freerdp-private.h"

	int freerdpclient_wrapper (int argc, char * argv[]);

const char * auth_check_path = AUTH_CHECK;

}

namespace {

  // The fixture for testing class Foo.
  class FreerdpclientWrapperTest : public ::testing::Test {
  protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    FreerdpclientWrapperTest() {
      // You can do set-up work for each test here.
		setenv("HOME", "/tmp", 1 /* overwrite */);
    }

    virtual ~FreerdpclientWrapperTest() {
      // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    virtual void SetUp() {
      // Code here will be called immediately after the constructor (right
      // before each test).
		unlink("/tmp/.freerdp-socket");
    }

    virtual void TearDown() {
      // Code here will be called immediately after each test (right
      // before the destructor).
		unlink("/tmp/.freerdp-socket");
    }

    // Objects declared here can be used by all tests in the test case for Foo.
  };

  TEST_F(FreerdpclientWrapperTest, canLinkTheWholeGang) {
	  EXPECT_EQ (1, 1); // right, that's trivial, but that means
	                    // that I got all of the wrapper and pam to link there
  }

  TEST_F(FreerdpclientWrapperTest, canHandleEmptyPassword) {
	  const char *argv[] = { NULL };

	  pam_handle_t *pamh = pam_handle_empty_pswd_new ();

	  EXPECT_EQ (PAM_AUTH_ERR,
				 pam_sm_authenticate (pamh, 0, 0, argv));

  }

  TEST_F(FreerdpclientWrapperTest, canCallPamOpenSession) {
	  const char *argv[] = { NULL };

	  pam_handle_t *pamh = pam_handle_new ();

	  EXPECT_EQ (PAM_SUCCESS,
				 pam_sm_authenticate (pamh, 0, 0, argv));
	  EXPECT_EQ (PAM_SUCCESS,
				 pam_sm_setcred (pamh, 0, 0, argv));

	  EXPECT_EQ (PAM_SUCCESS,
				 pam_sm_open_session (pamh, 0, 0, argv));
	  EXPECT_EQ(0, socket_sucker());
	  EXPECT_EQ (PAM_SUCCESS,
				 pam_sm_close_session (pamh, 0, 0, argv));
  }

}
